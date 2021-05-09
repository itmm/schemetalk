#pragma once

#include <memory>

class Command;
class Token;
class Invocation;
class Space;
class Map;
class Number;
class Pair;

class Node {
public:
	virtual ~Node() = default;
	[[nodiscard]] virtual const Space *as_space() const;
	[[nodiscard]] virtual const Token *as_token() const;
	[[nodiscard]] virtual const Invocation *as_invocation() const;
	[[nodiscard]] virtual const Number *as_number() const;
	[[nodiscard]] virtual const Command *as_command() const;
	[[nodiscard]] virtual const Map *as_map() const;
	[[nodiscard]] virtual Map *as_map();
	[[nodiscard]] virtual const Pair *as_pair() const;
	[[nodiscard]] virtual bool is_true() const;
	[[nodiscard]] virtual bool is_false() const;
	[[nodiscard]] bool is_bool() const;
};

inline bool Node::is_bool() const {
	return is_true() || is_false();
}

using Node_Ptr = std::shared_ptr<Node>;
