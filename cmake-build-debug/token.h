#line 59 "chap-2.md"
#line 59 "chap-2.md"
#line 59 "chap-2.md"
#pragma once
#line 76 "chap-3.md"
#include "map.h"
#line 60 "chap-2.md"
#include "node.h"
#include <string>

class Token: public Node {
	std::string value_;
public:
#line 79 "chap-3.md"
#line 49 "chap-4.md"
	void write(std::ostream &out) override;
#line 79 "chap-3.md"
	Node_Ptr eval(Node_Ptr self, Node_Ptr state) override;
#line 66 "chap-2.md"
	explicit Token(std::string value);
	const std::string &value() const;
};

inline Token::Token(std::string value):
	value_ { std::move(value) }
{ }

inline const std::string& Token::value() const {
	return value_;
}
#line 81 "chap-3.md"
inline Node_Ptr Token::eval(Node_Ptr self, Node_Ptr state) {
	auto s { dynamic_cast<Map *>(state.get()) };
	if (! s) { fail("invalid state"); }
	Node_Ptr result { s->get(value_) };
	return result ? result : self;
}
#line 51 "chap-4.md"
inline void Token::write(std::ostream &out) {
	out << value_;
}
