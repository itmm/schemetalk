#pragma once
#include <ostream>
#include <memory>

class Node;

using Node_Ptr = std::shared_ptr<Node>;

class Node {
public:
	virtual void write(std::ostream &out) { }
	virtual Node_Ptr eval(Node_Ptr self, Node_Ptr state);
	virtual ~Node() = default;
};
inline Node_Ptr Node::eval(Node_Ptr self, Node_Ptr state) {
	return self;
}
#include "err.h"
inline std::ostream &operator<<(std::ostream &out, Node_Ptr node) {
	if (! node) {
		fail("invalid node");
	}
	node->write(out);
	return out;
}
extern Node_Ptr nil;
void register_node_entries(Node_Ptr state);
