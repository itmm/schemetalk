#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <map>

class Compound;
class Token;
class Map;
class Command;
class Map_Invocation;
class Raw_Invocation;
class Space;

class Node {
public:
	virtual ~Node() = default;
	virtual Space *as_space() { return nullptr; }
	virtual Compound *as_compound() { return nullptr; }
	virtual Token *as_token() { return nullptr; }
	virtual Map *as_map() { return nullptr; }
	virtual Command *as_command() { return nullptr; }
	virtual Map_Invocation *as_map_invocation() { return nullptr; }
	virtual Raw_Invocation *as_raw_invocation() { return nullptr; }
};

using Node_Ptr = std::shared_ptr<Node>;

class Space: public Node {
public:
	Space *as_space() override { return this; }
};

class Compound: public Node {
	using Container = std::vector<Node_Ptr>;
	using Iter = Container::const_iterator;
	Container children_;
public:
	Compound *as_compound() override { return this; }
	void push(const Node_Ptr& value) { children_.push_back(value); }
	[[nodiscard]] Iter begin() const { return children_.cbegin(); }
	[[nodiscard]] Iter end() const { return children_.cend(); }
};

class Map: public Node {
private:
	using Container = std::map<std::string, Node_Ptr>;
	using Iter = Container::const_iterator;
	Container children_;
public:
	Map *as_map() override { return this; }
	void push(const std::string& key, const Node_Ptr &value);
	[[nodiscard]] Iter begin() const { return children_.cbegin(); }
	[[nodiscard]] Iter end() const { return children_.cend(); }
};

void Map::push(const std::string& key, const Node_Ptr &value) {
	children_.emplace(key, value);
}

class Map_Invocation: public Map {
	Node_Ptr name_;
public:
	explicit Map_Invocation(Node_Ptr &&name): name_ { std::move(name) } { }
	Map_Invocation *as_map_invocation() override { return this; }

	[[nodiscard]] Node &name() { return *name_; }
};

class Raw_Invocation: public Compound {
	Node_Ptr name_;
public:
	explicit Raw_Invocation(Node_Ptr &&name): name_ { std::move(name) } { }
	Raw_Invocation *as_raw_invocation() override { return this; }

	[[nodiscard]] Node &name() { return *name_; }
};

class Token: public Node {
private:
	std::string token_;
public:
	explicit Token(std::string token): token_ { std::move(token) } { }
	Token *as_token() override { return this; }
	[[nodiscard]] std::string token() const { return token_; }
};

std::istream::int_type read_node(std::istream::int_type ch, Node_Ptr &node);

std::istream::int_type read_compound(std::istream::int_type ch, Compound &compound) {
	for (;;) {
		bool has_space { false };
		while (ch != EOF && ch <= ' ') { ch = std::cin.get(); has_space = true; }
		if (has_space) { compound.push(std::make_shared<Space>()); }
		if (ch == ']' || ch == EOF) {
			break;
		}
		Node_Ptr child;
		ch = read_node(ch, child);
		if (! child) {
			std::cerr << "expected child\n"; exit(EXIT_FAILURE);
		}
		compound.push(child);
	}
	return ch;
}

std::istream::int_type read_raw_invocation(std::istream::int_type ch, Node_Ptr &node) {
	Node_Ptr name;
	ch = std::cin.get();
	ch = read_node(ch, name);
	if (! name) {
		std::cerr << "no raw method name\n"; exit(EXIT_FAILURE);
	}
	auto invocation = std::make_shared<Raw_Invocation>(std::move(name));
	ch = read_compound(ch, *invocation);
	if (ch == ']') {
		ch = std::cin.get();
		node = std::move(invocation);
	} else {
		std::cerr << "no ']' in raw invocation\n";
		exit(EXIT_FAILURE);
	}
	return ch;
}

std::istream::int_type read_map(std::istream::int_type ch, Map &map) {
	for (;;) {
		while (ch != EOF && ch <= ' ') { ch = std::cin.get(); }
		if (ch == ')' || ch == EOF) {
			break;
		}
		Node_Ptr child;
		ch = read_node(ch, child);
		if (! child->as_token()) {
			std::cerr << "key must be token\n"; exit(EXIT_FAILURE);
		}
		std::string key { child->as_token()->token() };

		while (ch != EOF && ch <= ' ') { ch = std::cin.get(); }
		ch = read_node(ch, child);
		if (! child) {
			std::cerr << "expected value\n"; exit(EXIT_FAILURE);
		}
		map.push(key, child);
	}
	return ch;
}

