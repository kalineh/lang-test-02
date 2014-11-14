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
		IntermediateOperation,
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

typedef std::shared_ptr<Intermediate> IntermediatePtr;
typedef std::shared_ptr<IntermediateBlock> IntermediateBlockPtr;
typedef std::vector<IntermediateBlockPtr> IntermediateBlockList;
typedef std::vector<IntermediatePtr> IntermediateList;

struct Intermediate
{
	virtual const int GetTypeId() { return IntermediateType::Intermediate; }
	virtual const char* ToStringType() { return "Intermediate"; }
	virtual std::string ToStringValue() { return std::string("<nul>"); } 
};

struct IntermediateOperation
	: Intermediate
{
	IntermediateOperation() { } 
	IntermediateOperation(const Operation& value) : value(value) { }
	IntermediateOperation(Operation::Type value) : value(value) { }

	virtual const int GetTypeId() { return IntermediateType::IntermediateOperation; }
	virtual const char* ToStringType() { return "IntermediateOperation"; }
	virtual std::string ToStringValue() { return std::string(value.ToString()); }

	Operation value;
};

struct IntermediateLiteralInteger
	: Intermediate
{
	IntermediateLiteralInteger() { } 
	IntermediateLiteralInteger(int value) : value(value) { }

	virtual const int GetTypeId() { return IntermediateType::IntermediateLiteralInteger; }
	virtual const char* ToStringType() { return "IntermediateLiteralInteger"; }
	virtual std::string ToStringValue() { return slow_lexical_cast<std::string>(value); }

	int value;
};

struct IntermediateLiteralFloat
	: Intermediate
{
	IntermediateLiteralFloat() { } 
	IntermediateLiteralFloat(float value) : value(value) { }

	virtual const int GetTypeId() { return IntermediateType::IntermediateLiteralFloat; }
	virtual const char* ToStringType() { return "IntermediateLiteralFloat"; }
	virtual std::string ToStringValue() { return slow_lexical_cast<std::string>(value); }

	float value;
};

struct IntermediateLiteralString
	: Intermediate
{
	IntermediateLiteralString() { } 
	IntermediateLiteralString(const char* value) : value(value) { }
	IntermediateLiteralString(const std::string& value) : value(value) { }

	virtual const int GetTypeId() { return IntermediateType::IntermediateLiteralString; }
	virtual const char* ToStringType() { return "IntermediateLiteralString"; }
	virtual std::string ToStringValue() { return value; }

	std::string value;
};

struct IntermediateLiteralIdentifier
	: Intermediate
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
	virtual const int GetTypeId() { return IntermediateType::IntermediateBlock; }
	virtual const char* ToStringType() { return "IntermediateBlock"; }
	virtual std::string ToStringValue() { return "block"; }

	IntermediateList value;
};

struct IntermediateFunction
	: Intermediate
{
	virtual const int GetTypeId() { return IntermediateType::IntermediateFunction; }
	virtual const char* ToStringType() { return "IntermediateFunction"; }
	virtual std::string ToStringValue() { return "function"; }

	IntermediatePtr identifier;
	// TODO: args
	// TODO: block
};

struct IntermediateCall
	: Intermediate
{
	virtual const int GetTypeId() { return IntermediateType::IntermediateCall; }
	virtual const char* ToStringType() { return "IntermediateCall"; }
	virtual std::string ToStringValue() { return "call"; }

	IntermediatePtr identifier;
	IntermediateList args;
};

struct IntermediateStore
	: Intermediate
{
	virtual const int GetTypeId() { return IntermediateType::IntermediateStore; }
	virtual const char* ToStringType() { return "IntermediateStore"; }
	virtual std::string ToStringValue() { return "store"; }

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
