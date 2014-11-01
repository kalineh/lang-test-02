
#include "core.h"

void VM::init()
{
	const int StackSize = 8 KB;

	_stack = (char*)malloc(StackSize);
	_ip = 0;

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
	LOG("[VM] exec: %8s: arg: %d", bcstr(inst.bc), (int)(int64_t(inst.arg) & 0xFFFFFFFF));
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
		case BC_PUSH_INT: return "PUSH_INT";
		case BC_PUSH_FLOAT: return "PUSH_FLOAT";
		case BC_PUSH_STRING: return "PUSH_STRING";
		case BC_PUSH_FUNC: return "PUSH_FUNC";
		case BC_POP_INT: return "POP_INT";
		case BC_POP_FLOAT: return "POP_FLOAT";
		case BC_POP_STRING: return "POP_STRING";
		case BC_POP_FUNC: return "POP_FUNC";
		case BC_DUP: return "DUP";
		case BC_CALL: return "CALL";
		case BC_RETN: return "RETN";

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
