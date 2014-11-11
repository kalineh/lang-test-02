#pragma once

#include "core.h"
#include "parser.h"

#include <string>
#include <strstream>
#include <iostream>
#include <vector>
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


struct WriterText
{
	void Write(VirtualInstructionPtr node);

private:
	void WriteNode(VirtualInstructionPtr node, int level = 0);
};

struct WriterVM
{
};

struct WriterLLVM
{
};

struct Translator : Process
{
	struct Exception { };
	struct Unsupported : Exception { };

	VirtualInstructionList instructions;
	// TODO: is this needed?
	//VirtualInstructionList stack;

	explicit Translator(std::shared_ptr<Parser> p);

	std::string Result() const;
	const VirtualInstructionList& ResultInstructions() const;

private:
	typedef Parser::NodePtr NodePtr;

	void Traverse(NodePtr node);
	void TranslateFunction(NodePtr node);
	void TranslateBlock(NodePtr node);
	void Translate(NodePtr node);
	void TranslateBinaryOp(NodePtr node, Operation::Type);

	// TODO: not create strings on each call: store the conversion in a map or something
	std::string ConvertOp(NodePtr node);

	void TranslateFromToken(NodePtr node);
	void TranslateCall(NodePtr node);
	void TranslateIndex(NodePtr node);

	VirtualInstructionPtr Top();

	void PushNew();

	void Append(VirtualInstructionPtr instruction);

	template <class T>
	void AppendNew(const T& t)
	{
		Append(std::make_shared<T>(t));
	}

	void AppendNewOp(Operation::Type op);

	VirtualInstructionPtr Pop();

	void TranslateIf(Parser::NodePtr node);
	void TranslateFor(Parser::NodePtr node);
	void TranslateWhile(Parser::NodePtr node);
};
