
#pragma once

#include "core.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <strstream>

#include "token.h"
#include "slice.h"

struct Process
{
	bool Failed;
	std::string Error;

	Process() : Failed(false) { }

protected:
	bool Fail(const char *fmt, ...);
	bool Fail(const std::string &);
};

// Tokenise an input string for later parsing
struct Lexer : Process
{
	std::string input;

	Lexer(const char *input);

	void Print() const;

private:
	friend struct Token;
	friend struct Parser;

	int offset, lineNumber;

	typedef std::vector<Token> Tokens;
	typedef std::vector<std::string> Lines;
	typedef std::map<std::string, Token::Type> Keywords;

	Lines lines;
	Tokens tokens;
	Keywords keyWords;

	void CreateLines();
	void AddKeywords();
	bool Run();
	char Current() const;
	char Next();
	bool EndOfLine() const;
	char Peek() const;

	bool NextToken();

	void LexError(const char *msg);

	bool LexAlpha();
	const std::string &Line() const;

	void SearchForKeyword(Token &tok) const;

	bool Add(Token::Type type, Slice slice);
	bool Add(Token::Type type, int len = 1);

	// consume characters that pass the given filter function
	Slice Gather(int(*filt)(int ch));

	bool AddTwoCharOp(Token::Type ty);
	bool AddIfNext(char ch, Token::Type thenType, Token::Type elseType);
	
public:
	static std::string CreateErrorMessage(Token tok, const char *fmt, ...);
	bool LexString();
};
