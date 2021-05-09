#include <iostream>

#include "arith.h"
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

	Node_Ptr root;
	while (std::cin) {
		std::cin >> root;
		root = eval(root, state);
		std::cout << *root;
	}
	return 0;
}
