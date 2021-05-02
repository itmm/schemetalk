#pragma once

#include <istream>

#include "node.h"

std::istream &operator>>(std::istream &in, Node_Ptr &node);