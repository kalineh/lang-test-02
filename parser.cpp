
#include "core.h"
#include "parser.h"

#include <iostream>
#include <algorithm>
#include <strstream>
#include <stdarg.h>

using namespace std;

Parser::Parser(std::shared_ptr<Lexer> lexer, Structure st)
{
	current = 0;
	indent = 0;

	if (lexer->Failed)
		return;

	// strip irrelevant tokens
	for (auto tok : lexer->tokens)
	{
		if (tok.type == Token::NewLine)
			continue;

		if (tok.type == Token::Whitespace)
			continue;

		if (tok.type == Token::Tab)
			continue;

		if (tok.type == Token::Comment)
			continue;

		tokens.push_back(tok);
	}

	root = NewNode(Node::Program);

	try
	{
		Run(st);
	}
	//catch (Exception::Base &e)
	//{
		//if (!Failed)
			//Fail(Lexer::CreateError(Current(), "%s", e.ToString()));
	//}
	catch (std::exception &f)
	{
		if (!Failed)
			Fail(Lexer::CreateErrorMessage(Current(), "%s", f.what()));
	}
	catch (...)
	{
		if (!Failed)
			Fail(Lexer::CreateErrorMessage(Current(), "internal error"));
	}
}

void Parser::Run(Structure st)
{
	switch (st)
	{
	case ParseStatement:
		if (!Statement(root))
		{
			CreateError("Statement expected");
			return;
		}
		break;

	case ParseExpression:
		if (!Expression())
		{
			CreateError("Expression expected");
			return;
		}
		root->Add(Pop());
		break;

	case ParseFunction:
		Function(root);
		break;

	case ParseProgram:
		Program();
		break;
	}

	if (!stack.empty())
		Fail("Internal error: Stack not empty after parsing");
}

bool Parser::Program()
{
	while (!Try(Token::None) && !Failed)
	{
		if (!Statement(root))
			break;
	}

	return true;
}

void Parser::Function(NodePtr node)
{
	Expect(Token::Func);
	Expect(Token::Ident);
	auto name = Last();
	auto fun = NewNode(Node::Function);
	fun->Add(name);
	Expect(Token::OpenParan);
	auto args = NewNode(Node::None);
	fun->Add(args);

	if (Try(Token::Ident))
	{
		args->Add(Consume());
		while (Try(Token::Comma))
		{
			Consume();
			args->Add(Expect(Token::Ident));
		}
	}

	Expect(Token::CloseParan);
	
	Block(fun);
	node->Add(fun);
}

Parser::NodePtr Parser::NewNode(Node::Type t)
{
	return std::make_shared<Node>(t);
}

Parser::NodePtr Parser::NewNode(Token const &t)
{
	return std::make_shared<Node>(t);
}

void Parser::Block(NodePtr node)
{
	if (!Try(Token::OpenBrace))
		return;

	Consume();

	auto block = NewNode(Node::Block);

	Push(block);

	while (Statement(block))
	{
		if (Try(Token::CloseBrace))
		{
			Consume();
			break;
		}
	}

	Pop();

	node->Add(block);
}

bool Parser::Statement(NodePtr block)
{
	switch (Current().type)
	{
		case Token::OpenBrace:
		{
			Block(block);
			return true;
		}

		case Token::Assert:
		{
			Consume();
			if (!Expression())
			{
				Fail(Lexer::CreateErrorMessage(Current(), "Assert needs an expression to test"));
				return false;
			}

			auto ass = NewNode(Consume());
			ass->Add(Pop());
			Push(ass);
			Expect(Token::Semi);
			goto finis;
		}

		case Token::Return:
		case Token::Yield:
		{
			auto ret = NewNode(Consume());
			if (Expression())
				ret->Add(Pop());
			block->Add(ret);
			Expect(Token::Semi);
			goto finis;
		}
		
		case Token::While:
		{
			While(block);
			return true;
		}

		case Token::For:
		{
			For(block);
			return true;
		}

		case Token::If:
		{
			IfCondition(block);
			return true;
		}
		
		case Token::Func:
		{
			Function(block);
			return true;
		}
	}

	if (!Expression())
		return false;

	// a block does not require a semicolon?
	//Expect(Token::Semi);

	block->Add(Pop());

finis:
	return true;
}

