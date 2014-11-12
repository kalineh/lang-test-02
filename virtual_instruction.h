#pragma once

#include "core.h"

#include <string>
#include <memory>

#include "operation.h"

struct VirtualInstructionType
{
	enum Type
	{
		VirtualInstruction,
		VirtualInstructionOperation,
		VirtualInstructionLiteralInteger,
		VirtualInstructionLiteralFloat,
		VirtualInstructionLiteralString,
		VirtualInstructionLiteralIdentifier,
		VirtualInstructionBlock,
		VirtualInstructionFunction,
		VirtualInstructionCall,
		VirtualInstructionStore,
		VirtualInstructionLoad,
		VirtualInstructionNew,
		VirtualInstructionDelete,
	};
};

struct VirtualInstruction
{
	virtual const int GetTypeId() { return VirtualInstructionType::VirtualInstruction; }
	virtual const char* ToStringType() { return "VirtualInstruction"; }
	virtual std::string ToStringValue() { return std::string("<nul>"); } 
};

typedef std::shared_ptr<VirtualInstruction> VirtualInstructionPtr;
typedef std::vector<VirtualInstructionPtr> VirtualInstructionList;

struct VirtualInstructionOperation
	: VirtualInstruction
{
	VirtualInstructionOperation() { } 
	VirtualInstructionOperation(const Operation& value) : value(value) { }
	VirtualInstructionOperation(Operation::Type value) : value(value) { }

	virtual const int GetTypeId() { return VirtualInstructionType::VirtualInstructionOperation; }
	virtual const char* ToStringType() { return "VirtualInstructionOperation"; }
	virtual std::string ToStringValue() { return std::string(value.ToString()); }

	Operation value;
};

struct VirtualInstructionLiteralInteger
	: VirtualInstruction
{
	VirtualInstructionLiteralInteger() { } 
	VirtualInstructionLiteralInteger(int value) : value(value) { }

	virtual const int GetTypeId() { return VirtualInstructionType::VirtualInstructionLiteralInteger; }
	virtual const char* ToStringType() { return "VirtualInstructionLiteralInteger"; }
	virtual std::string ToStringValue() { return slow_lexical_cast<std::string>(value); }

	int value;
};

struct VirtualInstructionLiteralFloat
	: VirtualInstruction
{
	VirtualInstructionLiteralFloat() { } 
	VirtualInstructionLiteralFloat(float value) : value(value) { }

	virtual const int GetTypeId() { return VirtualInstructionType::VirtualInstructionLiteralFloat; }
	virtual const char* ToStringType() { return "VirtualInstructionLiteralFloat"; }
	virtual std::string ToStringValue() { return slow_lexical_cast<std::string>(value); }

	float value;
};

struct VirtualInstructionLiteralString
	: VirtualInstruction
{
	VirtualInstructionLiteralString() { } 
	VirtualInstructionLiteralString(const char* value) : value(value) { }
	VirtualInstructionLiteralString(const std::string& value) : value(value) { }

	virtual const int GetTypeId() { return VirtualInstructionType::VirtualInstructionLiteralString; }
	virtual const char* ToStringType() { return "VirtualInstructionLiteralString"; }
	virtual std::string ToStringValue() { return value; }

	std::string value;
};

struct VirtualInstructionLiteralIdentifier
	: VirtualInstruction
{
	VirtualInstructionLiteralIdentifier() { } 
	VirtualInstructionLiteralIdentifier(const char* value) : value(value) { }
	VirtualInstructionLiteralIdentifier(const std::string& value) : value(value) { }

	virtual const int GetTypeId() { return VirtualInstructionType::VirtualInstructionLiteralIdentifier; }
	virtual const char* ToStringType() { return "VirtualInstructionLiteralIdentifier"; }
	virtual std::string ToStringValue() { return value; }

	std::string value;
};

struct VirtualInstructionBlock
	: VirtualInstruction
{
	virtual const int GetTypeId() { return VirtualInstructionType::VirtualInstructionBlock; }
	virtual const char* ToStringType() { return "VirtualInstructionBlock"; }
	virtual std::string ToStringValue() { return "block"; }

	VirtualInstructionList value;
};

struct VirtualInstructionFunction
	: VirtualInstruction
{
	virtual const int GetTypeId() { return VirtualInstructionType::VirtualInstructionFunction; }
	virtual const char* ToStringType() { return "VirtualInstructionFunction"; }
	virtual std::string ToStringValue() { return "function"; }

	VirtualInstructionPtr identifier;
	// TODO: args
	// TODO: block
};

struct VirtualInstructionCall
	: VirtualInstruction
{
	virtual const int GetTypeId() { return VirtualInstructionType::VirtualInstructionCall; }
	virtual const char* ToStringType() { return "VirtualInstructionCall"; }
	virtual std::string ToStringValue() { return "call"; }

	VirtualInstructionPtr identifier;
	VirtualInstructionList args;
};

struct VirtualInstructionStore
	: VirtualInstruction
{
	virtual const int GetTypeId() { return VirtualInstructionType::VirtualInstructionStore; }
	virtual const char* ToStringType() { return "VirtualInstructionStore"; }
	virtual std::string ToStringValue() { return "store"; }

	VirtualInstructionPtr lhs;
	VirtualInstructionPtr rhs;
};

struct VirtualInstructionLoad
	: VirtualInstruction
{
	virtual const int GetTypeId() { return VirtualInstructionType::VirtualInstructionLoad; }
	virtual const char* ToStringType() { return "VirtualInstructionLoad"; }
	virtual std::string ToStringValue() { return "load"; }
};

struct VirtualInstructionNew
	: VirtualInstruction
{
	virtual const int GetTypeId() { return VirtualInstructionType::VirtualInstructionNew; }
	virtual const char* ToStringType() { return "VirtualInstructionNew"; }
	virtual std::string ToStringValue() { return "new"; }

	VirtualInstruction identifier;
	VirtualInstruction type;
};

struct VirtualInstructionDelete
	: VirtualInstruction
{
	virtual const int GetTypeId() { return VirtualInstructionType::VirtualInstructionDelete; }
	virtual const char* ToStringType() { return "VirtualInstructionDelete"; }
	virtual std::string ToStringValue() { return "delete"; }

	VirtualInstruction identifier;
};
