#pragma once

#include "core.h"
#include "parser.h"

#include <string>
#include <strstream>
#include <vector>
#include <memory>

// virtual types

// operation
// literal

struct VirtualInstruction
{
};

typedef std::shared_ptr<VirtualInstruction> VirtualInstructionPtr;
typedef std::vector<VirtualInstructionPtr> VirtualInstructionList;

struct VirtualInstructionOperation
	: VirtualInstruction
{
	Operation value;
};

struct VirtualInstructionLiteralInteger
	: VirtualInstruction
{
	int value;
};

struct VirtualInstructionLiteralFloat
	: VirtualInstruction
{
	float value;
};

struct VirtualInstructionLiteralString
	: VirtualInstruction
{
	std::string value;
};

struct VirtualInstructionLiteralIdentifier
	: VirtualInstruction
{
	std::string value;
};

struct VirtualInstructionBlock
	: VirtualInstruction
{
	VirtualInstructionList value;
};

struct VirtualInstructionFunction
	: VirtualInstruction
{
	VirtualInstructionPtr identifier;
	
};

struct VirtualInstructionCall
	: VirtualInstruction
{
	VirtualInstructionPtr identifier;
	VirtualInstructionList args;
};

struct VirtualInstructionStore
	: VirtualInstruction
{
	VirtualInstructionPtr lhs;
	VirtualInstructionPtr rhs;
};

struct VirtualInstructionLoad
	: VirtualInstruction
{
};

struct VirtualInstructionNew
	: VirtualInstruction
{
	VirtualInstruction identifier;
	VirtualInstruction type;
};

struct VirtualInstructionDelete
	: VirtualInstruction
{
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

KAI_END