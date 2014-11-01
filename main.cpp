
#include "core.h"


/*

var f = () { return 10; };
var x = 1;
var y = f();
var z = x + y;

.func
pop args
push 10
retn

.stackalloc 4

// f = func
push func
pop stack[0]

// x = 1
push 1
pop stack[1]

// y = f()
push args
push func
call
pop func
pop stack[3]

// z = x + y
push x
push y
add
pop stack[4]

// when insts are generated from ast
// we produce a stack and a map of local var to stackpos
// so we can pull a local var with (DUP, offset-from-top)
// and assign with (POP, offset-from-top)
// the messy part is that the offsets are order-dependent

*/

#define MAKE_INST( bc, val ) { (VM::BC_##bc), cast32(val), }

bool test_assign_literal(void* arg)
{
	// x := 42;
	VM::INST insts[] =
	{
		MAKE_INST( PUSH_INT, 42 ),
		MAKE_INST( POP_INT, 0 ),
	};

	VM* vm = (VM*)arg;

	vm->execn(insts, countof(insts));

	return true;
}

bool test_assign_literal_multi(void* arg)
{
	// x := 42;
	// y := 100
	VM::INST insts[] =
	{
		MAKE_INST(PUSH_INT, 42),
		MAKE_INST(PUSH_INT, 100),
		MAKE_INST(POP_INT, 0),
		MAKE_INST(POP_INT, 0),
	};

	VM* vm = (VM*)arg;

	vm->execn(insts, countof(insts));

	return true;
}

bool test_assign_from_local(void* arg)
{
	// x := 42;
	// y := x
	// z := x
	VM::INST insts[] =
	{
		MAKE_INST(PUSH_INT, 42),
		MAKE_INST(DUP, 0),
		MAKE_INST(DUP, -1),
		MAKE_INST(POP_INT, 0),
	};

	VM* vm = (VM*)arg;

	vm->execn(insts, countof(insts));

	return true;
}

bool test_func_decl(void* arg)
{
	// f0 := () {};
	// f1 := () { return 1; }
	// f2 := (x) { return x + x; }

	const int fid0 = 0;
	const int fid1 = 1;
	const int fid2 = 2;

	VM::INST insts[] =
	{
		MAKE_INST(PUSH_FUNC, fid0),
		MAKE_INST(PUSH_FUNC, fid1),
		MAKE_INST(PUSH_FUNC, fid2),
		MAKE_INST(POP_FUNC, 0),
		MAKE_INST(POP_FUNC, 0),
		MAKE_INST(POP_FUNC, 0),
	};

	VM* vm = (VM*)arg;

	vm->execn(insts, countof(insts));

	return true;
}

bool test_func_call(void* arg)
{
	// x := 1
	// f0 := (a, b) { return int; };
	// f(x, 2);

	const int fid0 = 0;

	VM::INST insts[] =
	{
		MAKE_INST(PUSH_INT, 1),
		MAKE_INST(DUP, -1),
		MAKE_INST(PUSH_INT, 2),
		MAKE_INST(CALL, fid0),
		MAKE_INST(POP_FUNC, -1),
		MAKE_INST(POP_INT, -1),
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

	TEST_ADD(test_assign_literal, &vm);
	TEST_ADD(test_assign_literal_multi, &vm);
	TEST_ADD(test_assign_from_local, &vm);
	TEST_ADD(test_func_decl, &vm);
	TEST_ADD(test_func_call, &vm);

	TEST_RUN();

	TEST_RELEASE();

	vm.release();

	return 0;
}


