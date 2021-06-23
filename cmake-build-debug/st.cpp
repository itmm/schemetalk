#line 59 "/home/timm/prj/schemetalk/chap-1.md"
#line 94 "/home/timm/prj/schemetalk/chap-1.md"
#line 326 "/home/timm/prj/schemetalk/chap-2.md"
#line 65 "/home/timm/prj/schemetalk/chap-3.md"
#line 65 "/home/timm/prj/schemetalk/chap-3.md"
#line 120 "/home/timm/prj/schemetalk/chap-4.md"
#include "io.h"
#line 65 "/home/timm/prj/schemetalk/chap-3.md"
#include "map.h"
#line 326 "/home/timm/prj/schemetalk/chap-2.md"
#include "parser.h"
#line 94 "/home/timm/prj/schemetalk/chap-1.md"
#include "err.h"
#include <fstream>

#line 59
#line 68 "/home/timm/prj/schemetalk/chap-4.md"
Node_Ptr nil;
#line 59 "/home/timm/prj/schemetalk/chap-3.md"
int main(int argc, const char *argv[]) {
#line 98
	if (argc != 2) {
		fail("expect only one source file as argument");
	}
	const char *source_path { argv[1] };
	std::ifstream source { source_path };
#line 60
#line 329 "/home/timm/prj/schemetalk/chap-2.md"
	Node_Ptr state;
#line 68 "/home/timm/prj/schemetalk/chap-3.md"
#line 71 "/home/timm/prj/schemetalk/chap-4.md"
	nil = std::make_shared<Node>();
#line 68 "/home/timm/prj/schemetalk/chap-3.md"
	state = std::make_shared<Map>(Node_Ptr { });
#line 337 "/home/timm/prj/schemetalk/chap-2.md"
	Node_Ptr got;
#line 123 "/home/timm/prj/schemetalk/chap-4.md"
	register_io_entries(state);
#line 81 "/home/timm/prj/schemetalk/chap-3.md"
	for (;;) {
		source >> got;
		if (! got) { break; }
		got->eval(got, state);
	}
#line 60 "/home/timm/prj/schemetalk/chap-1.md"
}
