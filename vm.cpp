
#include "core.h"

void VM::init()
{
	_memblock = (mem8*)malloc(STACK_SIZE + CODE_SIZE + DATA_SIZE);

	_stack = (mem8*)_memblock;
	_code = (INST*)_memblock + STACK_SIZE;
	_data = (mem8*)_memblock + STACK_SIZE + CODE_SIZE;

	memset(_stack, 0, STACK_SIZE);
	memset(_code, 0, CODE_SIZE);
	memset(_data, 0, DATA_SIZE);
	
    _top = 0;
	_ip = 0;

	_ftable = (int*)malloc(FUNCTION_LIMIT * sizeof(int));
	memset(_ftable, 0, FUNCTION_LIMIT * sizeof(int));

	const int StackDebugInfoSize = sizeof(StackDebugInfo) * STACK_SIZE / SZ;
	_stack_debug_info = (StackDebugInfo*)malloc(StackDebugInfoSize);
	memset(_stack_debug_info, 0, StackDebugInfoSize);

	// fnuc debug info

	LOGN("[VM] initialized");
}

void VM::release()
{
	free(_memblock);
	free(_stack_debug_info);

	LOGN("[VM] released");
}

void VM::step()
{
	INST inst = _code[_ip];
	exec(inst);
	_ip++;
}

void VM::exec(INST inst)
{
	LOG("[VM] [ip:%04d top:%04d] %8s: arg: %d", _ip, _top, bcstr(inst.bc), inst.arg);

	const BC bc = inst.bc;	
	const mem32 arg = cast32(inst.arg);

	switch (bc)
	{
		case BC_HALT:
			LOG("[VM] execution halt at instruction %d", _ip);
			break;	

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
			// todo: validate same pushed type

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

		case BC_ADDR:
		{
			mem8* write = _stack + _top;
			mem32 val = _top + (arg * SZ);
			write32(write, val);
			_top += SZ;
			break;
		}

		case BC_LOAD:
		{
			mem8* read = _stack + _top + (arg * SZ);
			mem8* write = _stack + _top;
			mem32 vptr = *ptr32(read);
			mem32 val = _memblock[vptr];
			write32(write, val);
			break;
		}

		case BC_STORE:
		{
			mem8* read = _stack + _top;
			mem8* write = _stack + _top + (arg * SZ);
			mem32 val = *ptr32(read);
			mem32 vptr = *ptr32(write);
			_memblock[vptr] = val;
			break;
		}

		case BC_CALL:
		{
			mem8* read = _stack + _top;
			mem32 fid = *ptr32(read);
			*ptr32(read) = _ip;
			_ip = _ftable[arg] - 1;
			_top += SZ;
			break;
		}

		case BC_RETN:
		{
			mem8* read = _stack + _top;
			mem32 retn = *ptr32(read);
			// todo: why is this -2, not -1?
			_ip = retn - 2;
			_top -= SZ;
			break;
		}

		case BC_ADD_INT:
		{
			mem8* lhs = _stack + _top - SZ * 1;
			mem8* rhs = _stack + _top - SZ * 2;
			mem8* write = _stack + _top + (arg * SZ);
			*ptr32(write) = *(int32_t*)lhs + *(int32_t*)rhs;
			_top -= SZ * 2;
			break;
		}

		case BC_SUB_INT:
		{
			mem8* lhs = _stack + _top - SZ * 1;
			mem8* rhs = _stack + _top - SZ * 2;
			mem8* write = _stack + _top + (arg * SZ);
			*ptr32(write) = *(int32_t*)lhs - *(int32_t*)rhs;
			_top -= SZ * 2;
			break;
		}

		case BC_MUL_INT:
		{
			mem8* lhs = _stack + _top - SZ * 1;
			mem8* rhs = _stack + _top - SZ * 2;
			mem8* write = _stack + _top + (arg * SZ);
			*ptr32(write) = *(int32_t*)lhs * *(int32_t*)rhs;
			_top -= SZ * 2;
			break;
		}

		case BC_DIV_INT:
		{
			mem8* lhs = _stack + _top - SZ * 1;
			mem8* rhs = _stack + _top - SZ * 2;
			mem8* write = _stack + _top + (arg * SZ);
			*ptr32(write) = *(int32_t*)lhs / *(int32_t*)rhs;
			_top -= SZ * 2;
			break;
		}

		case BC_MOD_INT:
		{
			mem8* lhs = _stack + _top - SZ * 1;
			mem8* rhs = _stack + _top - SZ * 2;
			mem8* write = _stack + _top + (arg * SZ);
			*ptr32(write) = *(int32_t*)lhs % *(int32_t*)rhs;
			_top -= SZ * 2;
			break;
		}

		case BC_BRK:
		{
			raise(SIGINT);
			break;
		}

		case BC_DUMP_STACK:
		{
			print_stack(arg);
			break;
		}

		case BC_DUMP_CODE:
		{
			print_code(arg);
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

		ASSERTN((_top & 0x4) == 0);
		ASSERTN(_top >= 0);
		ASSERTN(_top < STACK_SIZE);

		ASSERTN(_ip >= 0);
		ASSERTN(_ip < CODE_SIZE);
	}
}

void VM::run()
{
	while (_code[_ip].bc != BC_HALT)
	{
		step();
	}
}

const char* VM::bcstr(VM::BC bc)
{
	switch (bc)
	{
		case BC_HALT: return "HALT";
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
		case BC_ADDR: return "ADDR";
		case BC_LOAD: return "LOAD";
		case BC_STORE: return "STORE";
		case BC_CALL: return "CALL";
		case BC_RETN: return "RETN";
		case BC_ADD_INT: return "ADD_INT";
		case BC_SUB_INT: return "SUB_INT";
		case BC_MUL_INT: return "MUL_INT";
		case BC_DIV_INT: return "DIV_INT";
		case BC_MOD_INT: return "MOD_INT";
		case BC_BRK: return "BRK";

		case BC_DUMP_STACK: return "DUMP_STACK";
		case BC_DUMP_CODE: return "DUMP_CODE";

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

void VM::print_stack(int count)
{
	LOG("[VM] Dumping stack (%d bytes, top = %d, ip = %d)", STACK_SIZE, _top, _ip);

	if (count == 0)
	{
		count = STACK_SIZE / 4;
	}

	for (int i = 0; i < count; i += 4)
	{
		LOG("[VM] [%4d] %02x %02x %02x %02x (%d)%s",
			i,
			_stack[i + 0],
			_stack[i + 1],
			_stack[i + 2],
			_stack[i + 3],
			*ptr32(&_stack[i + 0]),
			(i == _top) ? " <-- top" : ""
		);
	}

	LOGN("[VM] Stack dump complete.");
}

void VM::print_code(int count)
{
	LOG("[VM] Dumping code (%d bytes, top = %d, ip = %d)", STACK_SIZE, _top, _ip);

	if (count == 0)
	{
		count = CODE_SIZE / sizeof(INST);
	}

	for (int i = 0; i < count; ++i)
	{
		LOG("[VM] [%03d] %s : %d%s",
			i,
			bcstr(_code[i].bc),
			_code[i].arg,
			(i == _ip) ? " <-- ip" : ""
		);
	}
}

void VM::print_data(int count)
{
}

