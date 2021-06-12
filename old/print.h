#pragma once

#include <ostream>

#include "node.h"

std::ostream& operator<<(std::ostream &out, const Node &node);
std::ostream& operator<<(std::ostream &out, const Node_Ptr &node);
