#pragma once

#include <string>

#include "node.h"

class Token: public Node {
private:
	std::string token_;
public:
	explicit Token(std::string token): token_ { std::move(token) } { }
	[[nodiscard]] const Token *as_token() const override;
	[[nodiscard]] std::string token() const { return token_; }
};

