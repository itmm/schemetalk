#line 17 "chap-2.md"
#line 17 "chap-2.md"
#line 17 "chap-2.md"
#line 17 "chap-2.md"
#line 17 "chap-2.md"
#pragma once
#line 17 "chap-4.md"
#include <ostream>
#line 18 "chap-2.md"
#include <memory>

class Node;

using Node_Ptr = std::shared_ptr<Node>;

class Node {
public:
#line 324
#line 20 "chap-4.md"
	virtual void write(std::ostream &out) { }
#line 324 "chap-2.md"
	virtual Node_Ptr eval(Node_Ptr self, Node_Ptr state);
#line 26
	virtual ~Node() = default;
};
#line 326
inline Node_Ptr Node::eval(Node_Ptr self, Node_Ptr state) {
	return self;
}
#line 22 "chap-4.md"
#include "err.h"
inline std::ostream &operator<<(std::ostream &out, Node_Ptr node) {
	if (! node) {
		fail("invalid node");
	}
	node->write(out);
	return out;
}
#line 60 "chap-4.md"
extern Node_Ptr nil;
#line 131 "chap-4.md"
void register_node_entries(Node_Ptr state);
