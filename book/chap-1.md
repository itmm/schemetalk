# Einleitung

Was ist PDF?
Klar, jeder kennt das "Portable Document Format",
mit dem heute im Internet fast jede Form von Dokument
ausgetauscht werden kann.
Egal, ob Berichte, Bücher, Präsentationen, Verträge:
Alles kann in diesem Format ausgetauscht werden.
Aber wie sieht dieses Dateiformat eigentlich aus?
Welche Elemente enthält es?
Wie kann man selber ein Programm schreiben,
dass solche Dateien erzeugt?
Vielleicht sogar eine Auszeichnungssprache,
die direkt solche Dokumente erzeugt.
Ohne erst Umwege über TeX, Pandoc, Word oder Ähnliches zu nehmen?
Darum soll es in diesem Buch gehen:
Ziel ist es, ein Programm zu schreiben,
das Programme einer Lisp-ähnlichen Sprache interpretiert
und als Ausgabe PDF-Dokumente erzeugt.
Und dabei greifen wir nicht auf irgendwelche Bibliotheken zurück,
sondern verwenden nur eine gewöhnliche moderne C++ Umgebung.
Alles was nicht im namespace std vorhanden ist,
ist als Quellcode in diesem Buch enthalten.

Dabei wechseln wir zwischen unterschiedlichen Themen.
Zum einen beschäftigen wir uns mit dem PDF-Format.
Und zwar einem Teil davon, der notwendig ist,
um dieses Buch zu erzeugen.

Dazu gehört natürlich ein klein wenig Layout-Wissen,
damit das Buch nicht völlig unterirdisch aussieht.
Aber viele Bereiche von PDF können wir erst einmal außen vor lassen.
So werden keine Grafiken oder besonderen Zeichensätze unterstützt.
Es geht nur darum,
mit einfachen Mitteln ein ansprechendes Ergebnis zu setzen.

Um flexibel und erweiterbar zu bleiben,
wird eine Programmiersprache eingeführt.
Mit dieser können Programme geschrieben werden,
die als Ausgabe ein PDF-Dokument erzeugen.
Gleichzeitig handelt sich aber um eine vollständige Programmiersprache.
Es können also auch beliebige Berechnungen durchgeführt werden
und das Ergebnis ansprechend in einem schön gesetzten Text ausgegeben werden.
Um den Umfang nicht zu sprengen,
gibt es aber nur eine relativ einfache Sprache.
Aber trotzdem ist sie sehr mächtig.

# Grundaufbau von PDF

Man soll es nicht für möglich halten,
aber theoretisch kann man eine PDF-Datei in einem Text-Editor schreiben.
Es kann zwar komprimierte Byte-Ströme enthalten,
über welche Editoren stolpern,
aber das muss nicht sein.
In diesem Buch generieren wir nur reinen Text.
Damit bleiben die Ergebnisse lesbar und können hier sogar abgedruckt werden.

Jede PDF-Datei besteht aus mindestens drei Teilen:

* einem Header,
* einer Liste von Objekten und
* einem Trailer.

Der Header muss nur aus einer Zeilen bestehen:

```
%PDF-1.4
```

Das Prozentzeichen leitet einen Kommentar ein.
Der Rest der Zeile wird vom Interpreter eigentlich ignoriert.
Es gibt aber ein paar ganz spezielle Kommentare die notwendig sind,
damit eine Datei eine legale PDF-Datei ist.
Dieser Kommentar ist einer davon.

Die Zahl hinter dem `PDF-` signalisiert die PDF-Version,
in der das Dokument geschrieben ist.
In unserem Fall 1.4.
Das ist die Version, die von Acrobat 5 verwendet wurde.
Die ist schon recht angestaubt,
aber enthält die spannendsten Elemente von PDF.
Hauptsächlich verwende ich es,
da ich die 3rd edition von der PDF Reference von Adobe erstanden habe.
Und die beschreibt genau die Version 1.4 von PDF.
Die PDF Reference kann ich wärmstens empfehlen.
Im Gegensatz zum offiziellen PDF-Standard,
kann man das Buch richtig gut lesen.
Leider gibt es das Buch nur bis zur Version 1.7.
Und das habe ich noch nicht günstig gefunden.
Zwar kann man sich die Versionen auch bei Adobe direkt herunterladen,
aber ich bevorzuge ein gedrucktes Buch,
um schnell hin und her zu springen.

Fangen wir gleich mit dem Coding an.
In C++ können wir eine Klasse `PDF_Writer` schreiben,
die einen Ausgabestrom erhält.
Im Konstruktor wird der Header geschrieben
und im Destruktor der Trailer.
Dazwischen kommen dann noch Methoden,
um die einzelnen Objekte zu schreiben.
Der Header `pdf.h` kann wie folgt aussehen

```c++
#pragma once
#include <ostream>

class Pdf_Writer {
	std::ostream &out_;
	void write_header();
	void write_trailer();
public:
	explicit Pdf_Writer(std::ostream &out);
	~Pdf_Writer();
};
```

Und die passende Implementierung `pdf.cpp`:

