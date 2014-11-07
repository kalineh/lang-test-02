
#include "core.h"

#include <memory>
#include <vector>

struct ASTNode;
struct ASTBlock;
struct ASTStatement;
struct ASTDeclarationVariable;
struct ASTDeclarationFunction;
struct ASTExpression;
struct ASTExpressionAssignment;
struct ASTExpressionIdentifier;
struct ASTExpressionLiteralInteger;
struct ASTExpressionLiteralString;

//template <class T> struct ASTTypeInfo;
//template <> struct ASTTypeInfo<ASTNode> { static const char* typename = "ASTNode"; };
//template <> struct ASTTypeInfo<ASTBlock> { static const char* typename = "ASTBlock"; };
//template <> struct ASTTypeInfo<ASTStatement> { static const char* typename = "ASTStatement"; };

typedef std::shared_ptr<ASTNode> ASTNodePtr;
typedef std::shared_ptr<ASTStatement> ASTStatementPtr;

typedef std::vector<ASTNodePtr> ASTNodeList;
typedef std::vector<ASTStatementPtr> ASTStatementList;

struct ASTNode
{
};

struct ASTBlock : ASTNode
{
	ASTStatementList statements;
};

struct ASTStatement : ASTNode
{
};

struct ASTDeclarationVariable : ASTStatement
{
	// Identifier name
	// Identifier type
	// [Expression value]
};

struct ASTDeclarationFunction : ASTStatement
{
	// Identifier name
	// Identifier return_type
	// VariableDeclaration* parameters

};

struct ASTExpression : ASTNode
{
};

struct ASTExpressionAssignment : ASTExpression
{
	// Identifier lhs
	// Expression rhs
};

struct ASTExpressionIdentifier : ASTExpression
{
	// string name
};

struct ASTExpressionLiteralInteger : ASTExpression
{
	// int value
};

struct ASTExpressionLiteralString : ASTExpression
{
	// string value
};
