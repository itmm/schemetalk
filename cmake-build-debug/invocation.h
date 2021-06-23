#line 89 "/home/timm/prj/schemetalk/chap-2.md"
#line 89 "/home/timm/prj/schemetalk/chap-2.md"
#pragma once
#include "node.h"
#include <vector>

class Invocation: public Node {
	Node_Ptr function_;
	using Container = std::vector<Node_Ptr>;
	Container arguments_;
public:
#line 108 "/home/timm/prj/schemetalk/chap-3.md"
	Node_Ptr eval(Node_Ptr self, Node_Ptr state) override;
#line 98 "/home/timm/prj/schemetalk/chap-2.md"
	explicit Invocation(Node_Ptr function);
	const Node_Ptr &function() const;
	void push_back(Node_Ptr argument);
	using Iter = Container::const_iterator;
	Iter begin() const { return arguments_.begin(); }
	Iter end() const { return arguments_.end(); }
};

inline Invocation::Invocation(Node_Ptr function):
	function_ { function }
{ }

inline const Node_Ptr &Invocation::function() const {
	return function_;
}

inline void Invocation::push_back(Node_Ptr argument) {
	arguments_.push_back(argument);
}
