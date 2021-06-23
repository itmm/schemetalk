# Arithmetik

in `num.h`:

```c++
#pragma once
#include "node.h"

class Number: public Node {
	double value_;
public:
	Number(double value): value_ { value } { }
	
	double value() const { return value_; }
	void write(std::ostream &out) override {
		out << value_;
	}
};
```

in `parser.cpp`:

```c++
// ...
#include "err.h"
#include "num.h"

bool is_number(const std::string token) {
	auto it { token.begin() };
	if (it != token.end() && (*it == '+' || *it == '-')) {
		++it;
	}
	int digits { 0 };
	while (it != token.end() && *it >= '0' && *it <= '9') {
		++it; ++digits;
	}
	if (it != token.end() && *it == '.') {
		++it;
		while (it != token.end() && *it >= '0' && *it <= '9') {
			++it; ++digits;
		}
	}
	return it == token.end() && digits;
}

double to_number(const std::string value) {
	return std::stod(value);
}
// ...
			// generate token subtypes
			if (is_number(value)) {
				node = std::make_shared<Number>(to_number(value));
				break;
			}
// ...
```

in `arith.h`:

```c++
#pragma once

#include "node.h"

void register_arith_entries(Node_Ptr state);
```

in `arith.cpp`:

```c++
#include "arith.h"
#include "command.h"
#include "num.h"
#include "invocation.h"
#include "err.h"
#include "space.h"
#include "token.h"

class Add: public Command {
public:
	Node_Ptr invoke(Node_Ptr invocation, Node_Ptr state) override;
};

static Invocation::Iter eat_space(
	Invocation::Iter it, Invocation::Iter end
) {
	while (it != end && dynamic_cast<Space *>(it->get())) { ++it; }
	return it;
}

static Invocation::Iter parse_one_arg(
	const std::string &key, double &value,
	const Node_Ptr& state, Invocation::Iter it,
	Invocation::Iter end
) {
	it = eat_space(it, end);
	if (it != end && dynamic_cast<Token *>(it->get()) && dynamic_cast<Token *>(it->get())->value() == key) {
		++it;
	} else { fail("expected key '" + key + "'"); }
	it = eat_space(it, end);
	if (it == end) { fail("expected value after key '" + key + "'"); }
	Node_Ptr val { (**it).eval(*it, state) };
	if (val && dynamic_cast<Number *>(val.get())) {
		value = dynamic_cast<Number *>(val.get())->value();
		++it;
	} else { fail("no value after key '" + key + "'"); }
	return it;
}

static void parse_two_args_cmd(
	const Node_Ptr& invocation,
	const std::string &key1, double &value1,
	const std::string &key2, double &value2,
	const Node_Ptr& state
) {
	auto inv { dynamic_cast<Invocation *>(invocation.get()) };
	if (! inv) { fail("no invocation"); }
	auto it { inv->begin() };
	auto end { inv->end() };
	it = parse_one_arg(key1, value1, state, it, end);
	it = parse_one_arg(key2, value2, state, it, end);
	it = eat_space(it, end);
	if (it != end) { fail("too many parameters"); }
}

Node_Ptr Add::invoke(Node_Ptr invocation, Node_Ptr state) {
	double value, to;
	parse_two_args_cmd(
		invocation, "value:", value,
		"to:", to, state
	);
	return std::make_shared<Number>(value + to);
}

void register_arith_entries(Node_Ptr state) {
	auto s { dynamic_cast<Map *>(state.get()) };
	if (! s) { fail("no state"); }
	s->insert("add", std::make_shared<Add>());
}
```

in `st.cpp`:

```c++
#include "arith.h"
// ...
	Node_Ptr got;
	register_arith_entries(state);
// ...
```