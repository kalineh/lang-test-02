
#include "core.h"

struct Test
{
	const char* name;
	bool (*func)(void*);
	void* arg;
};

#define TEST_LOG(format, ...) printf(format, __VA_ARGS__); printf("\n");
#define TEST_INIT() g_tests = new Test[1024];
#define TEST_RELEASE() delete[] g_tests; g_test_count = 0;
#define TEST_RUN() g_run_tests();
#define TEST_ADD(x, a) \
	g_tests[g_test_count].name = #x; \
	g_tests[g_test_count].func = &x; \
	g_tests[g_test_count].arg = (void*)a; \
	g_test_count++;
#define TEST_CHECK(cond) \
	if (!cond) { \
		TEST_LOG("failure:"); \
		TEST_LOG(#cond); \
		return false; \
	}


extern Test* g_tests;
extern int g_test_count;

void g_run_tests();

