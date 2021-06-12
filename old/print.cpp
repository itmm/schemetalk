#include "print.h"

#include "err.h"
#include "invocation.h"
#include "map.h"
#include "num.h"
#include "pair.h"
#include "token.h"

static inline bool is_key(const Node &node) {
	auto token { node.as_token() };
	return token && token->token().back() == ':';
}

static void print_node(const Node &node, std::ostream &out);

static void print_invocation(
	const Invocation &invocation, std::ostream &out
) {
	out << "(";
	auto iter { invocation.begin() };
	while (iter != invocation.end() && (**iter).as_space()) {
		out << ' '; ++iter;
	}
	if (iter == invocation.end()) { err("empty invocation"); }
	print_node(**iter, out);
	bool was_key = false;
	while (++iter != invocation.end()) {
		if (was_key) {
			was_key = false;
			if ((**iter).as_space()) {
				out << ' ';
				continue;
			}
		}
		print_node(**iter, out);
		if (is_key(**iter)) { was_key = true; }
	}
	out << ")";
}

static void print_map(
	const Map &map, std::ostream &out
) {
	out << "(map";
	auto iter { map.begin() };
	for (;;) {
		if (iter == map.end()) { break; }
		out << ' ' << iter->first << ' ';
		print_node(*iter->second,  out);
		++iter;
	}
	out << ")";
}

static void print_pair(
	const Pair &pair, std::ostream &out
) {
	out << "(cons head: ";
	print_node(*pair.head(), out);
	if (pair.rest() && pair.rest() != pair_nil) {
		out << " rest: ";
		print_node(*pair.rest(), out);
	}
	out << ")";
}

static void print_node(const Node &node, std::ostream &out) {
	if (node.as_token()) {
		out << node.as_token()->token();
	} else if (node.as_number()) {
		out << node.as_number()->value();
	} else if (node.as_space()) {
		out << ' ';
	} else if (node.as_invocation()) {
		print_invocation(*node.as_invocation(), out);
	} else if (node.as_command()) {
		out << "##internal_command##";
	} else if (node.as_map()) {
		print_map(*node.as_map(), out);
	} else if (node.as_pair()) {
		print_pair(*node.as_pair(), out);
	} else if (node.is_true()) {
		out << "true";
	} else if (node.is_false()) {
		out << "false";
	} else if (&node == &*pair_nil) {
		out << "nil";
	} else { err("unknown node"); }
}

std::ostream& operator<<(std::ostream &out, const Node &node) {
	print_node(node, out);
	return out;
}

std::ostream& operator<<(std::ostream &out, const Node_Ptr &node) {
	if (node) {
		out << *node;
	} else {
		out << "#nil";
	}
	return out;
}
