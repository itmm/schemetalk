# Parsen der Quelldatei

Ein Vorteil von `SchemeTalk` ist der einfache Aufbau
der Quelldateien.
Aus der Quelldatei wird als interne Repräsentation ein
_abstrakter Syntax-Baum_ erzeugt.
Für diesen Baum gibt es zuerst drei unterschiedliche Arten:

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
};
```

`Node_Ptr` behält die Anzahl der Verweise auf ein Objekt
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

Die angegebenen Methoden ermöglichen es,
neue Knoten als Kinder in den abstrakten Syntax-Baum
einzuhängen
und auf die ganzen Attribute zuzugreifen.
Über die Argumente kann nur als Ganzes iteriert werden.

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

Es können Knoten aus einem Eingabe-Strom gelesen werden,
so wie andere Daten-Typen auch.
Spannend wird natürlich die Implementierung `parser.cpp`:

```c++
#include "parser.h"
#include "err.h"

using int_type = std::istream::int_type;
using char_type = std::istream::char_type;

int_type read_node(
	std::istream &in, int_type ch, Node_Ptr &node
);

int_type read_invocation(
	std::istream &in, int_type ch, Node_Ptr &node
) {
	// read invocation
	return ch;
}

int_type read_node(
	std::istream &in, int_type ch, Node_Ptr &node
) {
	// read node
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

Bei der Implementierung werden zwei Datentypen unterschieden.
Der `char_type` enthält nur ein Byte und damit alle Werte,
die ein Zeichen annehmen kann.
Der `int_type` kann mehr Werte umfassen.
Er enthält zumindest zusätzlich noch den Wert `EOF`,
sodass das Ende eines Eingabestroms signalisiert werden kann.

Da für das Lesen eines Knotens ein Lookahead von einem Zeichen
benötigt wird,
wird am Anfang ein Zeichen gelesen
und dieses nach dem Parsen in den Eingabe-Strom
zurückgeschrieben.

Das eigentliche Parsen in `parser.cpp` ist eine
Fallunterscheidung:

```c++
// ...
	// read node
	if (ch <= ' ') {
		// read space
	} else if (ch == '(') {
		ch = read_invocation(in, ch, node);
	} else if (ch == ')') {
		fail("unmatched ')'");
	} else {
		// read token
	}
// ...
```

Freizeichen werden beim Parsen zusammengefasst:

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

Ein Token besteht aus allen Zeichen bis zu:

* einem Freizeichen,
* einer öffnenden Klammer,
* einer schließenden Klammer,
* dem Dateiende.

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
		do {
			// generate token subtypes
			node = std::make_shared<Token>(value);
		} while (false);
// ...
```

## Parsen eines Funktionsaufrufs

Ein Funktionsaufruf besteht aus der öffnenden Klammer `(`,
einer Liste von Knoten und einer schließenden Klammer `)`.
Die Knoten können auch wieder Funktionsaufrufe sein.
Diese Liste darf nicht leer sein und der erste Knoten darf
kein Freizeichen sein.

Hier der grobe Rahmen in `parser.cpp`:

```c++
#include "invocation.h"
// ...
	// read invocation
	node.reset();
	ch = in.get();
	Node_Ptr arg;
	bool first_param { true };
	while (ch != ')') {
		// read invocation arg
	}
	ch = in.get();
// ...
```

Der erste Knoten beschreibt die Funktion,
die aufgerufen werden soll.
Ein Freizeichen nach der Funktion wird ignoriert.
Alle weiteren Knoten bilden die Argumente,
mit welcher die Funktion aufgerufen wird.

```c++
// ...
		// read invocation arg
		ch = read_node(in, ch, arg);
		if (ch == EOF && ! arg) {
			fail("incomplete invocation");
		}
		if (! node) {
			if (dynamic_cast<Space *>(arg.get())) {
				fail("space after (");
			}
			node = std::make_shared<Invocation>(arg);
		} else if (first_param && dynamic_cast<Space *>(
			arg.get()
		)) {
			first_param = false;
		} else {
			first_param = false;
			auto inv { dynamic_cast<Invocation *>(
				node.get()
			) };
			if (inv) {
				inv->push_back(arg);
			} else { fail("invalid node"); }
		}
// ...
```

Um die Hauptschleife zu implementieren,
brauchen die Knoten noch eine weitere Methode:
sie müssen ausgeführt werden können.
Die Standard-Implementierung liefert einfach nur den Knoten
zurück.

Die Datei `node.h` muss wie folgt angepasst werden:

```c++
// ...
public:
	virtual Node_Ptr eval(Node_Ptr self, Node_Ptr state);
// ...
inline Node_Ptr Node::eval(Node_Ptr self, Node_Ptr state) {
	return self;
}
```

Wichtig ist, dass der Parameter `self` übergeben wird.
Die Methode liefert keinen `Node*`,
sondern einen `Node_Ptr` zurück.
Der kann aber nicht so einfach aus einem Knoten erzeugt werden.
Leichter ist es,
wenn er einfach mit übergeben wird.

Damit kann die Hauptschleife in `st.cpp` implementiert werden:

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
