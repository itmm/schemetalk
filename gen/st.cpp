#include "map.h"
#include "parser.h"
#include "err.h"
#include <fstream>

int main(int argc, const char *argv[]) {
	if (argc != 2) {
		fail("expect only one source file as argument");
	}
	const char *source_path { argv[1] };
	std::ifstream source { source_path };
	Node_Ptr state;
	state = std::make_shared<Map>(Node_Ptr { });
	Node_Ptr got;
	for (;;) {
		source >> got;
		if (! got) { break; }
		got->eval(got, state);
	}
}
