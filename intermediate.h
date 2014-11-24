#pragma once

#include "core.h"

#include <string>
#include <memory>

#include "operation.h"

struct IntermediateType
{
	enum Type
	{
		Intermediate,
		IntermediateExpression,
		IntermediateDeclarationImplicit,
		IntermediateDeclarationExplicit,
		IntermediateAssignment,
		IntermediateBinaryOperation,
		IntermediateLiteralInteger,
		IntermediateLiteralFloat,
		IntermediateLiteralString,
		IntermediateLiteralIdentifier,
		IntermediateBlock,
		IntermediateFunction,
		IntermediateCall,
		IntermediateStore,
		IntermediateLoad,
		IntermediateNew,
		IntermediateDelete,
	};
};

struct Intermediate;
struct IntermediateBlock;
struct IntermediateExpression;

typedef std::shared_ptr<Intermediate> IntermediatePtr;
typedef std::shared_ptr<IntermediateBlock> IntermediateBlockPtr;
typedef std::shared_ptr<IntermediateExpression> IntermediateExpressionPtr;

typedef std::vector<IntermediateBlockPtr> IntermediateBlockList;
typedef std::vector<IntermediatePtr> IntermediateList;

typedef std::shared_ptr<IntermediateList> IntermediateListPtr;


struct Intermediate
{
	virtual const int GetTypeId() { return IntermediateType::Intermediate; }
	virtual const char* ToStringType() { return "Intermediate"; }
	virtual std::string ToStringValue() { return std::string("<nul>"); } 
};

struct IntermediateExpression
	: Intermediate
{
	IntermediateExpression() { }

	virtual const int GetTypeId() { return IntermediateType::IntermediateExpression; }
	virtual const char* ToStringType() { return "IntermediateExpression"; }
	virtual std::string ToStringValue() { return std::string("<expr>"); } 
};

struct IntermediateDeclarationImplicit
	: IntermediateExpression
{
	IntermediateDeclarationImplicit() { }
	IntermediateDeclarationImplicit(IntermediateExpressionPtr lhs, IntermediateExpressionPtr rhs) : lhs(lhs), rhs(rhs) { }

	virtual const int GetTypeId() { return IntermediateType::IntermediateDeclarationImplicit; }
	virtual const char* ToStringType() { return "IntermediateDeclarationImplicit"; }
	virtual std::string ToStringValue() { return lhs->ToStringValue() + " := " + rhs->ToStringValue(); }

	IntermediateExpressionPtr lhs;
	IntermediateExpressionPtr rhs;
};

struct IntermediateDeclarationExplicit
	: IntermediateExpression
{
	IntermediateDeclarationExplicit() { }
	IntermediateDeclarationExplicit(IntermediatePtr type, IntermediateExpressionPtr lhs, IntermediateExpressionPtr rhs) : type(type), lhs(lhs), rhs(rhs) { }

	virtual const int GetTypeId() { return IntermediateType::IntermediateDeclarationExplicit; }
	virtual const char* ToStringType() { return "IntermediateDeclarationExplicit"; }
	virtual std::string ToStringValue() { return lhs->ToStringValue() + " : " + type->ToStringValue() + " = " + rhs->ToStringValue(); }

	IntermediatePtr type;
	IntermediateExpressionPtr lhs;
	IntermediateExpressionPtr rhs;
};

struct IntermediateAssignment
	: IntermediateExpression
{
	IntermediateAssignment() { }
	IntermediateAssignment(IntermediateExpressionPtr lhs, IntermediateExpressionPtr rhs) : lhs(lhs), rhs(rhs) { }

	virtual const int GetTypeId() { return IntermediateType::IntermediateAssignment; }
	virtual const char* ToStringType() { return "IntermediateAssignment"; }
	virtual std::string ToStringValue() { return lhs->ToStringValue() + " = " + rhs->ToStringValue(); }

	IntermediateExpressionPtr lhs;
	IntermediateExpressionPtr rhs;
};

struct IntermediateBinaryOperation
	: IntermediateExpression
{
	IntermediateBinaryOperation() { }
	IntermediateBinaryOperation(Operation::Type op, IntermediateExpressionPtr lhs, IntermediateExpressionPtr rhs) : op(op), lhs(lhs), rhs(rhs) { }

	virtual const int GetTypeId() { return IntermediateType::IntermediateBinaryOperation; }
	virtual const char* ToStringType() { return "IntermediateBinaryOperation"; }
	virtual std::string ToStringValue() { return lhs->ToStringValue() + " " + Operation::ToString(op) + " " + rhs->ToStringValue(); }

	Operation::Type op;
	IntermediateExpressionPtr lhs;
	IntermediateExpressionPtr rhs;
};

