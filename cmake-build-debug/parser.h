#pragma once

#include "node.h"
#include <istream>

std::istream &operator>>(std::istream &in, Node_Ptr &node);
