# Der Zustandsbaum

in `map.h`:

```c++
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
```

in `st.cpp`:

```c++
#include "map.h"
// ...
	Node_Ptr state;
	state = std::make_shared<Map>(Node_Ptr { });
// ...
```

in `token.h`:

```c++
#pragma once
#include "map.h"
// ...
public:
	Node_Ptr eval(Node_Ptr self, Node_Ptr state) override;
// ...
inline Node_Ptr Token::eval(Node_Ptr self, Node_Ptr state) {
	auto s { dynamic_cast<Map *>(state.get()) };
	if (! s) { fail("invalid state"); }
	Node_Ptr result { s->get(value_) };
	return result ? result : self;
}
```

in `command.h`:

```c++
#pragma once
#include "node.h"

class Command: public Node {
public:
	virtual Node_Ptr invoke(
		Node_Ptr invocation, Node_Ptr state
	) = 0;
};
```

in `invocation.h`:

```c++
// ...
public:
	Node_Ptr eval(Node_Ptr self, Node_Ptr state) override;
// ...
```

in `invocation.cpp`:

```c++
#include "invocation.h"
#include "map.h"
#include "command.h"

Node_Ptr Invocation::eval(Node_Ptr self, Node_Ptr state) {
	auto s { dynamic_cast<Map *>(state.get()) };
	if (! s) {
		fail("wrong state type");
	}
	auto fn = dynamic_cast<Command *>(
		function_->eval(function_, state).get()
	);
	if (! fn) {
		fail("no function in invocation");
	}
	return fn->invoke(self, state);
}
```
