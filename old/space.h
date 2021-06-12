#pragma once

#include "node.h"

class Space: public Node {
public:
	[[nodiscard]] const Space *as_space() const override;
};