std::istream::int_type read_map_invocation(std::istream::int_type ch, Node_Ptr &node) {
	Node_Ptr name;
	ch = std::cin.get();
	ch = read_node(ch, name);
	if (! name) {
		std::cerr << "no map method name\n"; exit(EXIT_FAILURE);
	}
	auto invocation = std::make_shared<Map_Invocation>(std::move(name));
	ch = read_map(ch, *invocation);
	if (ch == ')') {
		ch = std::cin.get();
		node = std::move(invocation);
	} else {
		std::cerr << "no ')' in map invocation\n";
		exit(EXIT_FAILURE);
	}
	return ch;
}

std::istream::int_type read_node(std::istream::int_type ch, Node_Ptr &node) {
	if (ch == '[') {
		ch = read_raw_invocation(ch, node);
	} else if (ch == '(') {
		ch = read_map_invocation(ch, node);
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
	for (int i { indent }; i; --i) { std::cout << '\t'; }
}

void print_node(Node &node, int indent);

void print_compound(const Compound &compound, int indent = 0) {
	print_indent(indent); std::cout << "[raw\n";
	for (const auto &child : compound) {
		print_indent(indent + 1);
		print_node(*child, indent + 1);
	}
	print_indent(indent); std::cout << "]\n";
}

void print_map_invocation(Map_Invocation &invocation, int indent);
void print_raw_invocation(Raw_Invocation &invocation, int indent);

void print_node(Node &node, int indent) {
	if (node.as_token()) {
		std::cout << node.as_token()->token() << '\n';
	} else if (node.as_map_invocation()) {
		print_map_invocation(*node.as_map_invocation(), indent);
	} else if (node.as_raw_invocation()) {
		print_raw_invocation(*node.as_raw_invocation(), indent);
	} else if (node.as_command()) {
		std::cout << "{_internal_command_}\n";
	} else {
		std::cout << "{_UNKNOWN_}\n";
	}
}

class Command: public Node {
public:
	Command *as_command() override { return this; }
	virtual Node_Ptr execute(Map &called_state, Map_Invocation &invocation) = 0;
};

class Add_Command: public Command {
public:
	Node_Ptr execute(Map &called_state, Map_Invocation &invocation) override {
		return std::make_shared<Token>("42");
	}
};

class Map_Command: public Command {
public:
	Node_Ptr execute(Map &called_state, Map_Invocation &invocation) override {
		Node_Ptr result = std::make_shared<Map>();
		auto map = *result->as_map();
		for (const auto &entry : invocation) {
			map.push(entry.first, entry.second);
		}
		return result;
	}
};

void print_map_invocation(Map_Invocation &invocation, int indent) {
	std::cout << "(";
	print_node(invocation.name(), indent + 1);
	for (const auto &entry : invocation) {
		print_indent(indent + 1);
		std::cout << entry.first << ' ';
		print_node(*entry.second, indent + 1);
	}
	print_indent(indent); std::cout << ")\n";
}

void print_map(Map &map, int indent) {
	std::cout << "(map\n";
	for (const auto &entry : map) {
		print_indent(indent + 1);
		std::cout << entry.first << ' ';
		print_node(*entry.second, indent + 1);
	}
	print_indent(indent); std::cout << ")\n";
}

void print_raw_invocation(Raw_Invocation &invocation, int indent) {
	std::cout << "[";
	print_node(invocation.name(), indent + 1);
	for (const auto &entry : invocation) {
		print_indent(indent + 1);
		print_node(*entry, indent + 1);
	}
	print_indent(indent); std::cout << "]\n";
}

int main() {
	Compound root;
	Map state;
	state.push("add", std::make_shared<Add_Command>());
	state.push("map", std::make_shared<Map_Command>());
	auto ch = read_compound(std::cin.get(), root);
	if (ch != EOF) { std::cerr << "EOF expected\n"; exit(EXIT_FAILURE); }
	print_compound(root);
	print_map(state, 0);
	return 0;
}
