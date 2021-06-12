#pragma once

#include "node.h"

class Command: public Node {
public:
	[[nodiscard]] const Command *as_command() const override;
	virtual Node_Ptr eval(Node_Ptr invocation, Node_Ptr state) const = 0;
};
