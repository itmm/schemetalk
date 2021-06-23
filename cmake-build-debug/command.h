#line 92 "chap-3.md"
#pragma once
#include "node.h"

class Command: public Node {
public:
	virtual Node_Ptr invoke(
		Node_Ptr invocation, Node_Ptr state
	) = 0;
};
