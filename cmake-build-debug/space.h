#pragma once
#include "node.h"

class Space: public Node {
public:
	void write(std::ostream &out) override;
};
inline void Space::write(std::ostream &out) {
	out << ' ';
}
