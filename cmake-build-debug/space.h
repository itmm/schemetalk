#line 43 "chap-2.md"
#line 43 "chap-2.md"
#pragma once
#include "node.h"

class Space: public Node {
public:
#line 37 "chap-4.md"
	void write(std::ostream &out) override;
#line 48 "chap-2.md"
};
#line 39 "chap-4.md"
inline void Space::write(std::ostream &out) {
	out << ' ';
}
