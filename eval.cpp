#include "eval.h"

#include <cassert>

#include "err.h"
#include "invocation.h"
#include "node.h"
#include "token.h"

Node_Ptr eval_invocation(Node_Ptr node, State_Ptr state) {
	const auto invocation { node->as_invocation() };
	assert(invocation);
	auto it { invocation->begin() };
	while (it != invocation->end() && (**it).as_space()) { ++it; }
	if (it == invocation->end()) { err("no invocation"); }
	Node_Ptr cmd { *it };
	if (cmd->as_invocation()) {
		cmd = eval_invocation(cmd, state);
	}
	if (! cmd->as_token()) { err("no command name"); }
	auto key { cmd->as_token()->token() };
	Command_Ptr real_cmd { state->find(key) };
	if (real_cmd) {
		return real_cmd->eval(node, *state);
	}
	return node;
}

Node_Ptr eval(Node_Ptr node, State_Ptr state) {
	if (node->as_invocation()) {
		return eval_invocation(node, state);
	} else {
		return node;
	}
}
