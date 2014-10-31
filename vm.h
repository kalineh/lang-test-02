
#include "core.h"

struct VM
{
	enum BC
	{
		BC_PUSH_INT,
		BC_PUSH_FLOAT,
		BC_PUSH_STRING,
		BC_PUSH_FUNC,

		BC_POP_INT,
		BC_POP_FLOAT,
		BC_POP_STRING,
		BC_POP_FUNC,
		// 
		BC_CALL,
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
		void* arg;
	};

	char* _stack;
	char* _stack_ptr;

	StackDebugInfo* _debug_info;


	void init();
	void release();

	void exec(INST inst);
	void execn(INST* insts, int count);

	const char* bcstr(BC bc);
	int size(TY type);

	void push(TY type, void* value);
	void pop(TY type);

	void call();
};
