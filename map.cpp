#include "map.h"

#include "cmd.h"
#include "err.h"
#include "eval.h"
#include "invocation.h"
#include "token.h"

const Map *Map::as_map() const {
	return this;
}

Map *Map::as_map() {
	return this;
}

Node_Ptr Map::find(const std::string& name) const {
	auto found = values_.find(name);
	if (found != values_.end()) {
		return found->second;
	} else if (parent_) {
		const Map *m { parent_->as_map() };
		if (m) {
			return m->find(name);
		}
	}
	return Node_Ptr { };
}

void Map::push(const Node_Ptr& command, const std::string& name) {
	values_.insert({name, command});
}

static Invocation::Iter eat_space(Invocation::Iter it, Invocation::Iter end) {
	while (it != end && (**it).as_space()) { ++it; }
	return it;
}

class Map_Map: public Command {
public:
	Node_Ptr eval(Node_Ptr invocation, Map &state) const override;
};

Node_Ptr Map_Map::eval(Node_Ptr invocation, Map &state) const {
	Node_Ptr map { std::make_shared<Map>() };
	auto inv { *invocation->as_invocation() };
	auto it { inv.begin() };
	auto end { inv.end() };
	it = eat_space(it, end);
	if (it != end) {
		++it;
	} else { err("empty invocation"); }
	for (;;) {
		it = eat_space(it, end);
		if (it == end) { break; }
		Node_Ptr key { *it++ };
		const Token *key_token { key->as_token() };
		if (key_token && ! key_token->token().empty() && key_token->token().back() == ':') {
			it = eat_space(it, end);
			if (it != end) {
				Node_Ptr value { *it++ };
				value = ::eval(value, state);
				map->as_map()->push(value, key_token->token());
				continue;
			}
		}
		err("invalid map pair");
	}
	return map;
}

void add_map_commands(const Node_Ptr &state) {
	Map *m { state->as_map() };
	m->push(std::make_shared<Map_Map>(), "map");
}
