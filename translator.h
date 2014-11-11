#pragma once

#include "core.h"
#include "parser.h"

#include <string>
#include <strstream>
#include <iostream>
#include <vector>
#include <memory>

#include "operation.h"

struct VirtualInstruction
{
	virtual const char* DebugType() { return "VirtualInstruction"; }
	virtual std::string ToString() { return std::string("<nul>"); } 
};

typedef std::shared_ptr<VirtualInstruction> VirtualInstructionPtr;
typedef std::vector<VirtualInstructionPtr> VirtualInstructionList;

struct VirtualInstructionOperation
	: VirtualInstruction
{
	VirtualInstructionOperation() { } 
	VirtualInstructionOperation(const Operation& value) : value(value) { }
	VirtualInstructionOperation(Operation::Type value) : value(value) { }

	virtual const char* DebugType() { return "VirtualInstructionOperation"; }
	virtual std::string ToString() { return std::string(value.ToString()); }

	Operation value;
};

struct VirtualInstructionLiteralInteger
	: VirtualInstruction
{
	VirtualInstructionLiteralInteger() { } 
	VirtualInstructionLiteralInteger(int value) : value(value) { }

	virtual const char* DebugType() { return "VirtualInstructionLiteralInteger"; }
	virtual std::string ToString() { return slow_lexical_cast<std::string>(value); }

	int value;
};

struct VirtualInstructionLiteralFloat
	: VirtualInstruction
{
	VirtualInstructionLiteralFloat() { } 
	VirtualInstructionLiteralFloat(float value) : value(value) { }

	virtual const char* DebugType() { return "VirtualInstructionLiteralFloat"; }
	virtual std::string ToString() { return slow_lexical_cast<std::string>(value); }

	float value;
};

struct VirtualInstructionLiteralString
	: VirtualInstruction
{
	VirtualInstructionLiteralString() { } 
	VirtualInstructionLiteralString(const char* value) : value(value) { }
	VirtualInstructionLiteralString(const std::string& value) : value(value) { }

	virtual const char* DebugType() { return "VirtualInstructionLiteralString"; }
	virtual std::string ToString() { return value; }

	std::string value;
};

struct VirtualInstructionLiteralIdentifier
	: VirtualInstruction
{
	VirtualInstructionLiteralIdentifier() { } 
	VirtualInstructionLiteralIdentifier(const char* value) : value(value) { }
	VirtualInstructionLiteralIdentifier(const std::string& value) : value(value) { }

	virtual const char* DebugType() { return "VirtualInstructionLiteralIdentifier"; }
	virtual std::string ToString() { return value; }

	std::string value;
};

struct VirtualInstructionBlock
	: VirtualInstruction
{
	virtual const char* DebugType() { return "VirtualInstructionBlock"; }
	virtual std::string ToString() { return "block"; }

	VirtualInstructionList value;
};

struct VirtualInstructionFunction
	: VirtualInstruction
{
	virtual const char* DebugType() { return "VirtualInstructionFunction"; }
	virtual std::string ToString() { return "function"; }

	VirtualInstructionPtr identifier;
	// TODO: args
	// TODO: block
};

struct VirtualInstructionCall
	: VirtualInstruction
{
	virtual const char* DebugType() { return "VirtualInstructionCall"; }
	virtual std::string ToString() { return "call"; }

	VirtualInstructionPtr identifier;
	VirtualInstructionList args;
};

struct VirtualInstructionStore
	: VirtualInstruction
{
	virtual const char* DebugType() { return "VirtualInstructionStore"; }
	virtual std::string ToString() { return "store"; }

	VirtualInstructionPtr lhs;
	VirtualInstructionPtr rhs;
};

struct VirtualInstructionLoad
	: VirtualInstruction
{
	virtual const char* DebugType() { return "VirtualInstructionLoad"; }
	virtual std::string ToString() { return "load"; }
};

struct VirtualInstructionNew
	: VirtualInstruction
{
	virtual const char* DebugType() { return "VirtualInstructionNew"; }
	virtual std::string ToString() { return "new"; }

	VirtualInstruction identifier;
	VirtualInstruction type;
};

struct VirtualInstructionDelete
	: VirtualInstruction
{
	virtual const char* DebugType() { return "VirtualInstructionDelete"; }
	virtual std::string ToString() { return "delete"; }

	VirtualInstruction identifier;
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
