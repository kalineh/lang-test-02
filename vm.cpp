
#include "core.h"

void VM::init()
{
	const int StackSize = 8 KB;

	_stack = (char*)malloc(StackSize);
	_debug_info = (StackDebugInfo*)malloc(sizeof(StackDebugInfo) * StackSize);

	memset(_debug_info, 0, sizeof(StackDebugInfo) * StackSize);

	LOGN("[VM] initialized");
}

void VM::release()
{
	free(_stack);
	free(_debug_info);

	LOGN("[VM] released");
}

void VM::exec(INST inst)
{
	LOG("[VM] exec: %8s: arg: 0x%p", bcstr(inst.bc), inst.arg);
}

void VM::execn(INST* insts, int count)
{
	for (int i = 0; i < count; ++i)
	{
		exec(insts[i]);
	}
}

const char* VM::bcstr(VM::BC bc)
{
	switch (bc)
	{
		case TY_INT: return "int";
		case TY_FLOAT: return "float";
		case TY_STRING: return "string";
		case TY_FUNC: return "func";

		default:
			ASSERTN(false);
			return 0;
	}
}

int VM::size(VM::TY type)
{
	switch (type)
	{
		case TY_INT: return 4;
		case TY_FLOAT: return 4;
		case TY_STRING: return 4;
		case TY_FUNC: return 4;

		default:
			ASSERTN(false);
			return 0;
	}
}

void VM::push(VM::TY type, void* value)
{
	_stack_ptr += size(type);
}

void VM::pop(VM::TY type)
{
	_stack_ptr -= size(type);
}

void VM::call()
{
	// ?	
}
