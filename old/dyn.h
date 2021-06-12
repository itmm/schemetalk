#pragma once

#include "cmd.h"

class Dynamic: public Command {
	Node_Ptr parameters_;
	Node_Ptr body_;
	Node_Ptr state_;
public:
	Dynamic(Node_Ptr parameters, Node_Ptr body, Node_Ptr state):
		parameters_ { parameters }, body_ { body }, state_ { state }
	{ }

	Node_Ptr eval(Node_Ptr invocation, Node_Ptr state) const override;
};

void add_dyn_commands(const Node_Ptr &state);
