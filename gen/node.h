#pragma once
#include <memory>

class Node;

using Node_Ptr = std::shared_ptr<Node>;

class Node {
public:
	virtual ~Node() = default;
	virtual Node_Ptr eval(Node_Ptr self, Node_Ptr state);
};

inline Node_Ptr Node::eval(Node_Ptr self, Node_Ptr state) {
	return self;
}