std::shared_ptr<Node> Parser::Expect(Token::Type type)
{
	Token tok = Current();
	if (tok.type != type)
	{
		Fail(Lexer::CreateErrorMessage(tok, "Expected %s, have %s", Token::ToString(type), Token::ToString(tok.type)));
		//THROW_1(LogicError, "Unexpected token");
		throw "Unexpected token";
	}

	Next();
	return std::make_shared<Node>(Last());
}

bool Parser::Try(Token::Type type)
{
	return Current().type == type;
}

Token const &Parser::Consume()
{
	return tokens[current++];
}

bool Parser::PeekIs(Token::Type ty) const
{
	return Peek().type == ty;
}

Token const & Parser::Peek() const
{
	return tokens[current + 1];
}

Token const & Parser::Current() const
{
	return tokens[current];
}

Token const &Parser::Last()
{
	return tokens[current - 1];
}

Token const &Parser::Next()
{
	return tokens[++current];
}

bool Parser::PushConsume()
{
	Push(NewNode(Consume()));
	return true;
}

bool Parser::Expression()
{
	bool paran = Try(Token::OpenParan);
	if (paran)
	{
		Consume();
		if (!Expression())
			return false;

		Expect(Token::CloseParan);
		return true;
	}

	if (!Logical())
		return false;

	// TODO: replace colon with Node::Type
	// decli, decle
	// TODO: add type specifier to node
	if (Try(Token::Colon))
	{
		auto node = NewNode(Consume());
		auto ident = Pop();

		// if assign, is auto
		// if typeid, is typed

		if (Try(Token::OpenParan))
		{
			Consume();

			auto list = NewNode(Node::List);

			while (true)
			{
				if (Try(Token::CloseParan))
				{
					break;
				}
				else if (TryConcreteType())
				{
					node->Add(NewNode(Consume()));

					if (Try(Token::Comma))
						Consume();
				}
				else
				{
					Fail(Lexer::CreateErrorMessage(Current(), "Unexpected token in args list"));
					return false;
				}
			}

			node->Add(list);
			node->Add(ident);

			Expect(Token::CloseParan);

			if (Try(Token::Colon))
			{
				Consume();

				if (!TryConcreteType())
				{
					Fail(Lexer::CreateErrorMessage(Current(), "Missing return type"));
					return false;
				}

				// TODO: type instead of token/ident
				auto ret = NewNode(Consume());
			}

			Expect(Token::Assign);

			// TODO: uninit
			//if (Try(Token::Uninitialized)) { ... }

			if (!Logical())
			{
				Fail(Lexer::CreateErrorMessage(Current(), "Declaration missing assignment"));
				return false;
			}

			node->Add(Pop());
			node->Add(ident);

			// TODO: sort out the type
			// TODO: figure out layout on translator side w/ Node types
			// TODO: fixed structure for Colon as a decl with stable children indices
			// TODO: allow null children for implicit autos
			//if (type)
			//{
				//node->Add(type);
			//}

			Push(node);

			return true;
		}

		NodePtr type;

		if (Try(Token::TypeAuto) || TryConcreteType())
		{
			type = NewNode(Consume());
		}

		if (Try(Token::Assign))
		{
			Consume();

			// TODO: uninit
			//if (Try(Token::Uninitialized)) { ... }

			if (!Logical())
			{
				Fail(Lexer::CreateErrorMessage(Current(), "Declaration missing assignment"));
				return false;
			}

			node->Add(Pop());
			node->Add(ident);

			if (type)
			{
				node->Add(type);
			}

			Push(node);

			return true;
		}

		Fail(Lexer::CreateErrorMessage(Current(), "Missing assignment after declaration")); 
		return false;
	}

	if (Try(Token::Assign) ||
		Try(Token::PlusAssign) ||
		Try(Token::MinusAssign) ||
		Try(Token::MulAssign) ||
		Try(Token::DivAssign))
	{
		auto node = NewNode(Consume());
		auto ident = Pop();

		if (!Logical())
		{
			Fail(Lexer::CreateErrorMessage(Current(), "Assignment requires an expression"));
			return false;
		}

		node->Add(Pop());
		node->Add(ident);
		Push(node);
	}

	return true;
}

