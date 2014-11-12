#pragma once

#include "core.h"
#include "parser.h"

#include <string>
#include <strstream>
#include <iostream>
#include <vector>
#include <memory>

#include "operation.h"
#include "virtual_instruction.h"

struct Translator : Process
{
	struct Exception { };
	struct Unsupported : Exception { };

	VirtualInstructionBlockList stack;

	explicit Translator(std::shared_ptr<Parser> p);

	std::string Result() const;

	const VirtualInstructionBlockList& Stack();

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

	void PushNew();

	void Append(VirtualInstructionPtr instruction);

	template <class T>
	void AppendNew(const T& t)
	{
		Append(std::make_shared<T>(t));
	}

	void AppendNewOp(Operation::Type op);

	VirtualInstructionBlockPtr Top();
	VirtualInstructionBlockPtr Pop();

	void TranslateIf(Parser::NodePtr node);
	void TranslateFor(Parser::NodePtr node);
	void TranslateWhile(Parser::NodePtr node);
};
