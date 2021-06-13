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
Zuerst brauchen wir die Hauptfunktion `main` in der
Quell-Datei `st.cpp`:

```c++
int main(int argc, const char *argv[]) {
}
```

Das auszuführende Programm muss gelesen werden.
Jeder Ausdruck kann nach dem Lesen direkt ausgeführt werden.
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
