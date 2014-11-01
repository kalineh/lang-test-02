
#include "core.h"

struct VM
{
	enum BC
	{
		// stack[top] = arg;
		// top++;
		BC_PUSH_INT,
		BC_PUSH_FLOAT,
		BC_PUSH_STRING,
		BC_PUSH_FUNC,

		// stack[top + arg] = stack[top];
		// top--;
		BC_POP_INT,
		BC_POP_FLOAT,
		BC_POP_STRING,
		BC_POP_FUNC,

		// stack[top] = stack[top + arg];
		// top++;
		BC_DUP,

		// stack[top] = ip;
		// ip = stack[top-1];
		// top++;
		BC_CALL,

		// ip = stack[top];
		// top--;
		BC_RETN,
	};

	enum TY
	{
		TY_INT,
		TY_FLOAT,
		TY_STRING,
		TY_FUNC,
	};

	struct StackDebugInfo
	{
		int id;
		int offset;
		BC bc;

		union {
			void* arg_void;
			int arg_int;
			float arg_float;
			char* arg_str;
		};
	};

	struct INST
	{
		BC bc;
		int32_t arg;
	};

	struct FRAME
	{
		INST* insts;
	};

	mem8* _stack;
	mem8* _top;
	mem8* _ip;
	mem8* _memory;

	StackDebugInfo* _debug_info;

	void init();
	void release();

	void exec(INST inst);
	void execn(INST* insts, int count);

	const char* bcstr(BC bc);
	int size(TY type);

	void step();
};
