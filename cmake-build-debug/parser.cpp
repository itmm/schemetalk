#line 148 "/home/timm/prj/schemetalk/chap-2.md"
#line 148 "/home/timm/prj/schemetalk/chap-2.md"
#line 208 "/home/timm/prj/schemetalk/chap-2.md"
#line 226 "/home/timm/prj/schemetalk/chap-2.md"
#line 251 "/home/timm/prj/schemetalk/chap-2.md"
#line 251 "/home/timm/prj/schemetalk/chap-2.md"
#include "invocation.h"
#line 226
#include "token.h"
#line 208
#include "space.h"
#line 148
#include "parser.h"
#include "err.h"

using int_type = std::istream::int_type;
using char_type = std::istream::char_type;

int_type read_node(
	std::istream &in, int_type ch, Node_Ptr &node
) {
	// read node
#line 193
	if (ch <= ' ') {
		// read space
#line 211
		node = std::make_shared<Space>();
		while (ch != EOF && ch <= ' ') {
			ch = in.get();
		}
#line 160
	} else if (ch == '(') {
		// read invocation
#line 254
		ch = in.get();
		Node_Ptr arg;
		bool first_param { true };
		while (ch != ')') {
			// read invocation arg
#line 273
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
#line 277
		}
		ch = in.get();
#line 246
	} else if (ch == ')') {
		fail("unmatched ')'");
	} else {
		// read token
#line 229
		std::string value;
		while (ch != EOF && ch > ' ' &&
			ch != '(' && ch != ')'
		) {
			value += static_cast<char_type>(ch);
			ch = in.get();
		}
		node = std::make_shared<Token>(value);
#line 231
	}
#line 158
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
