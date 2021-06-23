#line 7 "chap-5.md"
#pragma once
#include "node.h"

class Number: public Node {
	double value_;
public:
	Number(double value): value_ { value } { }
	
	double value() const { return value_; }
	void write(std::ostream &out) override {
		out << value_;
	}
};
