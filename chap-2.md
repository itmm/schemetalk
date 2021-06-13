# Parsen der Quelldatei

Ein Vorteil von `SchemeTalk` ist der einfache Aufbau
der Quelldateien.
Eine Quelldatei besteht aus einer Liste von Knoten.
Jeder Knoten kann sofort ausgeführt werden,
sobald er gelesen wurde.
Dabei gibt es zuerst drei Arten von Knoten:

* Freiraum,
* Token und
* Funktionsaufrufe.

Die gemeinsame Basis-Klasse wird in der Datei `node.h` definiert:

```c++
#pragma once
#include <memory>

class Node;

using Node_Ptr = std::shared_ptr<Node>;

class Node {
public:
	virtual ~Node() = default;
	virtual Node_Ptr eval(Node_Ptr self, Node_Ptr state);
};

inline Node_Ptr Node::eval(Node_Ptr self, Node_Ptr state) {
	return self;
}
```

`Node_Ptr` behält die Anzahl der Verweise auf eine Objekt
im Überblick.
Sobald der letzte Verweis verschwindet,
wird das Objekt freigegeben.
Solange wir keine zyklischen Referenzen bauen,
klappt das gut.
Sonst entsteht auch kein Fehler,
wir verschwenden nur Speicher.

Die einfachste Implementierung ist der Freiraum in der Datei
`space.h`:

```c++
#pragma once
#include "node.h"

class Space: public Node {
public:
};
```

Wir müssen nur wissen,
ob ein Knoten ein Freiraum ist.
Der Freiraum selber hat keine zusätzlichen Attribute.

Im Gegensatz dazu hat ein Token in der Datei `token.h`
zumindest einen Wert:

```c++
#pragma once
#include "node.h"
#include <string>

class Token: public Node {
	std::string value_;
public:
	explicit Token(std::string value);
	const std::string &value() const;
};

inline Token::Token(std::string value):
	value_ { std::move(value) }
{ }

inline const std::string& Token::value() const {
	return value_;
}
```

Etwas komplizierter sieht ein Funktionsaufruf aus.
Er besteht grundsätzlich aus einem Knoten,
der auf die auszuführende Funktion zeigt.
Zusätzlich kann eine Liste von weiteren Knoten angegeben werden.
Dies sind die Argumente der Funktion.

Der entsprechende Subtyp ist in der Datei `invocation.h`
definiert:

```c++
#pragma once
#include "node.h"
#include <vector>

class Invocation: public Node {
	Node_Ptr function_;
	using Container = std::vector<Node_Ptr>;
	Container arguments_;
public:
	explicit Invocation(Node_Ptr function);
	const Node_Ptr &function() const;
	void push_back(Node_Ptr argument);
	using Iter = Container::const_iterator;
	Iter begin() const { return arguments_.begin(); }
	Iter end() const { return arguments_.end(); }
};

inline Invocation::Invocation(Node_Ptr function):
	function_ { function }
{ }

inline const Node_Ptr &Invocation::function() const {
	return function_;
}

inline void Invocation::push_back(Node_Ptr argument) {
	arguments_.push_back(argument);
}
```

Wichtig ist an dieser Stelle:
Ich habe noch nicht beschrieben,
wie die entsprechenden Elemente in der Quelldatei aussehen.
Stattdessen habe ich nur angegeben,
welche Informationen sie enthalten,
nachdem sie geparst wurden.

Doch dazu kommen wir mit dem Header `parser.h`:

```c++
#pragma once

#include "node.h"
#include <istream>

std::istream &operator>>(std::istream &in, Node_Ptr &node);
```

Spannend wird natürlich die Implementierung `parser.cpp`:

```c++
#include "parser.h"
#include "err.h"

using int_type = std::istream::int_type;
using char_type = std::istream::char_type;

int_type read_node(
	std::istream &in, int_type ch, Node_Ptr &node
) {
	if (ch <= ' ') {
		// read space
	} else if (ch == '(') {
		// read invocation
	} else if (ch == ')') {
		fail("unmatched ')'");
	} else {
		// read token
	}
	return ch;
}

std::istream &operator>>(std::istream &in, Node_Ptr &node) {
	int_type ch { in.get() };
	node.reset();
	if (ch != EOF) {
		ch = read_node(in, ch, node);
		if (ch != EOF) {
			in.putback(static_cast<char_type>(ch));
		}
	}
	return in;
}
```

```c++
#include "space.h"
// ...
		// read space
		node = std::make_shared<Space>();
		while (ch != EOF && ch <= ' ') {
			ch = in.get();
		}
// ...
```

```c++
#include "token.h"
// ...
		// read token
		std::string value;
		while (ch != EOF && ch > ' ' &&
			ch != '(' && ch != ')'
		) {
			value += static_cast<char_type>(ch);
			ch = in.get();
		}
		node = std::make_shared<Token>(value);
// ...
```

```c++
#include "invocation.h"
// ...
		// read invocation
		ch = in.get();
		Node_Ptr arg;
		bool first_param { true };
		while (ch != ')') {
			ch = read_node(in, ch, arg);
			if (ch == EOF && ! arg) {
				fail("incomplete invocation");
			}
			if (! node) {
				node = std::make_shared<Invocation>(arg);
			} else if (first_param && dynamic_cast<Space *>(
				arg.get()
			)) {
				first_param = false;
				continue;
			} else {
				first_param = false;
				auto inv { dynamic_cast<Invocation *>(
					node.get()
				) };
				if (inv) {
					inv->push_back(arg);
				} else { fail("invalid node"); }
			}
		}
		ch = in.get();
// ...
```

in `st.cpp`: 

```c++
#include "parser.h"
// ...
	std::ifstream source { source_path };
	Node_Ptr state;
	Node_Ptr got;
	for (;;) {
		source >> got;
		if (! got) { break; }
		got->eval(got, state);
	}
// ...
```
