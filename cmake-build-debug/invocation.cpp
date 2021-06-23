#line 115 "chap-3.md"
#include "invocation.h"
#include "map.h"
#include "command.h"

Node_Ptr Invocation::eval(Node_Ptr self, Node_Ptr state) {
	auto s { dynamic_cast<Map *>(state.get()) };
	if (! s) {
		fail("wrong state type");
	}
	auto fn = dynamic_cast<Command *>(
		function_->eval(function_, state).get()
	);
	if (! fn) {
		fail("no function in invocation");
	}
	return fn->invoke(self, state);
}
