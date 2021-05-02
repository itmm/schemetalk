#pragma once

#include <vector>

#include "node.h"

class Invocation: public Node {
	using Container = std::vector<Node_Ptr>;
	Container children_;
public:
	using Iter = Container::const_iterator;
	[[nodiscard]] const Invocation *as_invocation() const override;
	void push(const Node_Ptr& value) { children_.push_back(value); }
	[[nodiscard]] Iter begin() const { return children_.cbegin(); }
	[[nodiscard]] Iter end() const { return children_.cend(); }
};

