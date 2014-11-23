#include "core.h"
#include "writer.h"
#include "intermediate.h"

//#include <boost/lexical_cast.hpp>
//#include <boost/range/adaptor/reversed.hpp>

void WriterText::Write(IntermediatePtr node)
{
	return WriteNode(node);
}

void WriterText::WriteNode(IntermediatePtr node, int level)
{
	std::string lead = std::move(std::string(level * 4, ' '));

	std::cout << lead;

	switch (node->GetTypeId())
	{
		case IntermediateType::Intermediate:
			std::cout << "<base>" << std::endl;
			break;

		case IntermediateType::IntermediateExpression:
			std::cout << "EXPR(" << node->ToStringValue() << ")" << std::endl;
			break;

		case IntermediateType::IntermediateAssignment:
			std::cout << "ASSIGN(" << node->ToStringValue() << ")" << std::endl;
			break;

		case IntermediateType::IntermediateBinaryOperation:
			std::cout << "OP(" << node->ToStringValue() << ")" << std::endl;
			break;

		case IntermediateType::IntermediateLiteralInteger:
			std::cout << "INT(" << node->ToStringValue() << ")" << std::endl;
			break;

		case IntermediateType::IntermediateLiteralFloat:
			std::cout << "FLOAT(" << node->ToStringValue() << ")" << std::endl;
			break;

		case IntermediateType::IntermediateLiteralString:
			std::cout << "STRING(" << node->ToStringValue() << ")" << std::endl;
			break;

		case IntermediateType::IntermediateLiteralIdentifier:
			std::cout << "IDENTIFIER(" << node->ToStringValue() << ")" << std::endl;
			break;

		case IntermediateType::IntermediateBlock:
			std::cout << "BLOCK(" << node->ToStringValue() << ")" << std::endl;
			for (auto it : *std::static_pointer_cast<IntermediateBlock>(node)->value)
				WriteNode(it, level + 1);
			break;

		case IntermediateType::IntermediateFunction:
			std::cout << "FUNCTION(" << node->ToStringValue() << ")" << std::endl;
			for (auto it : *std::static_pointer_cast<IntermediateFunction>(node)->block->value)
				WriteNode(it, level + 1);
			break;

		case IntermediateType::IntermediateCall:
			std::cout << "CALL(" << node->ToStringValue() << ")" << std::endl;
			break;

		case IntermediateType::IntermediateStore:
			std::cout << "STORE(" << node->ToStringValue() << ")" << std::endl;
			break;

		case IntermediateType::IntermediateLoad:
			std::cout << "LOAD(" << node->ToStringValue() << ")" << std::endl;
			break;

		case IntermediateType::IntermediateNew:
			std::cout << "NEW(" << node->ToStringValue() << ")" << std::endl;
			break;

		case IntermediateType::IntermediateDelete:
			std::cout << "DELETE(" << node->ToStringValue() << ")" << std::endl;
			break;

		default:
			std::cout << "Unknown instruction: " << node->ToStringValue() << std::endl;	
			break;
	}
}
