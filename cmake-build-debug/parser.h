#line 135 "/home/timm/prj/schemetalk/chap-2.md"
#pragma once

#include "node.h"
#include <istream>

std::istream &operator>>(std::istream &in, Node_Ptr &node);
