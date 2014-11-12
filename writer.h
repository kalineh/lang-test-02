#pragma once

#include "core.h"
#include "parser.h"

#include <string>
#include <strstream>
#include <iostream>
#include <vector>
#include <memory>

#include "virtual_instruction.h"

struct WriterText
{
	void Write(VirtualInstructionPtr node);

private:
	void WriteNode(VirtualInstructionPtr node, int level = 0);
};

struct WriterVM
{
};

struct WriterLLVM
{
};

