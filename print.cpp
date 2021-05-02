#include "print.h"

#include "err.h"
#include "invocation.h"
#include "token.h"

static void print_indent(int indent, std::ostream &out) {
	out << "\n";
	for (int i { indent }; i; --i) { out << '\t'; }
}

static inline bool is_key(const Node &node) {
	auto token { node.as_token() };
	return token && token->token().back() == ':';
}

static void print_node(const Node &node, int indent, std::ostream &out);

static void print_invocation(
	const Invocation &invocation, int indent, std::ostream &out
) {
	out << "(";
	auto iter { invocation.begin() };
	while (iter != invocation.end() && (**iter).as_space()) {
		out << ' '; ++iter;
	}
	if (iter == invocation.end()) { err("empty invocation"); }
	print_node(**iter, indent, out);
	bool was_key = false;
	bool with_args = false;
	while (++iter != invocation.end()) {
		if (was_key) {
			was_key = false;
			if ((**iter).as_space()) {
				out << ' ';
				continue;
			}
		}
		print_node(**iter, indent + 1, out);
		with_args = true;
		if (is_key(**iter)) { was_key = true; }
	}
	if (with_args) { print_indent(indent, out); }
	out << ")";
}

static void print_node(const Node &node, int indent, std::ostream &out) {
	if (node.as_token()) {
		out << node.as_token()->token();
	} else if (node.as_space()) {
		print_indent(indent, out);
	} else if (node.as_invocation()) {
		print_invocation(*node.as_invocation(), indent, out);
	} else { err("unknown node"); }
}

std::ostream& operator<<(std::ostream &out, const Node &node) {
	print_node(node, 0, out);
	return out;
}
