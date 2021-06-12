#pragma once

#include "node.h"

class Number: public Node {
	double value_;
public:
	explicit Number(double value): value_ { value } { }
	[[nodiscard]] double value() const { return value_; }
	[[nodiscard]] const Number *as_number() const override;
};