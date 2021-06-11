#pragma once
#include "map.h"
#include "node.h"
#include <string>

class Token: public Node {
	std::string value_;
public:
	Node_Ptr eval(Node_Ptr self, Node_Ptr state) override;
	explicit Token(std::string value);
	const std::string &value() const;
};

inline Token::Token(std::string value):
	value_ { std::move(value) }
{ }

inline const std::string& Token::value() const {
	return value_;
}
inline Node_Ptr Token::eval(Node_Ptr self, Node_Ptr state) {
	auto s { dynamic_cast<Map *>(state.get()) };
	if (! s) { fail("invalid state"); }
	Node_Ptr result { s->get(value_) };
	return result ? result : self;
}
