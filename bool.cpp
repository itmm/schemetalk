#include "bool.h"

#include "map.h"

class Bool_True: public Node {
public:
	[[nodiscard]] bool is_true() const override { return true; }
};

class Bool_False: public Node {
public:
	[[nodiscard]] bool is_false() const override { return true; }
};

void add_bool(const Node_Ptr &state) {
	Map *m { state->as_map() };
	m->push(std::make_shared<Bool_True>(), "true");
	m->push(std::make_shared<Bool_False>(), "false");
}