struct IntermediateLiteralInteger
	: IntermediateExpression
{
	IntermediateLiteralInteger() { } 
	IntermediateLiteralInteger(int value) : value(value) { }

	virtual const int GetTypeId() { return IntermediateType::IntermediateLiteralInteger; }
	virtual const char* ToStringType() { return "IntermediateLiteralInteger"; }
	virtual std::string ToStringValue() { return slow_lexical_cast<std::string>(value); }

	int value;
};

struct IntermediateLiteralFloat
	: IntermediateExpression
{
	IntermediateLiteralFloat() { } 
	IntermediateLiteralFloat(float value) : value(value) { }

	virtual const int GetTypeId() { return IntermediateType::IntermediateLiteralFloat; }
	virtual const char* ToStringType() { return "IntermediateLiteralFloat"; }
	virtual std::string ToStringValue() { return slow_lexical_cast<std::string>(value); }

	float value;
};

struct IntermediateLiteralString
	: IntermediateExpression
{
	IntermediateLiteralString() { } 
	IntermediateLiteralString(const char* value) : value(value) { }
	IntermediateLiteralString(const std::string& value) : value(value) { }

	virtual const int GetTypeId() { return IntermediateType::IntermediateLiteralString; }
	virtual const char* ToStringType() { return "IntermediateLiteralString"; }
	virtual std::string ToStringValue() { return "'" + value + "'"; }

	std::string value;
};

struct IntermediateLiteralIdentifier
	: IntermediateExpression
{
	IntermediateLiteralIdentifier() { } 
	IntermediateLiteralIdentifier(const char* value) : value(value) { }
	IntermediateLiteralIdentifier(const std::string& value) : value(value) { }

	virtual const int GetTypeId() { return IntermediateType::IntermediateLiteralIdentifier; }
	virtual const char* ToStringType() { return "IntermediateLiteralIdentifier"; }
	virtual std::string ToStringValue() { return value; }

	std::string value;
};

struct IntermediateBlock
	: Intermediate
{
	IntermediateBlock()
	{
		value = std::make_shared<IntermediateList>();
	}

	virtual const int GetTypeId() { return IntermediateType::IntermediateBlock; }
	virtual const char* ToStringType() { return "IntermediateBlock"; }
	virtual std::string ToStringValue() { return "block"; }

	IntermediateListPtr value;
};

struct IntermediateFunction
	: Intermediate
{
	IntermediateFunction() { } 
	IntermediateFunction(IntermediatePtr ident) : ident(ident) { }
	IntermediateFunction(IntermediatePtr ident, IntermediateListPtr args, IntermediateBlockPtr block) : ident(ident), args(args), block(block) { }

	virtual const int GetTypeId() { return IntermediateType::IntermediateFunction; }
	virtual const char* ToStringType() { return "IntermediateFunction"; }
	virtual std::string ToStringValue() { return "function"; }

	IntermediatePtr ident;
	IntermediateListPtr args;
	IntermediateBlockPtr block;
};

struct IntermediateCall
	: Intermediate
{
	IntermediateCall() { }
	IntermediateCall(IntermediatePtr ident) : ident(ident) { }
	IntermediateCall(IntermediatePtr ident, IntermediateListPtr args) : ident(ident), args(args) { }

	virtual const int GetTypeId() { return IntermediateType::IntermediateCall; }
	virtual const char* ToStringType() { return "IntermediateCall"; }
	virtual std::string ToStringValue()
	{
		std::string s = ident->ToStringValue();
		s += "(";
		for (auto a : *args)
			s += a->ToStringValue();
		s += ")";
		return s;
	}

	IntermediatePtr ident;
	IntermediateListPtr args;
};

struct IntermediateStore
	: Intermediate
{
	virtual const int GetTypeId() { return IntermediateType::IntermediateStore; }
	virtual const char* ToStringType() { return "IntermediateStore"; }
	virtual std::string ToStringValue() { return "store." + type->ToStringValue(); }

	IntermediatePtr type;
	IntermediatePtr lhs;
	IntermediatePtr rhs;
};

struct IntermediateLoad
	: Intermediate
{
	virtual const int GetTypeId() { return IntermediateType::IntermediateLoad; }
	virtual const char* ToStringType() { return "IntermediateLoad"; }
	virtual std::string ToStringValue() { return "load"; }
};

struct IntermediateNew
	: Intermediate
{
	virtual const int GetTypeId() { return IntermediateType::IntermediateNew; }
	virtual const char* ToStringType() { return "IntermediateNew"; }
	virtual std::string ToStringValue() { return "new"; }

	Intermediate identifier;
	Intermediate type;
};

struct IntermediateDelete
	: Intermediate
{
	virtual const int GetTypeId() { return IntermediateType::IntermediateDelete; }
	virtual const char* ToStringType() { return "IntermediateDelete"; }
	virtual std::string ToStringValue() { return "delete"; }

	Intermediate identifier;
};
