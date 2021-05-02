#include <iostream>

#include "eval.h"
#include "read.h"
#include "print.h"

int main() {
	Node_Ptr root;
	while (std::cin) {
		std::cin >> root;
		root = eval(root);
		std::cout << *root;
	}
	return 0;
}
