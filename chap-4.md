# Ausgabe

in `io.h`:

```c++
#pragma once
#include "node.h"

void register_io_entries(Node_Ptr state);
```

in `node.h`:

```c++
#pragma once
#include <ostream>
// ...
public:
	virtual void write(std::ostream &out) { }
// ...
#include "err.h"
inline std::ostream &operator<<(std::ostream &out, Node_Ptr node) {
	if (! node) {
		fail("invalid node");
	}
	node->write(out);
	return out;
}
```

in `space.h`:

```c++
// ...
public:
	void write(std::ostream &out) override;
// ...
inline void Space::write(std::ostream &out) {
	out << ' ';
}
```

in `token.h`:

```c++
// ...
public:
	void write(std::ostream &out) override;
// ...
inline void Token::write(std::ostream &out) {
	out << value_;
}
```

in `node.h`:

```c++
// ...
extern Node_Ptr nil;
```

in `st.cpp`:

```c++
// ...

Node_Ptr nil;
// ...
	Node_Ptr state;
	nil = std::make_shared<Node>();
// ...
```

in `io.cpp`:

```c++
#include "io.h"
#include "invocation.h"
#include "map.h"
#include "command.h"
#include <iostream>

class Write: public Command {
public:
	Node_Ptr invoke(Node_Ptr invocation, Node_Ptr state) override;
};

Node_Ptr Write::invoke(Node_Ptr invocation, Node_Ptr state) {
	auto inv { dynamic_cast<Invocation *>(invocation.get()) };
	if (! inv) { fail("no invocation"); }
	for (auto arg : *inv) {
		std::cout << arg->eval(arg, state);
	}
	return nil;
}

class Write_Newline: public Write {
public:
	Node_Ptr invoke(Node_Ptr invocation, Node_Ptr state) override;
};

Node_Ptr Write_Newline::invoke(Node_Ptr invocation, Node_Ptr state) {
	Write::invoke(invocation, state);
	std::cout << '\n';
	return nil;
}

void register_io_entries(Node_Ptr state) {
	auto s { dynamic_cast<Map *>(state.get()) };
	if (! s) { fail("no state"); }
	s->insert("write", std::make_shared<Write>());
	s->insert("write-line", std::make_shared<Write_Newline>());
}
```

in `st.cpp`:

```c++
#include "io.h"
// ...
	Node_Ptr got;
	register_io_entries(state);
// ...
```

in `node.h`:

```c++
// ...
void register_node_entries(Node_Ptr state);
```

in `node.cpp`:

```c++
#include "node.h"
#include "map.h"

void register_node_entries(Node_Ptr state) {
	auto s { dynamic_cast<Map *>(state.get()) };
	if (! s) { fail("no state"); }
	s->insert("nil", nil);
}
```
