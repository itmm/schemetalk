#include <cassert>
#include <iostream>
#include <iomanip>

#include "pdf.h"

void Pdf_Writer::write_line(const std::string &line) {
	position_ += static_cast<int>(line.length()) + 1;
	out_ << line << '\n';
}

void Pdf_Writer::open_dict_() {
	assert(! in_dict_);
	write_line("  <<");
	in_dict_ = true;
}

void Pdf_Writer::close_dict_() {
	assert(in_dict_);
	write_line("  >>");
	in_dict_ = false;
}

void Pdf_Writer::open_obj(int id) {
	assert(obj_id_ == -1);
	assert(! in_dict_);
	assert(! obj_poss_[id]);
	obj_poss_[id] = position_;
	obj_id_ = id;
	std::ostringstream line;
	line << id << " 0 obj";
	write_line(line.str());
	open_dict_();
}

void Pdf_Writer::close_obj() {
	assert(obj_id_ > 0);
	if (in_dict_) { close_dict_(); }
	write_line("endobj");
	obj_id_ = -1;
}

void Pdf_Writer::int_entry(const std::string &name, int value) {
	assert(in_dict_);
	assert(! name.empty() && name[0] == '/');
	std::ostringstream line;
	line << "    " << name << ' ' << value;
	write_line(line.str());
}

void Pdf_Writer::tok_entry(const std::string &name, const std::string &tok) {
	assert(in_dict_);
	assert(! name.empty() && name[0] == '/');
	assert(! tok.empty() && tok[0] == '/');
	std::ostringstream line;
	line << "    " << name << ' ' << tok;
	write_line(line.str());
}

void Pdf_Writer::obj_entry(const std::string &name, int id) {
	assert(in_dict_);
	assert(! name.empty() && name[0] == '/');
	assert(id > 0 && id < next_obj_);
	std::ostringstream line;
	line << "    " << name << ' ' << id << " 0 R";
	write_line(line.str());
}

void Pdf_Writer::obj_list_entry(const std::string &name, const std::vector<int> &ids) {
	assert(in_dict_);
	assert(! name.empty() && name[0] == '/');
	std::ostringstream line;
	line << "    " << name << " [";
	for (const int &i : ids) {
		line << ' ' << i << " 0 R";
	}
	line << " ]";
	write_line(line.str());
}

void Pdf_Writer::int_list_entry(const std::string &name, const std::vector<int> &ints) {
	assert(in_dict_);
	assert(! name.empty() && name[0] == '/');
	std::ostringstream line;
	line << "    " << name << " [";
	for (const int &i : ints) {
		line << ' ' << i ;
	}
	line << " ]";
	write_line(line.str());
}

Pdf_Writer::Pdf_Writer(std::ostream &out): out_ { out } {
	write_line("%PDF-1.4");
	root_id_ = reserve_obj();
	open_obj(root_id_);
	tok_entry("/Type", "/Catalog");
	int pages = reserve_obj();
	obj_entry("/Pages", pages);
	close_obj();
	open_obj(pages);
	tok_entry("/Type", "/Pages");
	int_entry("/Count", 1);
	int page = reserve_obj();
	obj_list_entry("/Kids", { page });
	close_obj();
	open_obj(page);
	tok_entry("/Type", "/Page");
	obj_entry("/Parent", pages);
	int_list_entry("/MediaBox", { 0, 0, 612, 792 });
	close_obj();
}

Pdf_Writer::~Pdf_Writer() {
	assert(obj_id_ == -1);
	assert(! in_dict_);
	int xref { position_ };
	write_line("xref");
	{
		std::ostringstream line;
		line << "0 " << next_obj_;
		write_line(line.str());
	}
	write_line("0000000000 65535 f ");
	for (int i = 1; i < next_obj_; ++i) {
		int pos { obj_poss_[i] };
		if (pos) {
			std::ostringstream line;
			line << std::setfill('0') << std::setw(10) << pos << ' ';
			line << "00000 n ";
			write_line(line.str());
		} else { std::cerr << "no object for id " << i << "\n"; }
	}
	write_line("trailer");
	open_dict_();
	int_entry("/Size", next_obj_);
	if (root_id_ > 0) {
		obj_entry("/Root", root_id_);
	}
	close_dict_();
	write_line("startxref");
	{
		std::ostringstream line;
		line << xref;
		write_line(line.str());
	}
	write_line("%%EOF");
}