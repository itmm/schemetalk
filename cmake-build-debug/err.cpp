#line 120 "/home/timm/prj/schemetalk/chap-1.md"
#include "err.h"
#include <iostream>
#include <cstdlib>

[[noreturn]] void fail(const std::string &msg) {
	std::cerr << "!SchemeTalk failed: " << msg << '\n';
	std::exit(EXIT_FAILURE);
}
