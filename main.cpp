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
	virtual Space *as_space() { return nullptr; }
	virtual Token *as_token() { return nullptr; }
	virtual Invocation *as_invocation() { return nullptr; }
};

using Node_Ptr = std::shared_ptr<Node>;

class Space: public Node {
public:
	Space *as_space() override { return this; }
};

class Token: public Node {
private:
	std::string token_;
public:
	explicit Token(std::string token): token_ { std::move(token) } { }
	Token *as_token() override { return this; }
	[[nodiscard]] std::string token() const { return token_; }
};

class Invocation: public Node {
	using Container = std::vector<Node_Ptr>;
	using Iter = Container::const_iterator;
	Container children_;
public:
	Invocation *as_invocation() override { return this; }
	void push(const Node_Ptr& value) { children_.push_back(value); }
	[[nodiscard]] Iter begin() const { return children_.cbegin(); }
	[[nodiscard]] Iter end() const { return children_.cend(); }
};

std::istream::int_type read_node(std::istream::int_type ch, Node_Ptr &node);

std::istream::int_type read_invocation(
	std::istream::int_type ch, Invocation &compound
) {
	for (;;) {
		if (ch == ')') { ch = std::cin.get(); break; }
		if (ch == EOF) {
			std::cerr << "(#ERROR# EOF while reading command)\n";
			exit(EXIT_FAILURE);
		}
		Node_Ptr child;
		ch = read_node(ch, child);
		compound.push(child);
	}
	return ch;
}

std::istream::int_type read_node(std::istream::int_type ch, Node_Ptr &node) {
	if (ch != EOF && ch <= ' ') {
		while (ch != EOF && ch <= ' ') { ch = std::cin.get(); }
		node = std::move(std::make_shared<Space>());
	} else if (ch == '(') {
		ch = std::cin.get();
		node = std::move(std::make_shared<Invocation>());
		ch = read_invocation(ch, *node->as_invocation());
	} else {
		std::string token;
		while (ch != EOF && ch > ' ' && ch != '(' && ch != ')') {
			token += static_cast<char>(ch);
			ch = std::cin.get();
		}
		node = std::move(std::make_shared<Token>(token));
	}
	return ch;
}

void print_indent(int indent) {
	std::cout << "\n";
	for (int i { indent }; i; --i) { std::cout << '\t'; }
}

void print_node(Node &node, int indent);

void print_invocation(const Invocation &invocation, int indent) {
	std::cout << "(";
	auto iter { invocation.begin() };
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
		if ((**iter).as_token() && (**iter).as_token()->token().back() == ':') {
			was_key = true;
		}
	}
	if (with_args) { print_indent(indent); }
	std::cout << ")";
}

void print_node(Node &node, int indent) {
	if (node.as_token()) {
		std::cout << node.as_token()->token();
	} else if (node.as_space()) {
		print_indent(indent);
	} else if (node.as_invocation()) {
		print_invocation(*node.as_invocation(), indent);
	} else {
		std::cerr << "(#ERROR# unknown node)\n";
	}
}

int main() {
	Node_Ptr root;
	int ch = std::cin.get();
	while (ch != EOF) {
		ch = read_node(ch, root);
		print_node(*root, 0);
	}
	return 0;
}
