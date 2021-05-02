#pragma once

#include <memory>

class Token;
class Invocation;
class Space;
class Number;

class Node {
public:
	virtual ~Node() = default;
	[[nodiscard]] virtual const Space *as_space() const;
	[[nodiscard]] virtual const Token *as_token() const;
	[[nodiscard]] virtual const Invocation *as_invocation() const;
	[[nodiscard]] virtual const Number *as_number() const;
};

using Node_Ptr = std::shared_ptr<Node>;

