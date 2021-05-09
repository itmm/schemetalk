#include "pair.h"

#include "cmd.h"
#include "err.h"
#include "eval.h"
#include "invocation.h"
#include "map.h"
#include "token.h"

const Pair *Pair::as_pair() const {
	return this;
}

class Pair_Cons: public Command {
public:
	[[nodiscard]] Node_Ptr eval(
		Node_Ptr invocation, Node_Ptr state
	) const override;
};

static Invocation::Iter eat_space(Invocation::Iter it, Invocation::Iter end) {
	while (it != end && (**it).as_space()) { ++it; }
	return it;
}

static bool is_key(
	const std::string &key, Invocation::Iter it, Invocation::Iter end
) {
	return it != end && (**it).as_token() &&
		(**it).as_token()->token() == key;
}

Node_Ptr Pair_Cons::eval(Node_Ptr invocation, Node_Ptr state) const {
	Node_Ptr head;
	Node_Ptr rest;
	auto inv { *invocation->as_invocation() };
	auto it { inv.begin() };
	auto end { inv.end() };
	it = eat_space(it, end);
	++it;
	it = eat_space(it, end);
	if (is_key("head:", it, end )) {
		++it;
		it = eat_space(it, end);
		if (it != end) {
			head = ::eval(*it++, state);
		} else { err("cons: no head value"); }
	} else { err("cons: no head entry"); }
	it = eat_space(it, end);
	if (is_key("rest:", it, end)) {
		++it;
		it = eat_space(it, end);
		if (it != end) {
			rest = ::eval(*it++, state);
		} else { err("cons: no rest value"); }
	}
	it = eat_space(it, end);
	if (it != end) { err("cons: too many arguments"); }
	return std::make_shared<Pair>(head, rest);
}

class Pair_Head: public Command {
public:
	[[nodiscard]] Node_Ptr eval(
		Node_Ptr invocation, Node_Ptr state
	) const override;
};

Node_Ptr Pair_Head::eval(Node_Ptr invocation, Node_Ptr state) const {
	Node_Ptr list;
	auto inv { *invocation->as_invocation() };
	auto it { inv.begin() };
	auto end { inv.end() };
	it = eat_space(it, end);
	++it;
	it = eat_space(it, end);
	if (is_key("list:", it, end )) {
		++it;
		it = eat_space(it, end);
		if (it != end) {
			list = ::eval(*it++, state);
		} else { err("head: no list: value"); }
	} else { err("head: no list: entry"); }
	it = eat_space(it, end);
	if (it != end) { err("head: too many arguments"); }
	if (list->as_pair()) {
		return list->as_pair()->head();
	} else { err("head: no list"); }
}

class Pair_Rest: public Command {
public:
	[[nodiscard]] Node_Ptr eval(
		Node_Ptr invocation, Node_Ptr state
	) const override;
};

Node_Ptr Pair_Rest::eval(Node_Ptr invocation, Node_Ptr state) const {
	Node_Ptr list;
	auto inv { *invocation->as_invocation() };
	auto it { inv.begin() };
	auto end { inv.end() };
	it = eat_space(it, end);
	++it;
	it = eat_space(it, end);
	if (is_key("list:", it, end )) {
		++it;
		it = eat_space(it, end);
		if (it != end) {
			list = ::eval(*it++, state);
		} else { err("rest: no list: value"); }
	} else { err("rest: no list: entry"); }
	it = eat_space(it, end);
	if (it != end) { err("rest: too many arguments"); }
	if (list->as_pair()) {
		return list->as_pair()->rest();
	} else { err("rest: no list"); }
}

void add_pair_commands(const Node_Ptr &state) {
	Map *m { state->as_map() };
	m->push(std::make_shared<Pair_Cons>(), "cons");
	m->push(std::make_shared<Pair_Head>(), "head");
	m->push(std::make_shared<Pair_Rest>(), "rest");
}
