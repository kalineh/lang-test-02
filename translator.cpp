#include "core.h"
#include "translator.h"
#include "parser.h"

//#include <boost/lexical_cast.hpp>
//#include <boost/range/adaptor/reversed.hpp>

Translator::Translator(std::shared_ptr<Parser> p)
{
	if (p->Failed)
		return;

	PushNew();

	try
	{
		Translate(p->root);
	}
	catch (Exception &)
	{
		if (!Failed)
			Fail("Failed");
	}
}

void Translator::TranslateFromToken(Parser::NodePtr node)
{
	switch (node->token.type)
	{
	case Token::While:
		TranslateWhile(node);
		return;

	case Token::DivAssign:
		TranslateBinaryOp(node, Operation::DivEquals);
		return;

	case Token::MulAssign:
		TranslateBinaryOp(node, Operation::MulEquals);
		return;

	case Token::MinusAssign:
		TranslateBinaryOp(node, Operation::MinusEquals);
		return;

	case Token::PlusAssign:
		TranslateBinaryOp(node, Operation::PlusEquals);
		return;

	case Token::Assign:
		TranslateBinaryOp(node, Operation::Store);
		return;

	case Token::Lookup:
		AppendNewOp(Operation::Lookup);
		return;

	case Token::Self:
		AppendNewOp(Operation::This);
		return;

	case Token::NotEquiv:
		TranslateBinaryOp(node, Operation::NotEquiv);
		return;

	case Token::Equiv:
		TranslateBinaryOp(node, Operation::Equiv);
		return;

	case Token::Less:
		TranslateBinaryOp(node, Operation::Less);
		return;

	case Token::Greater:
		TranslateBinaryOp(node, Operation::Greater);
		return;

	case Token::GreaterEquiv:
		TranslateBinaryOp(node, Operation::GreaterOrEquiv);
		return;

	case Token::LessEquiv:
		TranslateBinaryOp(node, Operation::LessOrEquiv);
		return;

	case Token::Minus:
		TranslateBinaryOp(node, Operation::Minus);
		return;

	case Token::Plus:
		TranslateBinaryOp(node, Operation::Plus);
		return;

	case Token::Mul:
		TranslateBinaryOp(node, Operation::Multiply);
		return;

	case Token::Divide:
		TranslateBinaryOp(node, Operation::Divide);
		return;

	case Token::Or:
		TranslateBinaryOp(node, Operation::LogicalOr);
		return;

	case Token::And:
		TranslateBinaryOp(node, Operation::LogicalAnd);
		return;

	case Token::Int:
		// TODO: lexical cast
		//Append(reg.New<int>(boost::lexical_cast<int>(node->token.Text())));
		//AppendNew<VirtualInstructionLiteralInt>(node->token.Text());
		return;

	case Token::Float:
		// TODO: lexical cast
		//Append(reg.New<float>(boost::lexical_cast<float>(node->token.Text())));
		//AppendNew<VirtualInstructionLiteralFloat>(node->token.Text());
		return;

	case Token::String:
		AppendNew<VirtualInstructionLiteralString>(node->token.Text());
		return;

	case Token::Ident:
		AppendNew<VirtualInstructionLiteralIdentifier>(node->token.Text());
		return;

	case Token::Yield:
		//for (auto ch : node->Children)
		//	Translate(ch);
		//AppendNewOp(Operation::PushContext);
		//KAI_NOT_IMPLEMENTED();
		return;

	case Token::Return:
		for (auto ch : node->Children)
			Translate(ch);
		AppendNewOp(Operation::Return);
		return;
	}

	Fail("Unsupported node %s (token %s)", Node::ToString(node->type), Token::ToString(node->token.type));
	throw Unsupported();
}

void Translator::TranslateBinaryOp(Parser::NodePtr node, Operation::Type op)
{
	Translate(node->Children[0]);
	Translate(node->Children[1]);

	AppendNewOp(op);
}

