#include "io.h"
#include "invocation.h"
#include "map.h"
#include "command.h"
#include <iostream>

class Write: public Command {
public:
	Node_Ptr invoke(Node_Ptr invocation, Node_Ptr state) override;
};

Node_Ptr Write::invoke(Node_Ptr invocation, Node_Ptr state) {
	auto inv { dynamic_cast<Invocation *>(invocation.get()) };
	if (! inv) { fail("no invocation"); }
	for (auto arg : *inv) {
		std::cout << arg->eval(arg, state);
	}
	return nil;
}

class Write_Newline: public Write {
public:
	Node_Ptr invoke(Node_Ptr invocation, Node_Ptr state) override;
};

Node_Ptr Write_Newline::invoke(Node_Ptr invocation, Node_Ptr state) {
	Write::invoke(invocation, state);
	std::cout << '\n';
	return nil;
}

void register_io_entries(Node_Ptr state) {
	auto s { dynamic_cast<Map *>(state.get()) };
	if (! s) { fail("no state"); }
	s->insert("write", std::make_shared<Write>());
	s->insert("write-line", std::make_shared<Write_Newline>());
}
