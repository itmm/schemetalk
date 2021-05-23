#include <iostream>
#include <sstream>
#include <err.h>

#include "arith.h"
#include "bool.h"
#include "map.h"
#include "eval.h"
#include "read.h"
#include "pair.h"
#include "print.h"
#include "dyn.h"
#include "pdf.h"

int main() {
	Node_Ptr state { std::make_shared<Map>() };
	add_arith(state);
	add_map_commands(state);
	add_dyn_commands(state);
	add_pair_commands(state);
	add_bool(state);

	{
		Pdf_Writer writer(std::cout);
		Node_Ptr node;
		bool was_space { true };
		std::string line;
		while (std::cin) {
			std::cin >> node;
			if (node->as_invocation()) {
				node = eval(node, state);
			}
			if (! node) { err("got null"); }
			if (node->as_space()) {
				was_space = true;
				continue;
			}
			if (was_space) {
				if (line.length() > 60) {
					writer.write_log(line);
					line = "";
				} else {
					line += " ";
				}
			}
			std::ostringstream out;
			out << node;
			line += out.str();
			was_space = false;
		}
		if (! line.empty()) { writer.write_log(line); }
	}
	return 0;
}
