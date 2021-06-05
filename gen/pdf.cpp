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
void Pdf_Writer::write(int value, int min_size) {
	std::string vs { std::to_string(value) };
	while (vs.size() < min_size) { vs = '0' + vs; }
	write(vs);
}

void Pdf_Writer::write_trailer() {
	int xref { position() };
	write_xref();
	write("trailer\n");
	write_trailer_dict();
	write("startxref\n");
	write(xref);
	write("\n%%EOF\n");
}
#include <cassert>

void Pdf_Writer::define_obj(int id) {
	assert(id <= last_obj_id_);
	assert(! obj_positions_[id]);
	obj_positions_[id] = position();
}
#include <iostream>

void Pdf_Writer::write_xref() {
	write("xref\n");
	write("0 "); write(last_obj_id_ + 1); write("\n");
	for (int i = 0; i <= last_obj_id_; ++i) {
		auto pos { obj_positions_[i] };
		if (pos) {
			write(pos, 10); write(" 00000 n \n");
		} else {
			write("0000000000 65535 f \n");
			if (i > 0) {
				std::cerr << "no object for id " <<
					i << "\n";
			}
		}
	}
}
Map_Writer::Map_Writer(Pdf_Writer *root):
	Sub_Writer { root, "    " }
{
	*root << "  <<\n";
}

Map_Writer::~Map_Writer() {
	*root() << "  >>\n";
}
void Pdf_Writer::write_trailer_dict() {
	Map_Writer trailer { this };
	trailer << "/Size " << (last_obj_id_ + 1) << "\n";
	trailer << "/Root " << root_id_ << " 0 R\n";
}
