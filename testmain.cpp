#include "base/testing.hpp"
#include "base/virtual.hpp"
#include "base/test.cpp"

int test_exit = 0;

void testmain(){
	auto r = tests_create();

	base_tests(r);

	if(!tests_run(r)){
		test_exit = 1;
	}
}

int main(){
	virtual_init();
	testmain();
	return test_exit;
}

//// Single library TU
#include "base/base.cpp"
#include "base/libcpp_shim.cpp"

// TODO: Get rid of this
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
