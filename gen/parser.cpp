#include "invocation.h"
#include "token.h"
#include "space.h"
#include "parser.h"
#include "err.h"

using int_type = std::istream::int_type;
using char_type = std::istream::char_type;

int_type read_node(
	std::istream &in, int_type ch, Node_Ptr &node
) {
	if (ch <= ' ') {
		// read space
		node = std::make_shared<Space>();
		while (ch != EOF && ch <= ' ') {
			ch = in.get();
		}
	} else if (ch == '(') {
		// read invocation
		ch = in.get();
		Node_Ptr arg;
		while (ch != ')') {
			ch = read_node(in, ch, arg);
			if (ch == EOF && ! arg) {
				fail("incomplete invocation");
			}
			if (! node) {
				node = std::make_shared<Invocation>(arg);
			} else {
				auto inv { dynamic_cast<Invocation *>(node.get()) };
				if (inv) {
					inv->push_back(arg);
				} else { fail("invalid node"); }
			}
		}
	} else if (ch == ')') {
		fail("unmatched ')'");
	} else {
		// read token
		std::string value;
		while (ch != EOF && ch > ' ' &&
			ch != '(' && ch != ')'
		) {
			value += static_cast<char_type>(ch);
			ch = in.get();
		}
		node = std::make_shared<Token>(value);
	}
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
