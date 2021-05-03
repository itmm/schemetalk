#pragma once

#include "node.h"

class Map;

class Command: public Node {
public:
	[[nodiscard]] const Command *as_command() const override;
	virtual Node_Ptr eval(Node_Ptr invocation, Map &state) const = 0;
};