```c++
#include "pdf.h"

Pdf_Writer::Pdf_Writer(std::ostream &out):
	out_ { out }
{
	write_header();
}

Pdf_Writer::~Pdf_Writer() {
	write_trailer();
}

void Pdf_Writer::write_header() {
	out_ << "%PDF-1.4\n";
}
```

Die `write_trailer`-Methode können wir leider nicht so einfach definieren.
Die ist relativ knifflig.
Zuerst muss eine Tabelle mit Querverweisen geschrieben werden.
Für jedes Objekt muss angegeben werden,
an welcher Position es in der Datei steht.
Danach kommt der eigentliche Trailer,
der angibt wo das Haupt-Objekt zu finden ist.
Zum Schluss wird angegeben, wo die Tabelle mit den Querverweisen startet.

Zuerst brauchen wir eine Methode, die uns die aktuelle Datei-Position
zurückliefert.
In `pdf.h` können wir die Funktion `position` ergänzen:

```c++
// ...
class Pdf_Writer {
	// ...
public:
	// ...
	[[nondiscard]] auto position() const {
		return out_.tellp();
	}
}
```

Damit kann dann die Trailer-Implementierung in `pdf.cpp` ganz abstrakt so
aussehen:

```c++
void Pdf_Writer::write_trailer() {
	auto xref { position() };
	write_xref();
	out_ << "trailer\n";
	write_trailer_dict();
	out_ << "startxref\n";
	out_ << xref << '\n';
	out_ << "%%EOF\n";
}
```

Hier sehen wir wieder einen ganz besonderen Kommentar `%%EOF` ganz am Ende
der Datei.
Davor kommt das Schlüsselwort `startxref` mit dem Start der Querverweise.
Davor kommt das Schlüsselwort `trailer` mit einem Verzeichnis, das zum
Beispiel auf das Start-Objekt zeigt.
Davor kommen die Querverweise.

Die Beschreibung vom Ende her zeigt genau das Vorgehen eines PDF-Lesers.
Sobald er anhand des Headers erkennt,
dass es sich um ein gültiges PDF-Dokument handelt,
springt er ganz an das Ende der Datei.
Dort findet er den speziellen Kommentar als Ende-Markierung 
und davor die Position, ab der die Querverweise liegen.
Er springt an die Stelle mit den Querverweisen und liest diese
sowie das nachfolgende Verzeichnis.

Dieses Vorgehen erleichtert das Generieren von PDF-Dateien.
Anstatt schon im Header anzugeben wo das Haupt-Objekt steht,
muss dies erst am Ende geschehen.
Wenn während des Schreibens das Haupt-Objekt geschrieben wird,
muss nur die aktuelle Position festgehalten werden.
Die Dokumente können so in einem Durchlauf geschrieben werden.
Das wird für uns das Erstellen eines PDF-Dokuments merklich vereinfachen.

Wenn wir die Aufrufe von noch nicht vorhandenen Methoden auskommentieren
(damit das Programm kompiliert) können wir mit der folgenden Datei `main.cpp`
ein vollständiges Programm erstellen:

```c++
#include "pdf.h"
#include <iostream>

int main() {
	Pdf_Writer writer(std::cout);
}
```

Es produziert folgende Ausgabe:

```
%PDF-1.4
trailer
startxref
9
%%EOF
```

Das ist noch kein gültiges PDF-Dokument.
Es fehlen die Querverweise und das Verzeichnis am Ende.
Aber es ist schon mal ein Anfang,
auf den wir aufbauen können.

Kommen wir zuerst zu Objekten.
Jedes Objekt besteht aus einer ID und einer Versions-Nummer.
So ganz habe ich den Sinn der Versions-Nummer nicht verstanden.
Wir werden stets dafür `0` nehmen.
So müssen wir uns nur die ID merken.
Dazu gibt es eine Tabelle im `Pdf_Writer`,
welche sich diesen Zusammenhang merkt.
Der Header `pdf.h` muss wie folgt angepasst werden:

```c++
// ...
#include <map>
class Pdf_Writer {
	// ...
	std::map<int, std::ostream::pos_type> obj_poss_;
public:
	// ...
	void write_obj(int id);
};
```

Und in der Implementierung `pdf.cpp` entsprechend:

```c++
// ...
#include <cassert>

void write_obj(int id) {
	assert(! obj_poss_[id]);
	obj_poss_[id] = position();
}
```

Es wird dabei geprüft,
ob das Objekt bereits geschrieben wurde.
Ansonsten wird die aktuelle Position als Start-Position gespeichert.

Die Querverweis-Tabelle enthält nun eine sortierte Liste aller Objekte
zusammen mit deren Position in der Datei.
Jeder Eintrag hat eine feste Länge.
Die Zahlen werden entsprechend mit `0`-Ziffern aufgefüllt.

Am Ende der Zeile ist ein Freizeichen!
Wenn man als Zeilentrenner den Windows-Standard verwendet,
werden zwei Byte für Newline und Carriage-Return verwendet.
Da unter fast allen anderen Betriebssystemen dafür nur ein Newline reicht,
sorgt das zusätzliche Freizeichen dafür,
dass die Byte-Länge der Zeilen stimmt.
