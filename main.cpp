#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <map>

class Token;
class Invocation;
class Space;

class Node {
public:
	virtual ~Node() = default;
	[[nodiscard]] virtual const Space *as_space() const {
		return nullptr;
	}
	[[nodiscard]] virtual const Token *as_token() const {
		return nullptr;
	}
	[[nodiscard]] virtual const Invocation *as_invocation() const {
		return nullptr;
	}
};

using Node_Ptr = std::shared_ptr<Node>;

class Space: public Node {
public:
	[[nodiscard]] const Space *as_space() const override { return this; }
};

class Token: public Node {
private:
	std::string token_;
public:
	explicit Token(std::string token): token_ { std::move(token) } { }
	[[nodiscard]] const Token *as_token() const override { return this; }
	[[nodiscard]] std::string token() const { return token_; }
};

class Invocation: public Node {
	using Container = std::vector<Node_Ptr>;
	using Iter = Container::const_iterator;
	Container children_;
public:
	[[nodiscard]] const Invocation *as_invocation() const override {
		return this;
	}
	void push(const Node_Ptr& value) { children_.push_back(value); }
	[[nodiscard]] Iter begin() const { return children_.cbegin(); }
	[[nodiscard]] Iter end() const { return children_.cend(); }
};

using ch_type = std::istream::int_type;

ch_type read_node(ch_type ch, Node_Ptr &node);

[[noreturn]] void err(const std::string &msg) {
	std::cerr << "(#ERROR# " << msg << ")\n";
	exit(EXIT_FAILURE);
}

ch_type read_invocation(ch_type ch, Invocation &compound) {
	bool contents = false;
	for (;;) {
		if (ch == ')') { ch = std::cin.get(); break; }
		if (ch == EOF) { err("EOF while reading command"); }
		Node_Ptr child;
		ch = read_node(ch, child);
		if (! child->as_space()) { contents = true; }
		compound.push(child);
	}
	if (! contents) { err("empty invocation"); }
	return ch;
}

ch_type read_space(ch_type ch, Node_Ptr &node) {
	while (ch != EOF && ch <= ' ') { ch = std::cin.get(); }
	node = std::make_shared<Space>();
	return ch;
}

ch_type read_token(ch_type ch, Node_Ptr &node) {
	std::string token;
	while (ch != EOF && ch > ' ' && ch != '(' && ch != ')') {
		token += static_cast<char>(ch);
		ch = std::cin.get();
	}
	node = std::make_shared<Token>(token);
	return ch;
}

ch_type read_node(ch_type ch, Node_Ptr &node) {
	switch (ch) {
	case EOF: err("read after EOF");
	case ')': err("unmatched closing parenthesis");
	case '(': {
		ch = std::cin.get();
		auto invocation { std::make_shared<Invocation>() };
		node = invocation;
		ch = read_invocation(ch, *invocation);
		break;
	}
	default:
		if (ch <= ' ') {
			ch = read_space(ch, node);
		} else {
			ch = read_token(ch, node);
		}
	}
	return ch;
}

void print_indent(int indent) {
	std::cout << "\n";
	for (int i { indent }; i; --i) { std::cout << '\t'; }
}

void print_node(const Node &node, int indent);

inline bool is_key(const Node &node) {
	auto token { node.as_token() };
	return token && token->token().back() == ':';
}

void print_invocation(const Invocation &invocation, int indent) {
	std::cout << "(";
	auto iter { invocation.begin() };
	while (iter != invocation.end() && (**iter).as_space()) {
		std::cout << ' '; ++iter;
	}
	if (iter == invocation.end()) { err("empty invocation"); }
	print_node(**iter, indent);
	bool was_key = false;
	bool with_args = false;
	while (++iter != invocation.end()) {
		if (was_key) {
			was_key = false;
			if ((**iter).as_space()) {
				std::cout << ' ';
				continue;
			}
		}
		print_node(**iter, indent + 1);
		with_args = true;
		if (is_key(**iter)) { was_key = true; }
	}
	if (with_args) { print_indent(indent); }
	std::cout << ")";
}

void print_node(const Node &node, int indent) {
	if (node.as_token()) {
		std::cout << node.as_token()->token();
	} else if (node.as_space()) {
		print_indent(indent);
	} else if (node.as_invocation()) {
		print_invocation(*node.as_invocation(), indent);
	} else { err("unknown node"); }
}

int main() {
	Node_Ptr root;
	ch_type ch { std::cin.get() };
	while (ch != EOF) {
		ch = read_node(ch, root);
		print_node(*root, 0);
	}
	return 0;
}
