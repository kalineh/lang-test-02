#pragma once

#include "core.h"
#include "parser.h"

#include <string>
#include <strstream>
#include <vector>
#include <memory>

struct Translator : Process
{
	struct Exception { };
	struct Unsupported : Exception { };

	std::vector<Pointer<Continuation>> stack;
	Registry &reg;

	Translator(std::shared_ptr<Parser> p, Registry &reg);

	std::string Result() const;

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

	Pointer<Continuation> Top();
	void PushNew();
	void Append(Object ob);
	template <class T>
	void AppendNew(T const &val)
	{
		Append(reg.New<T>(val));
	}
	template <class T>
	void AppendNew()
	{
		Append(reg.New<T>());
	}
	Pointer<Continuation> Pop();
	void AppendNewOp(Operation::Type op);
	void TranslateIf(Parser::NodePtr node);
	void TranslateFor(Parser::NodePtr node);
	void TranslateWhile(Parser::NodePtr node);
};

KAI_END