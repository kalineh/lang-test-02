#pragma once

#include "core.h"
#include "lexer.h"
#include "node.h"

#include <memory>

// iterate over a stream of tokens to produce an abstract syntax tree
struct Parser : Process
{
	typedef std::shared_ptr<Node> NodePtr;

	// what type of thing to parse
	enum Structure
	{
		ParseStatement, ParseExpression, ParseFunction, ParseProgram,
	};

	Parser(std::shared_ptr<Lexer> lexer, Structure st = ParseProgram);

	void Print();

	bool Passed() const { return passed;  }
	const std::string &GetError() const { return error; }

private:
	friend struct Translator;

	std::vector<Token> tokens;
	std::vector<NodePtr> stack;
	int current;
	NodePtr root;
	bool passed;
	std::string error;

private:
	void Run(Structure st);
	void Push(NodePtr node);
	NodePtr Pop();

	bool Program();
	bool Statement(NodePtr );
	bool Expression();

	bool Logical();
	bool Relational();
	bool Additive();
	bool Term();
	bool Factor();

	bool PushConsume();

	Token const &Next();
	Token const &Last();
	Token const &Current() const;
	Token const &Peek() const;
	bool PeekIs(Token::Type ty) const;
	Token const &Consume();
	int indent;

	bool Try(Token::Type type);

	bool TryAny2(Token::Type a, Token::Type b) { return Try(a) || Try(b); }
	bool TryAny3(Token::Type a, Token::Type b, Token::Type c) { return Try(a) || Try(b) || Try(c); }
	bool TryAny4(Token::Type a, Token::Type b, Token::Type c, Token::Type d) { return Try(a) || Try(b) || Try(c) || Try(d); }

	bool TryConcreteType()
	{
		return TryAny3(Token::TypeInt, Token::TypeFloat, Token::TypeString);
	}

	NodePtr Expect(Token::Type type);

	std::string Lead(int level);
	void Print(Node const &node, int level);
	void Block(NodePtr block);
	bool ParseFactorIdent();

	void ParseGetMember();

	void ParseMethodCall();
	void Function(NodePtr);

	void AddBlock(NodePtr fun);

	NodePtr NewNode(Node::Type t);
	NodePtr NewNode(Token const &);
	
	void IfCondition(NodePtr block);
	void ParseIndexOp();
	void Assignment(NodePtr);
	void For(NodePtr block);
	void While(NodePtr block);
	bool CreateError(const char *text);
};
