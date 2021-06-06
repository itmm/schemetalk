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
Zum einen beschäftigen wir uns mit dem PDF.
Und zwar einem Teil davon, der notwendig ist,
um dieses Buch zu erzeugen.

Dazu gehört natürlich ein klein wenig Layout-Wissen,
damit das Buch nicht völlig unterirdisch aussieht.
Aber viele Bereiche von PDF lassen wir erst einmal außen vor.
So werden keine Grafiken oder besonderen Zeichensätze unterstützt.
Es geht nur darum,
mit einfachen Mitteln ein ansprechendes Ergebnis zu setzen.

Um flexibel und erweiterbar zu bleiben,
wird eine Programmiersprache eingeführt.
Mit dieser können Programme geschrieben werden,
die als Ausgabe ein PDF-Dokument erzeugen.
Gleichzeitig handelt sich aber um eine vollständige
Programmiersprache.
Es können also auch beliebige Berechnungen durchgeführt werden
und das Ergebnis ansprechend in einem schön gesetzten Text
ausgegeben werden.
Um den Umfang nicht zu sprengen,
gibt es aber nur eine relativ einfache Sprache.
Aber trotzdem ist sie sehr mächtig.

# Grundaufbau von PDF

Man soll es nicht für möglich halten,
aber theoretisch kann man eine PDF-Datei in einem Text-Editor
schreiben.
Es kann zwar komprimierte Byte-Ströme enthalten,
über welche Editoren stolpern,
aber das muss nicht sein.
In diesem Buch generieren wir nur reinen Text.
Damit bleiben die Ergebnisse lesbar und können hier sogar
abgedruckt werden.

Jede PDF-Datei besteht aus mindestens drei Teilen:

* einem Header,
* einer Liste von Objekten und
* einem Trailer.

Der Header muss nur aus einer Zeile bestehen:

```
%PDF-1.4
```

Das Prozentzeichen leitet einen Kommentar ein.
Der Rest der Zeile wird vom Interpreter eigentlich ignoriert.
Es gibt aber ein paar ganz spezielle Kommentare,
die notwendig sind, damit eine Datei eine legale PDF-Datei ist.
Dieser Kommentar ist einer davon.

Die Zahl hinter dem `PDF-` signalisiert die PDF-Version,
in der das Dokument geschrieben ist.
In unserem Fall 1.4.
Das ist die Version, die von Acrobat 5 verwendet wurde.
Die ist schon recht angestaubt,
aber enthält die spannendsten Elemente von PDF.
Hauptsächlich verwende ich es,
da ich die 3rd edition von der PDF Reference von Adobe erstanden
habe.
Und die beschreibt genau die Version 1.4 von PDF.
Die PDF Reference kann ich wärmstens empfehlen.
Im Gegensatz zum offiziellen PDF-Standard,
kann man das Buch richtig gut lesen.
Leider gibt es das Buch nur bis zur Version 1.7.
Und das habe ich noch nicht günstig gefunden.
Zwar kann man sich die Versionen auch bei Adobe direkt
herunterladen,
aber ich bevorzuge ein gedrucktes Buch,
um schnell hin- und herzuspringen.

Fangen wir gleich mit dem Coding an.
In C++ können wir eine Klasse `PDF_Writer` schreiben,
die einen Ausgabestrom erhält.
Im Konstruktor wird der Header geschrieben
und im Destruktor der Trailer.
Dazwischen kommen dann noch Methoden,
um die einzelnen Objekte zu schreiben.
Der Header `pdf.h` kann wie folgt aussehen:

```c++
#pragma once
#include <ostream>

class Pdf_Writer {
	std::ostream &out_;
	int position_ { 0 };
	void write(const std::string &str);
	void write_header();
	void write_trailer();
public:
	explicit Pdf_Writer(std::ostream &out);
	~Pdf_Writer();
};
```

Leider müssen wir uns die Anzahl der aktuelle geschriebenen
Bytes selber merken.
Zwar hat `std::ostream` die Methode `tellp`,
die jedoch stets `-1` liefert,
wenn die Ausgabe nicht in eine Datei umgeleitet wird.
Die Idee dahinter hängt mit der Methode `seekp` zusammen:
`tellp` liefert nur dann eine Position,
wenn man auch mit `seekp` wieder dahin springen kann.
Das funktioniert aber nur bei bidirektionalen Streams,
wie zum Beispiel Dateien.

