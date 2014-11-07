
#include "core.h"

struct Token
{
	enum Type
	{
		None,
		Whitespace,
		Semicolon,
		Int,
		Float,
		String,
		True,
		False,
		Return,
		Identifier,
		Dot,
		Comma,
		If,
		Else,
		For,
		While,
		OpenBrace,
		CloseBrace,
		OpenBracket,
		CloseBracket,
		Plus,
		Minus,
		Multiply,
		Divide,
		Modulus,
		PlusAssign,
		MinusAssign,
		MultiplyAssign,
		DivideAssign,
		ModulusAssign,
		Assign,
		Less,
		Greater,
		Equal,
		NotEqual,
		LessEqual,
		GreaterEqual,
		Not,
		And,
		Or,
		Xor,
		OpenSquareBracket,
		CloseSquareBracket,
		Increment,
		Decrement,
		Tab,
		Newline,
		Function,
		Comment,
		Assert,
		Preprocessor
	};
};

struct Lexer
{
	std::vector<Token> _tokens;
	std::vector<std::string> _lines;
	std::map<std::string, Token::Type> _keywords;

	// build keywords
	// pull lines
};
