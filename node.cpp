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

const Command *Node::as_command() const {
	return nullptr;
}

const Map *Node::as_map() const {
	return nullptr;
}

Map *Node::as_map() {
	return nullptr;
}