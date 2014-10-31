
#include "core.h"

Test* g_tests = NULL;
int g_test_count = 0;

void g_run_tests()
{
	int passed = 0;

	for (int i = 0; i < g_test_count; ++i)
	{
		Test* test = &g_tests[i];

		TEST_LOG("[TEST] [%d] %s: ", i, test->name);

		bool result = (*test->func)(test->arg);


		passed += result ? 1 : 0;
	}

	TEST_LOG("[TEST] tests %d/%d passed.", passed, g_test_count);
}
