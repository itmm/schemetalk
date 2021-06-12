#include <iostream>

#include "err.h"

[[noreturn]] void err(const std::string &msg) {
	std::cerr << "(#ERROR# " << msg << ")\n";
	exit(EXIT_FAILURE);
}