bool Parser::Logical()
{
	if (!Relational())
		return false;

	while (Try(Token::And) || Try(Token::Or))
	{
		auto node = NewNode(Consume());
		node->Add(Pop());
		if (!Relational())
			return Fail(Lexer::CreateErrorMessage(Current(), "Relational expected"));

		node->Add(Pop());
		Push(node);
	}

	return true;
}

bool Parser::Relational()
{
	if (!Additive())
		return false;

	while (Try(Token::Less) || Try(Token::Greater) || Try(Token::Equiv) || Try(Token::NotEquiv)
		|| Try(Token::LessEquiv) || Try(Token::GreaterEquiv))
	{
		auto node = NewNode(Consume());
		node->Add(Pop());
		if (!Additive())
			return CreateError("Additive expected");

		node->Add(Pop());
		Push(node);
	}

	return true;
}

bool Parser::Additive()
{
	// unary +/- operator
	if (Try(Token::Plus) || Try(Token::Minus))
	{
		auto ty = Consume().type;
		Consume();
		if (!Term())
			return CreateError("Term expected");

		auto node = new Node(ty == Token::Plus ? Node::Positive : Node::Negative);
		node->Add(Pop());
		return true;
	}

	if (!Term())
		return false;

	while (Try(Token::Plus) || Try(Token::Minus))
	{
		auto node = NewNode(Consume());
		node->Add(Pop());
		if (!Term())
			return CreateError("Term expected");

		node->Add(Pop());
		Push(node);
	}

	return true;
}

bool Parser::Term()
{
	if (!Factor())
		return false;

	while (Try(Token::Mul) || Try(Token::Divide))
	{
		auto node = NewNode(Consume());
		node->Add(Pop());
		if (!Factor())
			return CreateError("Factor expected with a term");

		node->Add(Pop());
		Push(node);
	}

	return true;
}

bool Parser::Factor()
{
	if (Try(Token::OpenParan))
	{
		Consume();
		if (!Expression())
		{
			CreateError("Expected an expression for a factor in parenthesis");

			return false;
		}

		Expect(Token::CloseParan);
		return true;
	}

	if (Try(Token::OpenSquareBracket))
	{
		auto list = NewNode(Node::List);
		do
		{
			Consume();
			if (Expression())
				list->Add(Pop());
			else
				list->Add(0);
		}
		while (Try(Token::Comma));

		Expect(Token::CloseSquareBracket);
		Push(list);

		return true;
	}

	if (Try(Token::Int) || Try(Token::Float) || Try(Token::String) || Try(Token::True) || Try(Token::False))
		return PushConsume();

	if (Try(Token::Self))
		return PushConsume();

	while (Try(Token::Lookup))
		PushConsume();

	if (Try(Token::Ident))
		return ParseFactorIdent();

	return false;
}

Parser::NodePtr Parser::Pop()
{
	if (stack.empty())
	{
		CreateError("Internal Error: Parse stack empty");
		//KAI_THROW_0(EmptyStack);
		throw "EmptyStack";
	}

	auto last = stack.back();
	stack.pop_back();

	return last;
}

void Parser::Push(NodePtr node)
{
	if (node)
		stack.push_back(node);
}

void Parser::Print()
{
	Print(*root, 0);
}

void Parser::Print(Node const &node, int level)
{
	std::cout << Lead(level) << node << std::endl;

	for (auto ch : node.Children)
		Print(*ch, level + 1);
}

