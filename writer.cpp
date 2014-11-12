#include "core.h"
#include "writer.h"
#include "virtual_instruction.h"

//#include <boost/lexical_cast.hpp>
//#include <boost/range/adaptor/reversed.hpp>

void WriterText::Write(VirtualInstructionPtr node)
{
	return WriteNode(node);
}

void WriterText::WriteNode(VirtualInstructionPtr node, int level)
{
	std::string lead = std::move(std::string(level * 4, ' '));

	std::cout << lead;

	switch (node->GetTypeId())
	{
		case VirtualInstructionType::VirtualInstruction:
			std::cout << "<base>" << std::endl;
			break;

		case VirtualInstructionType::VirtualInstructionOperation:
			std::cout << "OP(" << node->ToStringValue() << ")" << std::endl;
			break;

		case VirtualInstructionType::VirtualInstructionLiteralInteger:
			std::cout << "INT(" << node->ToStringValue() << ")" << std::endl;
			break;

		case VirtualInstructionType::VirtualInstructionLiteralFloat:
			std::cout << "FLOAT(" << node->ToStringValue() << ")" << std::endl;
			break;

		case VirtualInstructionType::VirtualInstructionLiteralString:
			std::cout << "STRING(" << node->ToStringValue() << ")" << std::endl;
			break;

		case VirtualInstructionType::VirtualInstructionLiteralIdentifier:
			std::cout << "IDENTIFIER(" << node->ToStringValue() << ")" << std::endl;
			break;

		case VirtualInstructionType::VirtualInstructionBlock:
			std::cout << "BLOCK(" << node->ToStringValue() << ")" << std::endl;
			for (auto it : std::static_pointer_cast<VirtualInstructionBlock>(node)->value)
				WriteNode(it, level + 1);
			break;

		case VirtualInstructionType::VirtualInstructionFunction:
			std::cout << "FUNCTION(" << node->ToStringValue() << ")" << std::endl;
			break;

		case VirtualInstructionType::VirtualInstructionCall:
			break;
			std::cout << "CALL(" << node->ToStringValue() << ")" << std::endl;

		case VirtualInstructionType::VirtualInstructionStore:
			std::cout << "STORE(" << node->ToStringValue() << ")" << std::endl;
			break;

		case VirtualInstructionType::VirtualInstructionLoad:
			std::cout << "LOAD(" << node->ToStringValue() << ")" << std::endl;
			break;

		case VirtualInstructionType::VirtualInstructionNew:
			std::cout << "NEW(" << node->ToStringValue() << ")" << std::endl;
			break;

		case VirtualInstructionType::VirtualInstructionDelete:
			std::cout << "DELETE(" << node->ToStringValue() << ")" << std::endl;
			break;

		default:
			std::cout << "Unknown instruction: " << node->ToStringValue() << std::endl;	
			break;
	}
}
