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
void Pdf_Writer::write_trailer() {
	int xref { position() };
	write_xref();
	out_ << "trailer\n";
	write_trailer_dict();
	out_ << "startxref\n";
	out_ << xref << '\n';
	out_ << "%%EOF\n";
}
#include <cassert>

void Pdf_Writer::define_obj(int id) {
	assert(id <= last_obj_id_);
	assert(! obj_positions_[id]);
	obj_positions_[id] = position();
}
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
	trailer << "/Size " << (last_obj_id_ + 1) << '\n';
	trailer << "/Root " << root_id_ << " 0 R\n";
}
