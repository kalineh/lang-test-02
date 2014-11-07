
#include "core.h"
#include "ast.h"

struct Compiler
{
	ASTNodePtr compile_file(const char* src);
	ASTNodePtr compile_str(const char* src);
};
