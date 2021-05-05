#include "eval.h"

#include <cassert>

#include "cmd.h"
#include "err.h"
#include "invocation.h"
#include "map.h"
#include "node.h"
#include "token.h"

Node_Ptr eval_invocation(Node_Ptr node, Node_Ptr state) {
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
	Node_Ptr node_cmd { state->as_map()->find(key) };
	if (node_cmd) {
		const Command *real_cmd { node_cmd->as_command() };
		if (real_cmd) {
			return real_cmd->eval(node, state);
		}
	}
	return node;
}

Node_Ptr eval_token(Node_Ptr node, Node_Ptr state) {
	Node_Ptr got = state->as_map()->find(node->as_token()->token());
	return got ? got : node;
}

Node_Ptr eval(Node_Ptr node, Node_Ptr state) {
	if (node->as_invocation()) {
		return eval_invocation(node, state);

	} else if (node->as_token()) {
		return eval_token(node, state);
	} else {
		return node;
	}
}
