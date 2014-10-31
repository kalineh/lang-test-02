
#include "core.h"

bool test1(void* arg)
{
	VM::INST insts[] =
	{
		{ VM::BC_PUSH_INT, NULL, },
		{ VM::BC_POP_INT, NULL, },
	};

	VM* vm = (VM*)arg;

	vm->execn(insts, countof(insts));

	return true;
}

int main(int argc, char** argv)
{
	VM vm;

	vm.init();

	TEST_INIT();

	TEST_ADD(test1, &vm);
	//TEST_ADD(test2, &vm);

	TEST_RUN();

	TEST_RELEASE();

	vm.release();

	return 0;
}


