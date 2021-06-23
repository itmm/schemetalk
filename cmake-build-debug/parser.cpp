#line 148 "chap-2.md"
#line 148 "chap-2.md"
#line 219 "chap-2.md"
#line 237 "chap-2.md"
#line 265 "chap-2.md"
#line 265 "chap-2.md"
#line 265 "chap-2.md"
#include "invocation.h"
#line 237
#include "token.h"
#line 219
#include "space.h"
#line 148
#include "parser.h"
#include "err.h"
#line 27 "chap-5.md"
#include "num.h"
#line 270 "chap-2.md"

#line 29 "chap-5.md"
bool is_number(const std::string token) {
	auto it { token.begin() };
	if (it != token.end() && (*it == '+' || *it == '-')) {
		++it;
	}
	int digits { 0 };
	while (it != token.end() && *it >= '0' && *it <= '9') {
		++it; ++digits;
	}
	if (it != token.end() && *it == '.') {
		++it;
		while (it != token.end() && *it >= '0' && *it <= '9') {
			++it; ++digits;
		}
	}
	return it == token.end() && digits;
}

double to_number(const std::string value) {
	return std::stod(value);
}
#line 271 "chap-2.md"
using int_type = std::istream::int_type;
using char_type = std::istream::char_type;

int_type read_node(
	std::istream &in, int_type ch, Node_Ptr &node
);

int_type read_invocation(
	std::istream &in, int_type ch, Node_Ptr &node
) {
	// read invocation
#line 268
	node.reset();
	ch = in.get();
	Node_Ptr arg;
	bool first_param { true };
	while (ch != ')') {
		// read invocation arg
		ch = read_node(in, ch, arg);
		if (ch == EOF && ! arg) {
			fail("incomplete invocation");
		}
		if (! node) {
			if (dynamic_cast<Space *>(arg.get())) {
				fail("space after (");
			}
			node = std::make_shared<Invocation>(arg);
		} else if (first_param && dynamic_cast<Space *>(
			arg.get()
		)) {
			first_param = false;
		} else {
			first_param = false;
			auto inv { dynamic_cast<Invocation *>(
				node.get()
			) };
			if (inv) {
				inv->push_back(arg);
			} else { fail("invalid node"); }
		}
#line 288
	}
	ch = in.get();
#line 253
	return ch;
}

int_type read_node(
	std::istream &in, int_type ch, Node_Ptr &node
) {
	// read node
#line 204
	if (ch <= ' ') {
		// read space
#line 222
		node = std::make_shared<Space>();
		while (ch != EOF && ch <= ' ') {
			ch = in.get();
		}
#line 171
	} else if (ch == '(') {
		ch = read_invocation(in, ch, node);
	} else if (ch == ')') {
		fail("unmatched ')'");
	} else {
		// read token
#line 240
		std::string value;
		while (ch != EOF && ch > ' ' &&
			ch != '(' && ch != ')'
		) {
			value += static_cast<char_type>(ch);
			ch = in.get();
		}
		do {
			// generate token subtypes
#line 52 "chap-5.md"
			if (is_number(value)) {
				node = std::make_shared<Number>(to_number(value));
				break;
			}
#line 340 "chap-2.md"
			node = std::make_shared<Token>(value);
		} while (false);
#line 253
	}
#line 169
	return ch;
}

std::istream &operator>>(std::istream &in, Node_Ptr &node) {
	int_type ch { in.get() };
	node.reset();
	if (ch != EOF) {
		ch = read_node(in, ch, node);
		if (ch != EOF) {
			in.putback(static_cast<char_type>(ch));
		}
	}
	return in;
}
