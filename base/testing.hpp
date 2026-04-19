#include "base.hpp"

struct TestRunner {
	DynArray<Test> tests;
};

TestRunner tests_create(){}

TestRunner tests_add(TestRunner* r)

struct Test {
	TestRunner* parent_runner;

	String name;
	usize total;
	usize failed;
};

void t_expect(Test* t, bool predicate, String msg, caller_srcloc){
	t->total += 1;
	if(!predicate){}
}


otal += 1;
	if(!predicate){}
}


