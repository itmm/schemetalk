# Die Programmiersprache SchemeTalk

In diesem Abschnitt möchte ich die Programmiersprache `SchemeTalk`
vorstellen.
Genauer gesagt:
einen Interpreter dieser Sprache.
Es wird sich hoffentlich zeigen,
dass es gar nicht _so_ schwer ist,
einen solchen Interpreter für eine einfache Sprache zu erstellen.
Diese Sprache soll in zukünftigen Kapitel unser Werkzeug sein,
um zum Beispiel Dokumente in formatiert in PDF-Dateien zu
schreiben.

Eine Grund-Motivation für diese Sprache waren genau diese
Dokumente.
Im Gegensatz zu anderen gängigen Hochsprachen
werden Freizeichen nicht grundsätzlich ignoriert.
Es macht einen Unterschied,
ob eine Funktion am Ende eines Wortes oder hinter dem Wort
aufgerufen wird.
Sprachen wie TeX bieten meiner Meinung nach dafür keine
befriedigende Lösung.

Daher die Sprache `SchemeTalk`.
Sie ist verwendet eine Syntax,
die den Sprachen `Scheme` (oder `Lisp`) und `Smalltalk`
entlehnt ist.
`Scheme` ist mit seiner einfachen Listen-Struktur leicht zu
verarbeiten.
Zumindest die Teil-Sprache,
die hier verwendet wird.
`Smalltalk` hat auf der anderen Seite einen sehr sympathischen
Methoden-Aufruf.
Die Parameter werden nicht einfach hintereinander geschrieben,
sondern erhalten eine Beschreibung vorangestellt.
Diese Syntax,
die auch in Objective-C verwendet wurde,
finde ich sehr angenehm für das Verständnis.
Dies sieht man schon daran,
dass moderne IDEs für Java oder C++ diese Beschreibungen in
den angezeigten Quell-Code einfügen.

Den Interpreter selber habe ich in C++ geschrieben.
Durch die freie Verfügbarkeit von `Clang` und `G++` sind
Compiler kostenlos für viele Plattformen erhältlich.
Und Niklaus Wirth würde mich vermutlich verachten,
wenn ich einen Interpreter für eine Programmiersprache in einer
Programmiersprache schreibe,
die selber interpretiert wird.

Es ist auch gar nicht so schwer.
Durch mein Programm `md-patcher` können wir das Programm
Schritt für Schritt herleiten.
Grob besteht die Hauptfunktion `main` in der Quell-Datei `st.cpp`
aus zwei Teilen:

```c++
int main(int argc, const char *argv[]) {
}
```

Zuerst muss das auszuführende Programm gelesen werden.
Dann muss es ausgeführt werden.
`main` selbst ist in C++ die Funktion,
die beim Start des Programms ausgeführt wird.
Sie erhält zwei Argumente.
`argc` enthält die Anzahl der Argumente,
die dem Programmaufruf von der Konsole übergeben wurden.
In `argv` stehen diese Argumente.

Das minimale Programm lässt sich so schon übersetzen.
Es macht halt noch nichts Sinnvolles.

Die Argumente der Kommandozeile werden benötigt,
da das erste Argument der Pfad der zu interpretierenden
Datei sein muss.
`SchemeTalk` kann nicht als Filter operieren,
der aus der Standard-Eingabe den Quelltext erwartet.
Dann könnte das interpretierte Programm selber keine Daten
mehr von der Standard-Eingabe lesen.
Dies ist jedoch zu unflexibel.
Daher muss das Programm selber aus einer Datei gelesen
werden.

Bei der Verwendung von `argc` und `argv` ist zu beachten,
dass der Programm-Aufruf selber als erstes Argument mitgezählt
wird.
Das von uns gesuchte Argument steht in `argv[1]` und `argc` muss
den Wert `2` haben.
Hier die Erweiterung in der Datei `st.cpp`:

```c++
#include "err.h"
#include <fstream>

int main(int argc, const char *argv[]) {
	if (argc != 2) {
		fail("expect only one source file as argument");
	}
	const char *source_path { argv[1] };
	std::ifstream source { source_path };
}
```

Die Hilfsfunktion `fail` gibt eine Fehlermeldung aus
und beendet das Programm.
Sie ist in der Datei `err.h` definiert:

```c++
#pragma once
#include <string>

[[noreturn]] void fail(const std::string &msg);
```

Und in der Datei `err.cpp` definiert:

```c++
#include "err.h"
#include <iostream>
#include <cstdlib>

[[noreturn]] void fail(const std::string &msg) {
	std::cerr << "!SchemeTalk failed: " << msg << '\n';
	std::exit(EXIT_FAILURE);
}
```

Erst einmal wirkt es recht harsch,
nur genau eine Datei zuzulassen.
Was ist mit Erweiterungen und Optionen?
Die gibt es noch nicht.
Daher werden sie nicht berücksichtigt.
Der Rest des Fragments öffnet die Quelldatei zum Lesen.

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

const Node_Ptr &Invocation::function() const {
	return function_;
}

void Invocation::push_back(Node_Ptr argument) {
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
		while (ch != ')') {
			ch = read_node(in, ch, arg);
			if (ch == EOF && ! arg) {
				fail("incomplete invocation");
			}
			if (! node) {
				node = std::make_shared<Invocation>(arg);
			} else {
				auto inv { dynamic_cast<Invocation *>(node.get()) };
				if (inv) {
					inv->push_back(arg);
				} else { fail("invalid node"); }
			}
		}
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
