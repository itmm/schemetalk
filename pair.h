#pragma once

#include "node.h"

class Pair: public Node {
	Node_Ptr head_;
	Node_Ptr rest_;
public:
	explicit Pair(Node_Ptr head, Node_Ptr rest):
		head_ { head }, rest_ { rest }
	{ }
	[[nodiscard]] const Pair *as_pair() const override;
	[[nodiscard]] const Node_Ptr &head() const { return head_; }
	[[nodiscard]] const Node_Ptr &rest() const { return rest_; }
};

extern Node_Ptr pair_nil;

void add_pair_commands(const Node_Ptr &state);
