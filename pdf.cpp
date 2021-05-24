#include "pdf.h"

#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "cmd.h"
#include "err.h"
#include "eval.h"
#include "invocation.h"
#include "map.h"
#include "print.h"
#include "token.h"

void Pdf_Writer::write_line(const std::string &line) {
	position_ += static_cast<int>(line.length()) + 1;
	out_ << line << '\n';
}

void Pdf_Writer::open_dict_() {
	assert(in_dict_ <= 0);
	write_line("  <<");
	++in_dict_;
}

void Pdf_Writer::close_dict_() {
	assert(in_dict_ > 0);
	write_line("  >>");
	--in_dict_;
}

void Pdf_Writer::open_plain_obj_(int id) {
	assert(obj_id_ == -1);
	assert(in_dict_ <= 0);
	assert(! obj_poss_[id]);
	obj_poss_[id] = position_;
	obj_id_ = id;
	std::ostringstream line;
	line << id << " 0 obj";
	write_line(line.str());
}

void Pdf_Writer::open_obj(int id) {
	open_plain_obj_(id);
	open_dict_();
}

void Pdf_Writer::close_obj() {
	assert(obj_id_ > 0);
	if (in_dict_ > 0) { close_dict_(); }
	if (in_dict_ > 0) { err("too many open dicts in object"); }
	int stream_length = position_ - stream_start_;
	if (stream_start_ > 0) {
		write_line("endstream");
		stream_start_ = -1;
	}
	write_line("endobj");
	obj_id_ = -1;
	if (stream_length_id_ > 0) {
		int id { stream_length_id_ };
		stream_length_id_ = -1;
		open_plain_obj_(id);
		std::ostringstream line;
		line << "    " << stream_length;
		write_line(line.str());
		close_obj();
	}
}

void Pdf_Writer::int_entry(const std::string &name, int value) {
	assert(in_dict_ > 0);
	assert(! name.empty() && name[0] == '/');
	std::ostringstream line;
	line << "    " << name << ' ' << value;
	write_line(line.str());
}

void Pdf_Writer::tok_entry(const std::string &name, const std::string &tok) {
	assert(in_dict_ > 0);
	assert(! name.empty() && name[0] == '/');
	assert(! tok.empty() && tok[0] == '/');
	std::ostringstream line;
	line << "    " << name << ' ' << tok;
	write_line(line.str());
}

void Pdf_Writer::obj_entry(const std::string &name, int id) {
	assert(in_dict_ > 0);
	assert(! name.empty() && name[0] == '/');
	assert(id > 0 && id < next_obj_);
	std::ostringstream line;
	line << "    " << name << ' ' << id << " 0 R";
	write_line(line.str());
}

void Pdf_Writer::obj_list_entry(const std::string &name, const std::vector<int> &ids) {
	assert(in_dict_ > 0);
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
	assert(in_dict_ > 0);
	assert(! name.empty() && name[0] == '/');
	std::ostringstream line;
	line << "    " << name << " [";
	for (const int &i : ints) {
		line << ' ' << i ;
	}
	line << " ]";
	write_line(line.str());
}

void Pdf_Writer::open_dict_entry(const std::string &name) {
	assert(in_dict_ > 0);
	assert(! name.empty() && name[0] == '/');
	write_line(name + " <<");
	++in_dict_;
}

void Pdf_Writer::open_stream() {
	assert(obj_id_ > 0);
	assert(in_dict_ == 1);
	assert(stream_length_id_ < 0);
	assert(stream_start_ < 0);
	stream_length_id_ = reserve_obj();
	obj_entry("/Length", stream_length_id_);
	close_dict_();
	write_line("stream");
	stream_start_ = position_;
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
	open_dict_entry("/Resources");
	open_dict_entry("/Font");
	int font_id = reserve_obj();
	obj_entry("/F0", font_id);
	close_dict_();
	close_dict_();
	int content_id = reserve_obj();
	obj_entry("/Contents", content_id);
	close_obj();
	open_obj(font_id);
	tok_entry("/Type", "/Font");
	tok_entry("/Subtype", "/Type1");
	tok_entry("/BaseFont", "/TimesRoman");
	close_obj();
	open_obj(content_id);
	open_stream();
	write_line("BT");
	write_line("    /F0 12 Tf");
	write_line("    72 720 Td");
	write_line("    14 TL");
}

Pdf_Writer::~Pdf_Writer() {
	write_line("ET");
	close_obj();
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

void Pdf_Writer::write_log(const std::string &line) {
	std::ostringstream out;
	out << "    (";
	for (char c : line) {
		switch (c) {
		case '(': case ')': case '\\':
			out << '\\'; // fallthrough
		default:
			out << c;
		}
	}
	out << ") '";
	write_line(out.str());
}

class Pdf_Cmd: public Command {
public:
	[[nodiscard]] Node_Ptr eval(Node_Ptr invocation, Node_Ptr state) const override;
};

static Invocation::Iter eat_space(Invocation::Iter it, Invocation::Iter end) {
	while (it != end && (**it).as_space()) { ++it; }
	return it;
}

Node_Ptr Pdf_Cmd::eval(Node_Ptr invocation, Node_Ptr state) const {
	auto it { invocation->as_invocation()->begin() };
	auto end { invocation->as_invocation()->end() };
	it = eat_space(it, end);
	if (it != end) { ++it; }
	it = eat_space(it, end);
	if (it == end || ! (**it).as_token() || (**it).as_token()->token() != "as:") {
		err("pdf: expected as: parameter");
	}
	it = eat_space(++it, end);
	if (it == end) { err("no as: value"); }
	Node_Ptr path { ::eval(*it++, state) };
	if (! path || ! path->as_token()) {
		err("no as: token value");
	}

	std::ofstream out(path->as_token()->token().c_str());
	Pdf_Writer writer(out);

	it = eat_space(it, end);
	if (it == end || ! (**it).as_token() || (**it).as_token()->token() != "with:") {
		err("pdf: expected with: parameter");
	}
	it = eat_space(++it, end);

	Node_Ptr value;

	while (it != end) {
		std::ostringstream line;
		for (; it != end && *it && ! (**it).as_space(); ++it) {
			value = *it;
			if (value->as_invocation()) {
				value = ::eval(value, state);
				if (! value || value->as_space()) {
					if (! line.str().empty()) {
						writer.write_log(line.str());
					}
					++it; break;
				}
			}
			line << value;
		}
		if (! line.str().empty()) {
			writer.write_log(line.str());
		}
		it = eat_space(it, end);
	}
	return value;
}

void add_pdf_commands(const Node_Ptr &state) {
	Map *s { state->as_map() };
	s->push(std::make_shared<Pdf_Cmd>(), "pdf");
}

