#line 7 "/home/timm/prj/schemetalk/chap-3.md"
#pragma once
#include "err.h"
#include "node.h"
#include <map>
#include <string>

class Map: public Node {
	Node_Ptr parent_;
	std::map<std::string, Node_Ptr> entries_;
public:
	explicit Map(Node_Ptr parent);
	void insert(const std::string &key, Node_Ptr value);
	Node_Ptr get(const std::string &key) const;
	template<typename F> void each(const F& f) const;
};

inline Map::Map(Node_Ptr parent):
	parent_ { parent}
{ }

inline void Map::insert(
	const std::string &key, Node_Ptr value
) {
	entries_[key] = value;
}

inline Node_Ptr Map::get(const std::string &key) const {
	auto got { entries_.find(key) };
	if (got != entries_.end()) {
		return got->second;
	} else if (parent_) {
		auto p { dynamic_cast<Map *>(parent_.get() )};
		if (p) {
			return p->get(key);
		} else {
			fail("invalid parent");
		}
	} else {
		return Node_Ptr { };
	}
}

template<typename F> inline void Map::each(const F& f) const {
	if (parent_) {
		auto p { dynamic_cast<Map *>(parent_.get() )};
		if (p) {
			p->each(f);
		} else { fail("invaild parent"); }
	}
	for (const auto &entry : entries_) {
		f(entry.first, entry.second);
	}
}
