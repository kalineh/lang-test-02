
#include "core.h"

#define MAKE_INST( bc, val ) { (VM::BC_##bc), cast32(val), }

bool test_assign_literal(void* arg)
{
	// x := 42;
	VM::INST insts[] =
	{
		MAKE_INST(PUSH_INT, 42),	// 42
		MAKE_INST(POP_INT, 0),		// 
	};

	VM* vm = (VM*)arg;

	vm->execn(insts, countof(insts));

	TEST_CHECK((vm->_stack[SZ * 0] == 42));
	TEST_CHECK((vm->_top == 0));

	memset(vm->_stack, 0, STACK_SIZE);
	memset(vm->_code, 0, CODE_SIZE);
	memset(vm->_data, 0, DATA_SIZE);

	return true;
}

bool test_assign_literal_multi(void* arg)
{
	// x := 42;
	// y := 100
	VM::INST insts[] =
	{
		MAKE_INST(PUSH_INT, 42),	// 42
		MAKE_INST(PUSH_INT, 100),	// 42, 100
		MAKE_INST(POP_INT, 0),		// 42
		MAKE_INST(POP_INT, 0),		// 
	};

	VM* vm = (VM*)arg;

	vm->execn(insts, countof(insts));

	TEST_CHECK((cast32(vm->_stack[SZ * 0]) == 42));
	TEST_CHECK((cast32(vm->_stack[SZ * 1]) == 100));
	TEST_CHECK((vm->_top == 0));

	memset(vm->_stack, 0, STACK_SIZE);
	memset(vm->_code, 0, CODE_SIZE);
	memset(vm->_data, 0, DATA_SIZE);

	return true;
}

bool test_assign_from_local(void* arg)
{
	// x := 42;
	// y := x
	// z := x
	VM::INST insts[] =
	{
		MAKE_INST(PUSH_INT, 42),	// 42
		MAKE_INST(DUP, -1),			// 42, 42
		MAKE_INST(DUP, -2),			// 42, 42, 42
		MAKE_INST(POP_INT, 0),		// 42, 42
		MAKE_INST(POP_INT, 0),		// 42
		MAKE_INST(POP_INT, 0),		// 
	};

	VM* vm = (VM*)arg;

	vm->execn(insts, countof(insts));

	TEST_CHECK((cast32(vm->_stack[SZ * 0]) == 42));
	TEST_CHECK((cast32(vm->_stack[SZ * 1]) == 42));
	TEST_CHECK((cast32(vm->_stack[SZ * 2]) == 42));
	TEST_CHECK((vm->_top == 0));

	memset(vm->_stack, 0, STACK_SIZE);
	memset(vm->_code, 0, CODE_SIZE);
	memset(vm->_data, 0, DATA_SIZE);

	return true;
}

bool test_func_decl(void* arg)
{
	// f0 := () {};
	// f1 := () { return 1; }
	// f2 := (x) { return x + x; }

	const int fid0 = 100;
	const int fid1 = 101;
	const int fid2 = 102;

	VM::INST insts[] =
	{
		MAKE_INST(PUSH_FUNC, fid0),		// f0
		MAKE_INST(PUSH_FUNC, fid1),		// f0, f1
		MAKE_INST(PUSH_FUNC, fid2),		// f0, f1, f2
		MAKE_INST(POP_FUNC, 0),			// f0, f1
		MAKE_INST(POP_FUNC, 0),			// f0
		MAKE_INST(POP_FUNC, 0),			// 
	};

	VM* vm = (VM*)arg;

	vm->execn(insts, countof(insts));

	TEST_CHECK((vm->_top == 0));

	memset(vm->_stack, 0, STACK_SIZE);
	memset(vm->_code, 0, CODE_SIZE);
	memset(vm->_data, 0, DATA_SIZE);

	return true;
}

bool test_func_call_fake(void* arg)
{
	// f0 := () { return 5; };
	// x := f();

	const int fid0 = 101;

	VM* vm = (VM*)arg;

	VM::INST insts[] =
	{
		MAKE_INST(PUSH_FUNC, fid0),		// f0
		MAKE_INST(PUSH_INT, 0),			// f0, 0

		MAKE_INST(PUSH_INT, 0),			// f0, 0, retv
		MAKE_INST(PUSH_FUNC, fid0),		// f0, 0, retv, fid0

			MAKE_INST(PUSH_INT, 5),		// f0, 0, retv, retn, 5
			MAKE_INST(POP_INT, -2),		// f0, 0, retv.5, retn
			MAKE_INST(RETN, 0),			// f0, 0, retv.5

		MAKE_INST(POP_INT, -1),			// f0, 5
		MAKE_INST(POP_INT, 0),			// f0
		MAKE_INST(POP_FUNC, 0),			// 
	};

	vm->execn(insts, countof(insts));

	//vm->print_stack(32);

	TEST_CHECK((cast32(vm->_stack[0]) == fid0));
	TEST_CHECK((cast32(vm->_stack[4]) == 5));
	TEST_CHECK((cast32(vm->_stack[8]) == 5));
	TEST_CHECK((vm->_top == 0));

	memset(vm->_stack, 0, STACK_SIZE);
	memset(vm->_code, 0, CODE_SIZE);
	memset(vm->_data, 0, DATA_SIZE);

	return true;
}

