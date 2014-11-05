
#include "core.h"

#include <iostream>
#include "node.h"
#include "codegen.h"

extern int yyparse();
extern void yyparse_init(const char*);
extern void yyparse_cleanup();

extern NBlock* programBlock;

void createCoreFunctions(CodeGenContext& context) {}

int main(int argc, char** argv)
{
        if (argc <= 1)
        {
                std::cout << "error: no input files" << std::endl;
                return -1;
        }

        for (int i = 1; i < argc; ++i)
        {
                std::cout << "Compiling file " << argv[i] << std::endl;

                yyparse_init( argv[ 1 ] );
                yyparse();
                yyparse_cleanup();
        }

        std::cout << "Compilation complete: " << programBlock << std::endl;

        InitializeNativeTarget();
        CodeGenContext context;
        createCoreFunctions(context);
        context.generateCode(*programBlock);
        context.runCode();

        return 0;
}
~                                                                                                                                                                         
~                   
