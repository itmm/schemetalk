#include "node.h"

#include "invocation.h"
#include "token.h"

Node_Ptr eval_invocation(Node_Ptr node) {
	return node;
}

Node_Ptr eval(Node_Ptr node) {
	if (node->as_invocation()) {
		return eval_invocation(node);
	} else {
		return node;
	}
}
