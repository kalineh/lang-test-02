#pragma once

#include "core.h"
#include "parser.h"

#include <string>
#include <strstream>
#include <iostream>
#include <vector>
#include <memory>

#include "operation.h"
#include "intermediate.h"

struct Translator : Process
{
	struct Exception { };
	struct Unsupported : Exception { };

	IntermediateBlockPtr root;
	IntermediateBlockList stack;

	explicit Translator(std::shared_ptr<Parser> p);

	std::string Result() const;

	// we dont want a stack
	// the root is a top-level block
	// everything else goes into that

	IntermediateBlockPtr Root();

private:
	typedef Parser::NodePtr NodePtr;

	void TranslateFunction(NodePtr node);
	void TranslateBlock(NodePtr node);
	void Translate(NodePtr node);
	void TranslateBinaryOp(NodePtr node, Operation::Type);

	// TODO: not create strings on each call: store the conversion in a map or something
	std::string ConvertOp(NodePtr node);

	void TranslateFromToken(NodePtr node);
	void TranslateCall(NodePtr node);
	void TranslateIndex(NodePtr node);

	void PushBlock();
	void PopBlock();

	IntermediateBlockPtr TopBlock();

	template <class T>
	std::shared_ptr<T> RetrieveByOffset(int offset)
	{
		auto block = TopBlock();
		auto result = (*block->value)[block->value->size() + offset - 1];

		return std::static_pointer_cast<T>(result);
	}

	void Append(IntermediatePtr instruction);

	template <class T>
	void AppendNew(const T& t)
	{
		Append(std::make_shared<T>(t));
	}

	void AppendNewOp(Operation::Type op)
	{
		// TODO: remove
	}

	void TranslateIf(Parser::NodePtr node);
	void TranslateFor(Parser::NodePtr node);
	void TranslateWhile(Parser::NodePtr node);
};
