#include "io.h"
#include "map.h"
#include "parser.h"
#include "err.h"
#include <fstream>

Node_Ptr nil;
int main(int argc, const char *argv[]) {
	if (argc != 2) {
		fail("expect only one source file as argument");
	}
	const char *source_path { argv[1] };
	std::ifstream source { source_path };
	Node_Ptr state;
	nil = std::make_shared<Node>();
	state = std::make_shared<Map>(Node_Ptr { });
	Node_Ptr got;
	register_io_entries(state);
	for (;;) {
		source >> got;
		if (! got) { break; }
		got->eval(got, state);
	}
}
