#pragma once
#include "base.hpp"
#include "memory.hpp"
#include "dyn_array.hpp"
#include "arena.hpp"

extern "C" int printf(const char*, ...);

struct TestRunner;
struct Test;

using TestFunc = void (*)(Test*);

struct TestMessage {
	String msg;
	sourcelocation loc;
};

struct Test {
	TestRunner* parent_runner;

	String name;
	Allocator allocator;
	usize total;
	usize failed;
	TestFunc func;
	DynArray<TestMessage> messages;
};

struct TestRunner {
	DynArray<Test> tests;
};

Arena* tests_get_arena(){
	static Arena a{};
	if(!a.data){
		a = Arena::from_virtual(64 * mem_megabyte, 1 * mem_megabyte);
	}
	return &a;
}

TestRunner* tests_create(){
	auto a = tests_get_arena()->allocator();
	auto runner = make<TestRunner>(a);
	runner->tests = make_dyn_array<Test>(a);
	return runner;
}

bool tests_run(TestRunner* r){
	bool all_ok = true;
	for(usize i = 0; i < len(r->tests); i++){
		auto test = &r->tests[i];
		test->func(test);

		if(test->failed > 0){
			all_ok = false;
		}

		if(len(test->name)){
			printf(
				"%s: %.*s\n",
				test->failed == 0 ? "PASS" : "FAIL",
				int(len(test->name)), raw_data(test->name)
			);
		}
		else {
			printf(
				"%s: <unnamed test #%d\n",
				test->failed == 0 ? "PASS" : "FAIL",
				int(i)
			);
		}

		for(usize j = 0; j < len(test->messages); j++){
			auto msg = test->messages[j];
			printf(
				"  (%s:%d) %.*s\n",
				msg.loc.file_name(), msg.loc.line(),
				int(len(msg.msg)), raw_data(msg.msg)
			);
		}
	}
	return all_ok;
}

void tests_add(TestRunner* r, TestFunc func){
	Test t = {
		.parent_runner = r,
		.name = "",
		.allocator = heap_allocator(), // TODO: test_allocator()
		.total = 0,
		.failed = 0,
		.func = func,
		.messages = make_dyn_array<TestMessage>(tests_get_arena()->allocator()),
	};
	append(&r->tests, t);
}

bool t_expect(Test* t, bool predicate, String msg, sourcelocation loc = sourcelocation::current()){
	t->total += 1;
	if(!predicate){
		t->failed += 1;
		append(&t->messages, {msg, loc});
	}
	return predicate;
}

