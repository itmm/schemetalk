#include "bool.h"

#include "cmd.h"
#include "err.h"
#include "eval.h"
#include "invocation.h"
#include "map.h"
#include "token.h"

class Bool_True: public Node {
public:
	[[nodiscard]] bool is_true() const override { return true; }
};

class Bool_False: public Node {
public:
	[[nodiscard]] bool is_false() const override { return true; }
};

Node_Ptr bool_true { std::make_shared<Bool_True>() };
Node_Ptr bool_false { std::make_shared<Bool_False>() };

static Invocation::Iter eat_space(Invocation::Iter it, Invocation::Iter end) {
	while (it != end && (**it).as_space()) { ++it; }
	return it;
}

static Invocation::Iter parse_arg_base(
	const std::string &key, bool &value, const Node_Ptr& state,
	Invocation::Iter it, Invocation::Iter end
) {
	it = eat_space(it, end);
	if (it != end && (**it).as_token() && (**it).as_token()->token() == key) {
		++it;
	} else { err("expected key '" + key + "'"); }
	it = eat_space(it, end);
	if (it == end) { err("expected bool after key '" + key + "'"); }
	Node_Ptr val { ::eval(*it, state) };
	if (val && val->is_bool()) {
		value = val->is_true();
		++it;
	} else { err("no value after key '" + key + "'"); }
	return it;
}


static void parse_one_arg_cmd(
	const Node_Ptr& invocation,
	const std::string &key1, bool &value1,
	const Node_Ptr& state
) {
	auto inv { *invocation->as_invocation() };
	auto it { inv.begin() };
	auto end { inv.end() };
	it = eat_space(it, end);
	if (it != end) {
		++it;
	} else { err("empty invocation"); }
	it = parse_arg_base(key1, value1, state, it, end);
	it = eat_space(it, end);
	if (it != end) { err("too many parameters"); }
}

static void parse_two_args_cmd(
	const Node_Ptr& invocation,
	const std::string &key1, bool &value1,
	const std::string &key2, bool &value2,
	const Node_Ptr& state
) {
	auto inv { *invocation->as_invocation() };
	auto it { inv.begin() };
	auto end { inv.end() };
	it = eat_space(it, end);
	if (it != end) {
		++it;
	} else { err("empty invocation"); }
	it = parse_arg_base(key1, value1, state, it, end);
	it = parse_arg_base(key2, value2, state, it, end);
	it = eat_space(it, end);
	if (it != end) { err("too many parameters"); }
}

class Bool_Not: public Command {
public:
	[[nodiscard]] Node_Ptr eval(Node_Ptr invocation, Node_Ptr state) const override;
};

Node_Ptr Bool_Not::eval(Node_Ptr invocation, Node_Ptr state) const {
	bool value;
	parse_one_arg_cmd(
		invocation, "value:", value, state
	);
	return value ? bool_false : bool_true;
}

class Bool_And: public Command {
public:
	[[nodiscard]] Node_Ptr eval(Node_Ptr invocation, Node_Ptr state) const override;
};

Node_Ptr Bool_And::eval(Node_Ptr invocation, Node_Ptr state) const {
	bool value, with;
	parse_two_args_cmd(
		invocation, "value:", value, "with:", with, state
	);
	return (value && with) ? bool_true : bool_false;
}

class Bool_Or: public Command {
public:
	[[nodiscard]] Node_Ptr eval(Node_Ptr invocation, Node_Ptr state) const override;
};

Node_Ptr Bool_Or::eval(Node_Ptr invocation, Node_Ptr state) const {
	bool value, with;
	parse_two_args_cmd(
		invocation, "value:", value, "with:", with, state
	);
	return (value || with) ? bool_true : bool_false;
}

class Bool_Assert: public Command {
public:
	[[nodiscard]] Node_Ptr eval(Node_Ptr invocation, Node_Ptr state) const override;
};

Node_Ptr Bool_Assert::eval(Node_Ptr invocation, Node_Ptr state) const {
	bool value;
	parse_one_arg_cmd(
		invocation, "test:", value, state
	);
	return std::make_shared<Token>(value ? "." : "FAILED");
}

void add_bool(const Node_Ptr &state) {
	Map *m { state->as_map() };
	m->push(bool_true, "true");
	m->push(bool_false, "false");
	m->push(std::make_shared<Bool_Not>(), "not");
	m->push(std::make_shared<Bool_And>(), "and");
	m->push(std::make_shared<Bool_Or>(), "or");
	m->push(std::make_shared<Bool_Assert>(), "assert");
}
