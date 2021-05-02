#include "node.h"

const Space *Node::as_space() const {
	return nullptr;
}

const Token *Node::as_token() const {
	return nullptr;
}

const Invocation *Node::as_invocation() const {
	return nullptr;
}

const Number *Node::as_number() const {
	return nullptr;
}
