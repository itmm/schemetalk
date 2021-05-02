#include <iostream>

#include "read.h"
#include "print.h"

int main() {
	Node_Ptr root;
	while (std::cin) {
		std::cin >> root;
		std::cout << *root;
	}
	return 0;
}
