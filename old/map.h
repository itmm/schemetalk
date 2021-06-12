#pragma once

#include <map>
#include <string>
#include <utility>

#include "node.h"

class Map;

class Map: public Node {
	Node_Ptr parent_;
	using Collection = std::map<std::string, Node_Ptr>;
	Collection values_;
public:
	using Iter = Collection::const_iterator;
	explicit Map(Node_Ptr parent = Node_Ptr { });
	[[nodiscard]] Iter begin() const { return values_.begin(); }
	[[nodiscard]] Iter end() const { return values_.end(); }
	[[nodiscard]] const Map *as_map() const override;
	[[nodiscard]] Map *as_map() override;
	[[nodiscard]] Node_Ptr find(const std::string& name) const;
	void push(const Node_Ptr& node, const std::string& name);
};

inline Map::Map(Node_Ptr parent):
	parent_ { std::move(parent) }
{ }

void add_map_commands(const Node_Ptr &state);