void Translator::Translate(Parser::NodePtr node)
{
	if (!node)
	{
		Failed = true;
		return;
	}

	switch (node->type)
	{
	case Node::IndexOp:
		TranslateBinaryOp(node, Operation::Index);
		return;

	case Node::GetMember:
		TranslateBinaryOp(node, Operation::GetProperty);
		return;

	case Node::TokenType:
		TranslateFromToken(node);
		return;

	case Node::Assignment:
		// like a binary op, but argument order is reversed
		Translate(node->Children[1]);
		Translate(node->Children[0]);
		AppendNewOp(Operation::Store);
		return;

	case Node::Call:
		TranslateCall(node);
		return;

	case Node::Conditional:
		TranslateIf(node);
		return;

	case Node::Block:
		PushNew();
		for (auto st : node->Children)
			Translate(st);
		Append(Pop());
		return;

	case Node::List:
		// TODO: reverse
		//for (auto ch : boost::adaptors::reverse(node->Children))
			//Translate(ch);
		//AppendNew<int>(node->Children.size());
		//AppendNewOp(Operation::ToArray);
		return;

	case Node::For:
		TranslateFor(node);
		return;

	case Node::Function:
		TranslateFunction(node);
		return;

	case Node::Program:
		for (auto e : node->Children)
			Translate(e);
		return;
	}

	Fail("Unsupported node %s (token %s)", Node::ToString(node->type), Token::ToString(node->token.type));
	throw Unsupported();
}

void Translator::TranslateBlock(NodePtr node)
{
	for (auto st : node->Children)
		Translate(st);
}

void Translator::TranslateFunction(NodePtr node)
{
	// child 0: ident
	// child 1: args
	// child 2: block
	Node::ChildrenType const &ch = node->Children;

	// write the body
	PushNew();
	for (auto b : ch[2]->Children)
		Translate(b);

	// add the args
	for (auto a : ch[1]->Children)
		AppendNew<VirtualInstructionLiteralIdentifier>(a->token.Text());

	// write the name and store
	//AppendNew(Label(ch[0]->token.Text()));
	AppendNew<VirtualInstructionLiteralIdentifier>(ch[0]->token.Text());

	// TODO: probably dont need a store for us, since we arent 
	//       really translating a function as a value
	AppendNewOp(Operation::Store);
}

void Translator::TranslateCall(NodePtr node)
{
	Node::ChildrenType &children = node->Children;
	for (auto a : children[1]->Children)
		Translate(a);

	Translate(children[0]);
	if (children.size() > 2 && children[2]->token.type == Token::Replace)
		AppendNewOp(Operation::Replace);
	else
		AppendNewOp(Operation::SuspendNew);
}

VirtualInstructionPtr Translator::Top()
{
	// TODO: do we need a stack?
	//return stack.back();
	return instructions.back();
}

void Translator::PushNew()
{
	auto block = std::make_shared<VirtualInstructionBlock>();

	instructions.push_back(block);
}

void Translator::Append(VirtualInstructionPtr instruction)
{
	// TODO: should be stack push?
	instructions.push_back(instruction);
}

VirtualInstructionPtr Translator::Pop()
{
	// TODO: pop from stack?
	auto top = Top();
	instructions.pop_back();
	return top;
}

std::string Translator::Result() const
{
	// TODO: translate viptr to str
	//StringStream str;
	//for (auto ob : *stack.back()->GetCode())
		//str << ' ' << ob;
	//return str.ToString().c_str();

	return "NYI";
}

void Translator::AppendNewOp(Operation::Type op)
{
	AppendNew<VirtualInstructionOperation>(op);
}

void Translator::TranslateIf(Parser::NodePtr node)
{
	Node::ChildrenType const &ch = node->Children;
	bool hasElse = ch.size() > 2;
	Translate(ch[0]);
	if (hasElse)
		Translate(ch[2]);

	Translate(ch[1]);
	AppendNewOp(hasElse ? Operation::IfThenSuspendElseSuspend : Operation::IfThenSuspend);
}

void Translator::TranslateFor(Parser::NodePtr node)
{
	AppendNewOp(Operation::None);
}

void Translator::TranslateWhile(Parser::NodePtr node)
{
	AppendNewOp(Operation::None);
}

