#include "core.h"
#include "translator.h"
#include "parser.h"

//#include <boost/lexical_cast.hpp>
//#include <boost/range/adaptor/reversed.hpp>

Translator::Translator(std::shared_ptr<Parser> p)
{
	PushBlock();

	root = TopBlock();

	if (p->Failed)
		return;

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

	case Token::Colon:
		TranslateAssignment(node);
		return;

	case Token::Assign:
		TranslateAssignment(node);
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
		//auto i = slow_lexical_cast<int>(node->token.Text());
		//auto p = std::make_shared<IntermediateLiteralInteger>(i);
		AppendNew<IntermediateLiteralInteger>(slow_lexical_cast<int>(node->token.Text()));
		return;

	case Token::Float:
		// TODO: lexical cast
		//Append(reg.New<float>(boost::lexical_cast<float>(node->token.Text())));
		//AppendNew<IntermediateLiteralFloat>(node->token.Text());
		return;

	case Token::String:
		AppendNew<IntermediateLiteralString>(node->token.Text());
		return;

	case Token::Ident:
		AppendNew<IntermediateLiteralIdentifier>(node->token.Text());
		return;

	case Token::TypeAuto:
	case Token::TypeInt:
	case Token::TypeFloat:
	case Token::TypeString:
		AppendNew<IntermediateLiteralIdentifier>(node->token.Text());
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

void Translator::TranslateAssignment(Parser::NodePtr node)
{
	PushBlock();

	Translate(node->Children[0]);
	Translate(node->Children[1]);
	Translate(node->Children[2]);

	auto type = RetrieveByOffset<Intermediate>(-0);
	auto lhs = RetrieveByOffset<IntermediateExpression>(-1);
	auto rhs = RetrieveByOffset<IntermediateExpression>(-2);

	PopBlock();
	
	auto assignment = std::make_shared<IntermediateAssignment>(type, lhs, rhs);

	Append(assignment);

}

void Translator::TranslateBinaryOp(Parser::NodePtr node, Operation::Type op)
{
	PushBlock();

	Translate(node->Children[0]);
	Translate(node->Children[1]);

	auto lhs = RetrieveByOffset<IntermediateExpression>(-0);
	auto rhs = RetrieveByOffset<IntermediateExpression>(-1);

	PopBlock();
	
	auto operation = std::make_shared<IntermediateBinaryOperation>(op, lhs, rhs);

	Append(operation);

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
		TranslateBinaryOp(node, Operation::Store);
		return;

	case Node::Call:
		TranslateCall(node);
		return;

	case Node::Conditional:
		TranslateIf(node);
		return;

	case Node::Block:
		PushBlock();
		for (auto st : node->Children)
			Translate(st);
		PopBlock();
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
	// TODO: dont we put a block on the list too?
	for (auto st : node->Children)
		Translate(st);
}

void Translator::TranslateFunction(NodePtr node)
{
	auto ident = node->Children[0];
	auto args = node->Children[1];
	auto block = node->Children[2];

	auto func = std::make_shared<IntermediateFunction>();

	func->ident = std::make_shared<IntermediateLiteralIdentifier>(ident->token.Text());

	for (auto a : args->Children)
	{
		auto c = std::make_shared<IntermediateLiteralIdentifier>(a->token.Text());
		func->args->push_back(c);
	}

	PushBlock();
	func->block = TopBlock();

	for (auto b : block->Children)
	{
		Translate(b);	
	}

	PopBlock();

	Append(func);
}

void Translator::TranslateCall(NodePtr node)
{
	auto children = node->Children;
	auto child_ident = children[0];
	auto child_args = children[1];

	PushBlock();

	for (auto arg : child_args->Children)
		Translate(arg);

	Translate(child_ident);

	auto ident = RetrieveByOffset<IntermediateLiteralIdentifier>(-0);
	auto args = std::make_shared<IntermediateList>();

	for (int i = 0; i < (int)child_args->Children.size(); ++i)
	{
		auto a = RetrieveByOffset<IntermediateExpression>(-1 + -i);
		args->push_back(a);
	}

	PopBlock();

	auto call = std::make_shared<IntermediateCall>(ident, args);
	
	Append(call);

	//Node::ChildrenType &children = node->Children;
	//for (auto a : children[1]->Children)
		//Translate(a);

	//Translate(children[0]);
	//if (children.size() > 2 && children[2]->token.type == Token::Replace)
		//AppendNewOp(Operation::Replace);
	//else
		//AppendNewOp(Operation::SuspendNew);
}

IntermediateBlockPtr Translator::Root()
{
	return root;
}

void Translator::PushBlock()
{
	stack.push_back(std::make_shared<IntermediateBlock>());
}

void Translator::PopBlock()
{
	stack.pop_back();
}

IntermediateBlockPtr Translator::TopBlock()
{
	return stack[stack.size() - 1];
}

void Translator::Append(IntermediatePtr instruction)
{
	TopBlock()->value->push_back(instruction);
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