bool test_int_ptr(void* arg)
{
	// x := 1;
	// p := &x;
	// y := *p;
	// *p = 2;

	const int fid0 = 100;

	VM::INST insts[] =
	{
		MAKE_INST(PUSH_INT, 1),		// 1
		MAKE_INST(ADDR, -1),		// 1, &x
		MAKE_INST(DUP, 0),			// 1, &x, !&x
		MAKE_INST(LOAD, -2),		// 1, &x, *x
		MAKE_INST(PUSH_INT, 2),		// 1, &x, *x, 2
		MAKE_INST(STORE, -3),		// 1, &x, *x, 2
		MAKE_INST(POP_INT, 0),		// 1, &x, *x
		MAKE_INST(POP_INT, 0),		// 1, &x
		MAKE_INST(POP_PTR, 0),		// 1
		MAKE_INST(POP_INT, 0),		//
	};

	VM* vm = (VM*)arg;

	vm->execn(insts, countof(insts));

	TEST_CHECK((vm->_top == 0));

	memset(vm->_stack, 0, STACK_SIZE);
	memset(vm->_code, 0, CODE_SIZE);
	memset(vm->_data, 0, DATA_SIZE);

	return true;
}

bool test_arithmetic(void* arg)
{
	// x := 1 + 2
	// y := x + x

	VM::INST insts[] =
	{
		MAKE_INST(PUSH_INT, 0),		// x
		MAKE_INST(PUSH_INT, 1),		// x, 1
		MAKE_INST(PUSH_INT, 2),		// x, 1, 2
		MAKE_INST(ADD_INT, -3),		// 3
		MAKE_INST(PUSH_INT, 0),		// 3, y
		MAKE_INST(DUP, -2),			// 3, y, 3
		MAKE_INST(DUP, -3),			// 3, y, 3, 3
		MAKE_INST(ADD_INT, -3),		// 3, 6
		MAKE_INST(POP_INT, 0),		// 3
		MAKE_INST(POP_INT, 0),		// 
	};

	VM* vm = (VM*)arg;

	vm->execn(insts, countof(insts));

	TEST_CHECK((vm->_top == 0));
	TEST_CHECK((vm->_stack[0] == 3));
	TEST_CHECK((vm->_stack[4] == 6));

	memset(vm->_stack, 0, STACK_SIZE);
	memset(vm->_code, 0, CODE_SIZE);
	memset(vm->_data, 0, DATA_SIZE);

	return true;
}

bool test_func_call_real(void* arg)
{
	// f0 := () { return 5; };
	// x := f();

	const int fid0 = 100;
	const int faddr0 = 256;

	VM::INST f0_insts[] =
	{
		MAKE_INST(PUSH_INT, 5),		// retv, retn | 5
		MAKE_INST(POP_INT, -2),		// 5 retn |
		MAKE_INST(RETN, 0),			// 5 |
	};

	VM* vm = (VM*)arg;

	memcpy(vm->_code + faddr0, f0_insts, sizeof(f0_insts));

	VM::INST insts[] =
	{
		MAKE_INST(PUSH_FUNC, fid0),		// f0
		MAKE_INST(PUSH_INT, 0),			// f0, 0

		MAKE_INST(PUSH_INT, 0),			// f0, 0, retv
		MAKE_INST(CALL, fid0),			// f0, 0, retv, retn

		MAKE_INST(POP_INT, -1),			// f0, 5, retv
		MAKE_INST(POP_INT, 0),			// f0
		MAKE_INST(POP_FUNC, 0),			// 
	};

	vm->_ftable[fid0] = faddr0;
	vm->_ip = 0;
	vm->_top = 0;

	memcpy(vm->_code, insts, sizeof(insts));
	memcpy(vm->_code + faddr0, f0_insts, sizeof(f0_insts));

	vm->run();

	//vm->print_stack(32);
	//vm->print_code(16);

	TEST_CHECK((vm->_top == 0));
	TEST_CHECK((cast32(vm->_stack[0]) == fid0));
	TEST_CHECK((cast32(vm->_stack[4]) == 5));
	TEST_CHECK((cast32(vm->_stack[8]) == 5));

	memset(vm->_stack, 0, STACK_SIZE);
	memset(vm->_code, 0, CODE_SIZE);
	memset(vm->_data, 0, DATA_SIZE);

	return true;
}

#include <iostream>
#include <fstream>
#include <memory>
#include "lexer.h"
#include "parser.h"
#include "translator.h"

int main(int argc, char** argv)
{
	auto in = std::ifstream("../test.kl", std::ios::in | std::ios::binary);
	auto input = std::string(
		std::istreambuf_iterator<char>(in),
		std::istreambuf_iterator<char>()
	);

	in.close();

	auto lexer = std::make_shared<Lexer>(input.c_str());
	auto parser = std::make_shared<Parser>(lexer);
	auto translator = std::make_shared<Translator>(parser);

	//lexer->Print();
	//parser->Print();
	translator->Print();

	return 0;

	VM vm;

	vm.init();

	TEST_INIT();

	TEST_ADD(test_assign_literal, &vm);
	TEST_ADD(test_assign_literal_multi, &vm);
	TEST_ADD(test_assign_from_local, &vm);
	TEST_ADD(test_func_decl, &vm);
	TEST_ADD(test_func_call_fake, &vm);
	TEST_ADD(test_int_ptr, &vm);
	TEST_ADD(test_arithmetic, &vm);

	//TEST_ADD(test_ip_simple, &vm);
	TEST_ADD(test_func_call_real, &vm);

	TEST_RUN();

	TEST_RELEASE();

	vm.release();

	return 0;
}
