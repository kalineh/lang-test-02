
#include "core.h"

void VM::init()
{
	const int StackSize = 8 KB;

	_stack = (mem8*)malloc(STACK_SIZE);
	_code = (INST*)malloc(CODE_SIZE * sizeof(INST));
	_data = (mem8*)malloc(DATA_SIZE);

	memset(_stack, 0, STACK_SIZE);
	memset(_code, 0, CODE_SIZE * sizeof(INST));
	memset(_data, 0, DATA_SIZE);
	
    _top = 0;
	_ip = 0;

	_ftable = (int*)malloc(FUNCTION_LIMIT * sizeof(int));
	memset(_ftable, 0, FUNCTION_LIMIT * sizeof(int));

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
			_top -= SZ;
			mem8* read = _stack + _top;
			mem8* write = _stack + _top + (arg * SZ);
			mem32 val = *ptr32(read);
			write32(write, val);
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

		case BC_LOAD:
		{
			mem8* read = _stack + _top + (arg * SZ);
			mem8* write = _stack + _top;
			mem32 vptr = *ptr32(read);
			mem32 val = _data[vptr];
			write32(write, val);
			break;
		}

		case BC_STORE:
		{
			mem8* read = _stack + _top;
			mem8* write = _stack + _top + (arg * SZ);
			mem32 val = *ptr32(read);
			mem32 vptr = *ptr32(write);
			_data[vptr] = val;
			break;
		}

		case BC_CALL:
		{
			mem8* read = _stack + _top;
			mem32 fid = *ptr32(read);
			*ptr32(read) = _ip + 1;
			_ip = _ftable[fid];
			_top += SZ;
			break;
		}

		case BC_RETN:
		{
			mem8* read = _stack + _top;
			mem32 retn = *ptr32(read);
			_ip = retn;
			_top -= SZ;
			break;
		}

		case BC_BRK:
		{
			raise(SIGINT);
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
		case BC_BRK: return "BRK";

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

void VM::print_stack(int count)
{
	LOG("[VM] Dumping stack (%d bytes, top = %d)...", STACK_SIZE, _top);

	if (count == 0)
	{
		count = STACK_SIZE / 4;
	}

	for (int i = 0; i < count; i += 4)
	{
		LOG("[VM] [%4d] %02x %02x %02x %02x (%d)",
			i,
			_stack[i + 0],
			_stack[i + 1],
			_stack[i + 2],
			_stack[i + 3],
			*ptr32(&_stack[i + 0])
		);
	}

	LOGN("[VM] Stack dump complete.");
}

void VM::print_code(int count)
{
}

void VM::print_data(int count)
{
}

