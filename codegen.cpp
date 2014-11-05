
/*
#include "node.h"
#include "codegen.h"
#include "parser.hpp"

#include <llvm/PassManager.h>

#include <typeinfo>
using namespace std;

void CodeGenContext::generateCode(NBlock& root)
{
	std::cout << "Generating code...\n";

	vector<Type*> argTypes;
	FunctionType* ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), makeArrayRef(argTypes), false);
	mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "main", module);
	BasicBlock* bblock = BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);

	pushBlock(bblock);
	root.codeGen(*this);
	ReturnInst::Create(getGlobalContext(), bblock);
	popBlock();

	std::cout << "Code generation complete.\n";

	PassManager pm;
	pm.add(createPrintModulePass(outs()));
	pm.run(*module);

	//Pass* printer = createPrintModulePass(outs());
	//ModulePassManager pm;
	//pm.addPass(printer);
	//pm.run(module);
}

GenericValue CodeGenContext::runCode()
{
	std::cout << "Executing code...\n";

	ExecutionEngine* ee = EngineBuilder(module).create();
	vector<GenericValue> noargs;
	GenericValue v = ee->runFunction(mainFunction, noargs);

	std::cout << "Code execution complete.\n";

	return v;
}

static Type* typeOf(const NIdentifier& type)
{
	if (type.name.compare("int") == 0)
	{
		return Type::getInt64Ty(getGlobalContext());
	}

	if ( type.name.compare("double") == 0)
	{
		return Type::getDoubleTy(getGlobalContext());
	}

	return Type::getVoidTy(getGlobalContext());
}


llvm::Value* NInteger::codeGen(CodeGenContext& context)
{
	std::cout << "Creating integer: " << value << endl;

	return ConstantInt::get(Type::getInt64Ty(getGlobalContext()), value, true);
}

llvm::Value* NDouble::codeGen(CodeGenContext& context)
{
	std::cout << "Creating double: " << value << endl;

	return ConstantFP::get(Type::getDoubleTy(getGlobalContext()), value);
}

llvm::Value* NIdentifier::codeGen(CodeGenContext& context)
{
	std::cout << "Creating identifier reference: " << name << endl;
	
	if (context.locals().find(name) == context.locals().end())
	{
		std::cerr << "undeclared variable " << name << endl;
	}

	return new LoadInst(context.locals()[name], "", false, context.currentBlock());
}

llvm::Value* NMethodCall::codeGen(CodeGenContext& context)
{
	std::cout << "Creating method call: " << id.name << endl;

	Function* function = context.module->getFunction(id.name.c_str());

	if (function == NULL)
	{
		std::cerr << "no such function " << id.name << endl;
	}

	std::vector<Value*> args;

	ExpressionList::const_iterator a = arguments.begin();
	ExpressionList::const_iterator b = arguments.end();

	for (; a != b; ++a)
	{
		args.push_back((*a)->codeGen(context));
	}

	return CallInst::Create(function, makeArrayRef(args), "", context.currentBlock());
}

llvm::Value* NBinaryOperator::codeGen(CodeGenContext& context)
{
	std::cout << "Creating binary operation " << op << endl;

	Instruction::BinaryOps instr;

	switch (op)
	{
		case TPLUS:		instr = Instruction::Add; goto math;
		case TMINUS:	instr = Instruction::Sub; goto math; 
		case TMUL: 		instr = Instruction::Mul; goto math;
		case TDIV:		instr = Instruction::FDiv; goto math;

		// TODO: comparison
	}

	return NULL;

math:
	return BinaryOperator::Create(instr, lhs.codeGen(context), rhs.codeGen(context), "", context.currentBlock());
}

llvm::Value* NAssignment::codeGen(CodeGenContext& context)
{
	std::cout << "Creating assignment for " << lhs.name << endl;

	if (context.locals().find(lhs.name) == context.locals().end())
	{
		std::cerr << "undeclared variable " << lhs.name << endl;
		return NULL;
	}

	return new StoreInst(rhs.codeGen(context), context.locals()[lhs.name], false, context.currentBlock());
}

llvm::Value* NBlock::codeGen(CodeGenContext& context)
{
	std::cout << "Creating block..." << endl;

	StatementList::const_iterator a = statements.begin();
	StatementList::const_iterator b = statements.end();

	Value* last = NULL;

	for (; a != b; ++a)
	{
		std::cout << "Generating code for " << typeid(**a).name() << endl;
		last = (*a)->codeGen(context);
	}

	return last;
}

llvm::Value* NExpressionStatement::codeGen(CodeGenContext& context)
{
	std::cout << "Generating code for " << typeid(expression).name() << endl;

	return expression.codeGen(context);
}

llvm::Value* NVariableDeclaration::codeGen(CodeGenContext& context)
{
	std::cout << "Creating variable declaration " << type.name << " " << id.name << endl;

	AllocaInst* alloc = new AllocaInst(typeOf(type), id.name.c_str(), context.currentBlock());
	context.locals()[id.name] = alloc;

	if (assignmentExpr != NULL)
	{
		NAssignment assn(id, *assignmentExpr);
		assn.codeGen(context);
	}

	return alloc;
}

llvm::Value* NFunctionDeclaration::codeGen(CodeGenContext& context)
{
	std::cout << "Creating function: " << id.name << endl;

	vector<Type*> argTypes;
	
	VariableList::const_iterator a = arguments.begin();
	VariableList::const_iterator b = arguments.end();

	for (; a != b; ++a)
	{
		argTypes.push_back(typeOf((*a)->type));
	}

	FunctionType* ftype = FunctionType::get(typeOf(type), makeArrayRef(argTypes), false);
	Function* function = Function::Create(ftype, GlobalValue::InternalLinkage, id.name.c_str(), context.module);
	BasicBlock* bblock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);

	context.pushBlock(bblock);

	Function::arg_iterator argsValues = function->arg_begin();
	Value* argumentValue;

	a = arguments.begin();
	b = arguments.end();
	
	for (; a != b; ++a)
	{
		(*a)->codeGen(context);

		argumentValue = argsValues++;
		argumentValue->setName((*a)->id.name.c_str());
		StoreInst* inst = new StoreInst(argumentValue, context.locals()[(*a)->id.name], false, bblock);
	}

	block.codeGen(context);
	ReturnInst::Create(getGlobalContext(), context.getCurrentReturnValue(), bblock);

	context.popBlock();

	return function;
}

*/