Das macht die Implementierung etwas umständlicher.
Die `write` Methode gibt eine Zeichenkette aus
und aktualisiert dabei die `position_`.
Alle Ausgaben nach `out_` müssen über diese Methode laufen
(sonst passt die Position nicht mehr).
Lediglich am Ende können wir mogeln,
wenn wir sicher sind,
dass die aktuelle Position nicht mehr benötigt wird.

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

void Pdf_Writer::write(const std::string &str) {
	out_ << str;
	position_ += str.size();
}

void Pdf_Writer::write_header() {
	write("%PDF-1.4\n");
}
```

Die `write_trailer`-Methode können wir leider nicht so einfach
definieren.
Die ist relativ knifflig.
Zuerst muss eine Tabelle mit Querverweisen geschrieben werden.
Für jedes Objekt muss angegeben werden,
an welcher Position es in der Datei steht.
Danach kommt der eigentliche Trailer,
der angibt wo das Haupt-Objekt zu finden ist.
Zum Schluss wird angegeben, wo die Tabelle mit den Querverweisen
startet.

Zuerst brauchen wir eine Methode `position` in `pdf.h`, die uns
die aktuelle Datei-Position zurückliefert:

```c++
// ...
class Pdf_Writer {
	void write_xref();
	void write_trailer_dict();
// ...
public:
	[[nodiscard]] auto position() const {
		return position_;
	}
	// ...
};
```

Damit kann dann die Trailer-Implementierung in `pdf.cpp` ganz
abstrakt so aussehen:

```c++
// ...
void Pdf_Writer::write_trailer() {
	int xref { position() };
	write_xref();
	out_ << "trailer\n";
	write_trailer_dict();
	out_ << "startxref\n";
	out_ << xref << '\n';
	out_ << "%%EOF\n";
}
```

Ab der Definition von `xref` können wir wieder direkt in
`out_` schreiben.
Das war der letzte Zugriff auf die `position`.
Das hat nicht nur ästhetische Vorteile.
Neben der Ausgabe von Zahlen
(die wir später auch noch brauchen),
müssen Zahlen auch auf eine bestimmte Breite aufgefüllt werden.
Es ist natürlich angenehmer,
hier die im `std::ostream` vorhandene Funktionalität zu nutzen,
anstatt diese nachzubauen.

Hier sehen wir wieder einen ganz besonderen Kommentar `%%EOF`
ganz am Ende der Datei.
Davor kommt das Schlüsselwort `startxref` mit dem Start der
Querverweise.
Davor kommt das Schlüsselwort `trailer` mit einem Verzeichnis,
das zum Beispiel auf das Start-Objekt zeigt.
Davor kommen die Querverweise.

Die Beschreibung vom Ende her zeigt genau das Vorgehen eines
PDF-Lesers.
Sobald er anhand des Headers erkennt,
dass es sich um ein gültiges PDF-Dokument handelt,
springt er ganz an das Ende der Datei.
Dort findet er den speziellen Kommentar als Ende-Markierung 
und davor die Position, ab der die Querverweise liegen.
Er springt an die Stelle mit den Querverweisen und liest diese
sowie das nachfolgende Verzeichnis.

Dieses Vorgehen erleichtert das Generieren von PDF-Dateien.
Anstatt schon im Header anzugeben,
wo das Haupt-Objekt steht,
muss dies erst am Ende geschehen.
Wenn während des Schreibens das Haupt-Objekt geschrieben wird,
muss nur die aktuelle Position festgehalten werden.
Die Dokumente können so in einem Durchlauf geschrieben werden.
Das wird für uns das Erstellen eines PDF-Dokuments merklich
vereinfachen.

Wenn wir die Aufrufe von noch nicht vorhandenen Methoden
auskommentieren (damit das Programm kompiliert),
können wir mit der folgenden Datei `main.cpp`
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

Zuerst gibt es eine Möglichkeit, um neue Objekte zu reservieren.
Der Umgang ist ein zweistufiger Prozess:
Zuerst wird eine bisher noch nicht verwendete ID für ein Objekt
reserviert.
Danach kann dann das Objekt irgendwann definiert werden.
Wichtig ist, dass auf ein Objekt bereits Bezug genommen werden
kann bevor es definiert wurde.
Das ermöglicht es uns, die Dateien in einem Rutsch zu schreiben.

Zuerst wird im `Pdf_Writer` hinterlegt, was die zuletzt 
reservierte ID ist.
Dies diehnt der Methode `reserve_obj_id` zum Reservieren weiterer
IDs.
Die Datei `pdf.h` muss dazu wie folgt ergänzt werden:

```c++
// ...
class Pdf_Writer {
	int last_obj_id_ { 0 };
	// ...
public:
	int reserve_obj_id() {
		return ++last_obj_id_;
	}
	// ...
};
```

Zusätzlich gibt es eine Tabelle im `Pdf_Writer`,
welche sich für jedes Objekt die Position in der Datei merkt,
an der es definiert wurde.
Der Header `pdf.h` muss wie folgt angepasst werden:

```c++
#pragma once
#include <map>
// ...
class Pdf_Writer {
	std::map<int, int> obj_positions_;
	// ...
public:
	void define_obj(int id);
	// ...
};
```

Und in der Implementierung `pdf.cpp` entsprechend:

```c++
// ...
#include <cassert>

