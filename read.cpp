#include "read.h"

#include "err.h"
#include "invocation.h"
#include "space.h"
#include "token.h"

using ch_type = std::istream::int_type;

static ch_type read_node(Node_Ptr &node, ch_type ch, std::istream &in);

static ch_type read_invocation(
	Invocation &compound, ch_type ch, std::istream &in
) {
	bool contents = false;
	for (;;) {
		if (ch == ')') { ch = in.get(); break; }
		if (ch == EOF) { err("EOF while reading command"); }
		Node_Ptr child;
		ch = read_node(child, ch, in);
		if (! child->as_space()) { contents = true; }
		compound.push(child);
	}
	if (! contents) { err("empty invocation"); }
	return ch;
}

static ch_type read_space(Node_Ptr &node, ch_type ch, std::istream &in) {
	while (ch != EOF && ch <= ' ') { ch = in.get(); }
	node = std::make_shared<Space>();
	return ch;
}

static ch_type read_token(Node_Ptr &node, ch_type ch, std::istream &in) {
	std::string token;
	while (ch != EOF && ch > ' ' && ch != '(' && ch != ')') {
		token += static_cast<char>(ch);
		ch = in.get();
	}
	node = std::make_shared<Token>(token);
	return ch;
}

static ch_type read_node(Node_Ptr &node, ch_type ch, std::istream &in) {
	switch (ch) {
		case EOF: err("read after EOF");
		case ')': err("unmatched closing parenthesis");
		case '(': {
			ch = in.get();
			auto invocation { std::make_shared<Invocation>() };
			node = invocation;
			ch = read_invocation(*invocation, ch, in);
			break;
		}
		default:
			if (ch <= ' ') {
				ch = read_space(node, ch, in);
			} else {
				ch = read_token(node, ch, in);
			}
	}
	return ch;
}

std::istream &operator>>(std::istream &in, Node_Ptr &node) {
	auto ch { in.get() };
	if (ch != EOF) {
		ch = read_node(node, ch, in);
		if (ch != EOF) {
			in.putback(static_cast<std::istream::char_type>(ch));
		}
	}
	return in;
}
