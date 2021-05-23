#include <iostream>

#include "arith.h"
#include "bool.h"
#include "map.h"
#include "eval.h"
#include "read.h"
#include "pair.h"
#include "print.h"
#include "dyn.h"

int main() {
	Node_Ptr state { std::make_shared<Map>() };
	add_arith(state);
	add_map_commands(state);
	add_dyn_commands(state);
	add_pair_commands(state);
	add_bool(state);

	Node_Ptr root;
	bool was_space { false };
	while (std::cin) {
		std::cin >> root;
		root = eval(root, state);
		if (! root) { break; }
		if (root->as_space()) {
			if (was_space) { continue; }
			was_space = true;
		} else {
			was_space = false;
		}
		std::cout << root;
	}
	return 0;
}
