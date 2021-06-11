#pragma once
#include "node.h"
#include <string>

class Token: public Node {
	std::string value_;
public:
	explicit Token(std::string value);
	const std::string &value() const;
};

inline Token::Token(std::string value):
	value_ { std::move(value) }
{ }

inline const std::string& Token::value() const {
	return value_;
}
