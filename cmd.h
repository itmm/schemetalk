#pragma once

#include "node.h"

class State;

class Command {
public:
	virtual Node_Ptr eval(Node_Ptr invocation, State &state) = 0;
};

using Command_Ptr = std::shared_ptr<Command>;