void Pdf_Writer::define_obj(int id) {
	assert(id <= last_obj_id_);
	assert(! obj_positions_[id]);
	obj_positions_[id] = position();
}
```

Es wird dabei geprüft,
ob das Objekt noch nicht reserviert
oder bereits geschrieben wurde.
Ansonsten wird die aktuelle Position als Start-Position
gespeichert.

Nun können wir die Querverweis-Tabelle ausgeben.
Diese besteht aus einem Schlüsselwort und der Start-ID
gefolgt von der Anzahl der Einträge.
Da das 0-Objekt mit herausgeschrieben werden muss, ist die
Anzahl um eins größer als das zuletzt reservierte Objekt.

Die Querverweis-Tabelle enthält nun eine sortierte Liste aller
Objekte zusammen mit deren Position in der Datei.
Jeder Eintrag hat eine feste Länge.
Die Zahlen werden entsprechend mit `0`-Ziffern aufgefüllt.

Am Ende der Zeile ist ein Freizeichen!
Wenn man als Zeilentrenner den Windows-Standard verwendet,
werden zwei Byte für Newline und Carriage-Return verwendet.
Da unter fast allen anderen Betriebssystemen dafür nur ein
Newline reicht,
sorgt das zusätzliche Freizeichen dafür,
dass die Byte-Länge der Zeilen stimmt.

Nicht definierte Objekte werden als freie Einträge ausgegeben.
Zumindest das erste Objekt mit der ID `0` ist stets frei.

Es ergibt sich damit die folgende Implementierung
in der Datei `pdf.cpp`:

```c++
// ...
#include <iostream>
#include <iomanip>

void Pdf_Writer::write_xref() {
	out_ << "xref\n";
	out_ << "0 " << (last_obj_id_ + 1) << '\n';
	for (int i = 0; i <= last_obj_id_; ++i) {
		auto pos { obj_positions_[i] };
		if (pos) {
			out_ << std::setfill('0') << std::setw(10) <<
				pos << " 00000 n \n";
		} else {
			out_ << "0000000000 65535 f \n";
			if (i > 0) {
				std::cerr << "no object for id " <<
					i << "\n";
			}
		}
	}
}
```

Bleibt _nur_ noch die Ausgabe des Verzeichnisses,
um die Generierung des Trailers abzuschließen.

Dazu ist wieder etwas mehr Aufwand notwendig.
Verzeichnisse werden auch in Objekten oft ausgegeben.
Daher wird das Eröffnen und Abschließen in eine
Klasse `Map_Writer` gesteckt.
Im Konstruktor wird ein Verzeichnis geöffnet und im Destruktor
abgeschlossen.
So kann mit lokalen Objekten komfortabel verhindert werden,
dass Objekte nicht korrekt beendet werden.

Die Form der Generatoren gibt es jedoch nicht nur für
Verzeichnisse.
Auch Objekte und Listen können so geschrieben werden.
Daher wird der gemeinsame Code in eine Basis-Klasse
`Sub_Writer` gesteckt.

Aber bevor dieser implementiert werden kann,
muss zuerst einmal eine Möglichkeit bestehen,
von außen in den `Pdf_Writer` zu schreiben.
Dies erreichen wir durch die Implementierung des `<<`
Operators in `pdf.h`:

```c++
// ...
class Pdf_Writer {
	// ...
public:
	Pdf_Writer &operator<<(const std::string &str);
	Pdf_Writer &operator<<(int value);
	Pdf_Writer &operator<<(char ch);
	// ...
};
// ...
inline Pdf_Writer &Pdf_Writer::operator<<(const std::string &str) {
	write(str);
	return *this;
}

