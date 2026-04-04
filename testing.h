#define MAX_TEST_COUNT 5000

typedef struct {
	char const* name;
	int failed;
	int total;
} Test;

typedef void (*TestFunc)(Test* t);

typedef struct {
	Test t;
	TestFunc f;
} TestCase;

void test_run(char const* name, TestFunc f){
	Test t = {
		.name = name,
		.failed = 0,
		.total = 0,
	};

	f(&t);

	printf("[%s] %s: ", name, t.failed > 0 ? "FAIL" : "PASS");
	printf("ok in %d/%d", t.total - t.failed, t.total);
}

void test_ensure_ex(Test* t, int predicate , const char* msg,char const* file, int line){
	t->total += 1;
	if(!predicate){
		printf("[!!! %s:%d] %s\n", file, line, msg);
		t->failed += 1;
	}
}

#define t_ensure(t, p, m) test_ensure_ex((t), (p), (m), __FILE__, __LINE__)

#define t_expect(t, p) test_ensure_ex((t), (p), "failed expect: " #p, __FILE__, __LINE__)

#define t_eq(t, l, r) test_ensure_ex((t), (l) == (r), "not equal: " #l " != " #r, __FILE__, __LINE__)

#define t_neq(t, l, r) test_ensure_ex((t), (l) != (r), "equal: " #l " == " #r, __FILE__, __LINE__)

// TestCase test_buf[MAX_TEST_COUNT];
// int test_buf_len = 0;

// void test_register(TestCase const* c){
// 	test_buf[test_buf_len] = *c;
// 	test_buf[test_buf_len].failed = 0;
// 	test_buf[test_buf_len].total = 0;
// }
