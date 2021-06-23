#line 137 "/home/timm/prj/schemetalk/chap-4.md"
#include "node.h"
#include "map.h"

void register_node_entries(Node_Ptr state) {
	auto s { dynamic_cast<Map *>(state.get()) };
	if (! s) { fail("no state"); }
	s->insert("nil", nil);
}
