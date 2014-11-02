
#include "core.h"

void VM::init()
{
	const int StackSize = 8 KB;

	_stack = (mem8*)malloc(StackSize);
	memset(_stack, 0, StackSize);
	
    _top = 0;
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
	LOG("[VM] exec: %8s: arg: %d", bcstr(inst.bc), inst.arg);

	const BC bc = inst.bc;	
	const mem32 arg = cast32(inst.arg);

	switch (bc)
	{
		case BC_PUSH_INT:
		case BC_PUSH_FLOAT:
		case BC_PUSH_STRING:
		case BC_PUSH_FUNC:
		case BC_PUSH_PTR:
		{
			write32(&_stack[_top], arg);
			_top += SZ;
			break;
		}

		case BC_POP_INT:
		case BC_POP_FLOAT:
		case BC_POP_STRING:
		case BC_POP_FUNC:
		case BC_POP_PTR:
		{
			mem8* write = _stack + _top;
			mem32 val = *ptr32(&_stack[_top]);
			write32(write, val);
			_top -= SZ;
			break;
		}

		case BC_DUP:
		{
			mem8* read = _stack + _top + (arg * SZ);
			mem8* write = _stack + _top;
			mem32 val = *ptr32(read);
			write32(write, val);
			_top += SZ;
			break;
		}

		case BC_CALL:
		{
			// ptr dst = stack[top];
			// stack[top] = ip + 1
			// ip = dst;
			break;
		}

		case BC_RETN:
		{
			// ip = stack[top]
			// top--;
			break;
		}

		default:
			ASSERTN(false);
			break;
	}	
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
		case BC_PUSH_PTR: return "PUSH_PTR";
		case BC_POP_INT: return "POP_INT";
		case BC_POP_FLOAT: return "POP_FLOAT";
		case BC_POP_STRING: return "POP_STRING";
		case BC_POP_FUNC: return "POP_FUNC";
		case BC_POP_PTR: return "POP_PTR";
		case BC_DUP: return "DUP";
		case BC_LOAD: return "LOAD";
		case BC_STORE: return "STORE";
		case BC_CALL: return "CALL";
		case BC_RETN: return "RETN";

		default:
			ASSERTN(false);
			return "bad bytecode";
	}
}

int VM::size(VM::TY type)
{
	switch (type)
	{
		case TY_INT: return SZ;
		case TY_FLOAT: return SZ;
		case TY_STRING: return SZ;
		case TY_FUNC: return SZ;
		case TY_PTR: return SZ;

		default:
			ASSERTN(false);
			return SZ;
	}
}

void VM::step()
{
}
