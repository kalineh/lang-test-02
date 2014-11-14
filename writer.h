#pragma once

#include "core.h"
#include "parser.h"

#include <string>
#include <strstream>
#include <iostream>
#include <vector>
#include <memory>

#include "intermediate.h"

struct WriterText
{
	void Write(IntermediatePtr node);

private:
	void WriteNode(IntermediatePtr node, int level = 0);
};

struct WriterVM
{
};

struct WriterLLVM
{
};

