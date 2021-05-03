#include <iostream>

#include "arith.h"
#include "map.h"
#include "eval.h"
#include "read.h"
#include "print.h"

int main() {
	Node_Ptr state { std::make_shared<Map>() };
	add_arith(state);
	add_map_commands(state);
	Node_Ptr root;
	while (std::cin) {
		std::cin >> root;
		root = eval(root, *state->as_map());
		std::cout << *root;
	}
	return 0;
}
