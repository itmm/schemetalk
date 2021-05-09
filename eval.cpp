#include "eval.h"

#include <cassert>
#include <iostream>

#include "cmd.h"
#include "err.h"
#include "invocation.h"
#include "map.h"
#include "node.h"
#include "print.h"
#include "token.h"

Node_Ptr eval_invocation(const Node_Ptr& node, const Node_Ptr& state) {
	const auto invocation { node->as_invocation() };
	assert(invocation);
	auto it { invocation->begin() };
	while (it != invocation->end() && (**it).as_space()) { ++it; }
	if (it == invocation->end()) { err("no invocation"); }
	Node_Ptr cmd { ::eval(*it, state) };
	if (! cmd->as_command()) { err("no command"); }
	return cmd->as_command()->eval(node, state);
}

Node_Ptr eval_token(const Node_Ptr& node, const Node_Ptr& state) {
	bool fnd;
	Node_Ptr got = state->as_map()->find(node->as_token()->token(), fnd);
	if (! fnd) {
		err("unknown token '" + node->as_token()->token() + "'");
	}
	return got;
}

Node_Ptr eval(Node_Ptr node, const Node_Ptr& state) {
	if (node && node->as_invocation()) {
		return eval_invocation(node, state);
	} else if (node && node->as_token()) {
		return eval_token(node, state);
	} else {
		return node;
	}
}