inline Pdf_Writer &Pdf_Writer::operator<<(int value) {
	write(std::to_string(value));
	return *this;
}

inline Pdf_Writer &Pdf_Writer::operator<<(char ch) {
	out_ << ch;
	++position_;
	return *this;
}
```

Damit kann der `Pdf_Writer` ähnlich wie ein `std::ostream`
verwendet werden.
Wir beschränken uns jedoch auf die Typen,
die wirklich benötigt werden:

* Zeichenketten,
* Zahlen und
* Zeichen.

Wichtig ist dabei,
dass wir entweder `write` aufrufen,
oder genau wissen,
wie viele Zeichen ausgegeben werden.
Nur so kann `position_` vernünftig aktualisiert werden.

Der `Sub_Writer` merkt sich den `Pdf_Writer` und zusätzlich
noch ein Präfix,
das zu Beginn jeder Zeile ausgegeben wird.
Es wird auch in `pdf.h` definiert:

```c++
// ...
class Sub_Writer {
	Pdf_Writer *root_;
	std::string prefix_;
public:
	[[nodiscard]] Pdf_Writer *root() const { return root_; }
	explicit Sub_Writer(Pdf_Writer *root, std::string prefix);
	template<typename T> Pdf_Writer &operator<<(T any);
};

inline Sub_Writer::Sub_Writer(
	Pdf_Writer *root, std::string prefix
):
	root_ { root }, prefix_ { std::move( prefix ) }
{ }

template<typename T> inline
	Pdf_Writer &Sub_Writer::operator<<(T any) {
		return *root_ << prefix_ << any;
	}
```

Damit kann der `Map_Writer` implementiert werden.
Ein Verzeichnis beginnt mit `<<` und endet mit `>>`.
Zusätzlich werden die Zeilen etwas eingerückt.

Die Klasse kann in `pdf.h` deklariert werden:

```c++
// ...
class Map_Writer: public Sub_Writer {
public:
	explicit Map_Writer(Pdf_Writer *root);
	~Map_Writer();
};
```

Die Implementierung der Methoden erfolgt in `pdf.cpp`:

```c++
// ...
Map_Writer::Map_Writer(Pdf_Writer *root):
	Sub_Writer { root, "    " }
{
	*root << "  <<\n";
}

Map_Writer::~Map_Writer() {
	*root() << "  >>\n";
}
```

Damit kann endlich das Verzeichnis geschrieben werden.
Es enthält nur zwei Einträge:

* eine Referenz auf das Haupt-Objekt (das wir noch definieren müssen) und
* die Anzahl der Elemente in der Querverweis-Tabelle.

Im `Pdf_Writer` in `pdf.h` gibt es ein neues Attribut mit dem
Haupt-Objekt:

```c++
// ...
class Pdf_Writer {
	int root_id_ { 0 };
	// ...
public:
	// ...
};
// ...
```

Damit kann die Methode `write_trailer_dict` in `pdf.cpp`
geschrieben werden:

```c++
// ...
void Pdf_Writer::write_trailer_dict() {
	Map_Writer trailer { this };
	trailer << "/Size " << (last_obj_id_ + 1) << '\n';
	trailer << "/Root " << root_id_ << " 0 R\n";
}
```

Die Einträge in einem Verzeichnis bestehen immer aus einem
Schlüssel, der mit einem Slash `/` beginnt und einem Wert.
Wenn der Wert ein Verweis auf ein Objekt ist,
so besteht er aus drei Teilen:

* der Objekt-ID,
* der Objekt-Version und
* dem Token `R`.

Aktuell würden wir folgende Ausgabe produzieren:

```
%PDF-1.4
xref
0 1
0000000000 65535 f 
trailer
  <<
    /Size 1
    /Root 0 0 R
  >>
startxref
9
%%EOF
```

Das wär schon fast ein gültiges PDF-Dokument.
Es hat _nur_ das Problem,
dass die Wurzel auf ein gelöschtes Objekt verweist
(das dazu noch die ungültige ID 0 hat).
