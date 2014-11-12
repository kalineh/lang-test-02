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
			for (auto it : std::static_pointer_cast<VirtualInstructionBlock>(node)->value)
				WriteNode(it, level + 1);
			break;

		case VirtualInstructionType::VirtualInstructionFunction:
			break;

		case VirtualInstructionType::VirtualInstructionCall:
			break;

		case VirtualInstructionType::VirtualInstructionStore:
			break;

		case VirtualInstructionType::VirtualInstructionLoad:
			break;

		case VirtualInstructionType::VirtualInstructionNew:
			break;

		case VirtualInstructionType::VirtualInstructionDelete:
			break;
	}
}