std::string Parser::Lead(int level)
{
	return std::move(std::string(level*4, ' '));
}

//warning C4127: conditional expression is constant
#pragma warning (disable:4127)

bool Parser::ParseFactorIdent()
{
	PushConsume();

	while (true)
	{
		if (Try(Token::Dot))
		{
			ParseGetMember();
			continue;
		}

		if (Try(Token::OpenParan))
		{
			ParseMethodCall();
			continue;
		}

		if (Try(Token::OpenSquareBracket))
		{
			ParseIndexOp();
			continue;
		}

		break;
	}

	return true;
}

void Parser::ParseMethodCall()
{
	Consume();
	auto call = NewNode(Node::Call);
	call->Add(Pop());
	auto args = NewNode(Node::ArgList);
	call->Add(args);

	if (Expression())
	{
		args->Add(Pop());
		while (Try(Token::Comma))
		{
			Consume();
			if (!Expression())
			{
				Fail("What is the next argument?");
				return;
			}

			args->Add(Pop());
		}
	}

	Push(call);
	Expect(Token::CloseParan);

	if (Try(Token::Replace))
		call->Add(Consume());
}

void Parser::ParseGetMember()
{
	Consume();
	auto get = NewNode(Node::GetMember);
	get->Add(Pop());
	get->Add(Expect(Token::Ident));
	Push(get);
}

void Parser::IfCondition(NodePtr block)
{
	if (!Try(Token::If))
		return;

	Consume();

	Expect(Token::OpenParan);

	// get the test expression
	if (!Expression())
	{
		CreateError("If what?");
		return;
	}

	NodePtr condition = Pop();

	Expect(Token::CloseParan);

	// get the true-clause
	NodePtr trueClause = NewNode(Node::Block);
	Block(trueClause);

	// make the conditional node in AST
	NodePtr cond = NewNode(Node::Conditional);
	cond->Add(condition);
	cond->Add(trueClause);

	// if there's an else, add it as well
	if (Try(Token::Else))
	{
		Consume();
		NodePtr falseClause = NewNode(Node::Block);
		Block(falseClause);
		cond->Add(falseClause);
	}

	block->Add(cond);
}

void Parser::ParseIndexOp()
{
	Consume();
	auto index = NewNode(Node::IndexOp);
	index->Add(Pop());
	if (!Expression())
	{
		CreateError("Index what?");
		return;
	}

	Expect(Token::CloseSquareBracket);
	index->Add(Pop());
	Push(index);
}

void Parser::For(NodePtr block)
{
	if (!Try(Token::For))
		return;

	Consume();

	auto f = NewNode(Node::For);
	if (!Expression())
	{
		CreateError("For what?");
		return;
	}

	if (Try(Token::In))
	{
		Consume();
		f->Add(Pop());

		if (!Expression())
		{
			CreateError("For each in what?");
			return;
		}

		f->Add(Pop());
	}
	else
	{
		Expect(Token::Semi);
		f->Add(Pop());

		if (!Expression())
		{
			CreateError("When does the for statement stop?");
			return;
		}

		f->Add(Pop());
		Expect(Token::Semi);

		if (!Expression())
		{
			CreateError("What happens when a for statement ends?");
			return;
		}

		f->Add(Pop());
	}

	Expect(Token::NewLine);
	AddBlock(f);
	block->Add(f);
}

void Parser::While(NodePtr block)
{
	auto w = NewNode(Consume());
	if (!Expression())
	{
		CreateError("While what?");
		return;
	}

	w->Add(Pop());
	block->Add(w);
}

bool Parser::CreateError(const char *text)
{
	return Fail(Lexer::CreateErrorMessage(Current(), text));
}

void Parser::AddBlock(NodePtr node)
{
	// TODO: this doesnt make sense with our brace parsing
	auto block = NewNode(Node::Block);
	Block(block);
	node->Add(block);
}
