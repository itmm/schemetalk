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

Map_Writer::Map_Writer(Pdf_Writer *root): root_ { root } {
	*root_ << "\t<<\n";
}

Map_Writer::~Map_Writer() {
	*root_ << "\t>>\n";
}

List_Writer::List_Writer(Pdf_Writer *root): root_ { root } {
	*root_ << "[ ";
}

List_Writer::~List_Writer() {
	*root_ << " ]\n";
}

Map_Writer_Ptr Pdf_Writer::open_dict_() {
	return std::make_unique<Map_Writer>(this);
}

Obj_Writer::Obj_Writer(int id, Pdf_Writer *root): root_ { root } {
	*root_ << id << " 0 obj\n";
}

Obj_Writer_Ptr Pdf_Writer::open_obj(int id) {
	assert(! obj_poss_[id]);
	obj_poss_[id] = position();
	return std::make_unique<Obj_Writer>(id, this);
}

Obj_Writer::~Obj_Writer() {
	auto stream_length = root_->position() - stream_start_;
	if (stream_start_ > 0) {
		*root_ << "endstream\n";
		stream_start_ = -1;
	}
	*root_ << "endobj\n";
	if (stream_length_id_ > 0) {
		int id { stream_length_id_ };
		stream_length_id_ = -1;
		{
			Obj_Writer_Ptr obj { root_->open_obj(id) };
			*root_ << "    " << stream_length << '\n';
		}
	}
}

void Map_Writer::int_entry(const std::string &name, int value) {
	assert(! name.empty() && name[0] == '/');
	*root_ << "    " << name << ' ' << value << '\n';
}

void List_Writer::int_entry(int value) {
	*root_ << ' ' << value;
}

void Map_Writer::tok_entry(const std::string &name, const std::string &tok) {
	assert(! name.empty() && name[0] == '/');
	assert(! tok.empty() && tok[0] == '/');
	*root_ << "    " << name << ' ' << tok << '\n';
}

void Map_Writer::obj_entry(const std::string &name, int id) {
	assert(! name.empty() && name[0] == '/');
	*root_ << "    " << name << ' ' << id << " 0 R\n";
}

void List_Writer::obj_entry(int id) {
	*root_ << ' ' << id << " 0 R";
}

Map_Writer_Ptr Map_Writer::open_dict_entry(const std::string &name) {
	assert(! name.empty() && name[0] == '/');
	*root_ << name << ' ';
	return std::make_unique<Map_Writer>(root_);
}

List_Writer_Ptr Map_Writer::open_list_entry(const std::string &name) {
	assert(! name.empty() && name[0] == '/');
	*root_ << name << ' ';
	return std::make_unique<List_Writer>(root_);
}

Map_Writer_Ptr Obj_Writer::open_dict() {
	return std::make_unique<Map_Writer>(root_);
}

void Obj_Writer::open_stream(Map_Writer_Ptr map) {
	assert(stream_length_id_ < 0);
	assert(stream_start_ < 0);
	stream_length_id_ = root_->reserve_obj();
	map->obj_entry("/Length", stream_length_id_);
	map.reset();
	*root_ << "stream\n";
	stream_start_ = root_->position();
}

void Pdf_Writer::write_header() {
	out_ << "%PDF-1.4\n";
}

Pdf_Writer::Pdf_Writer(std::ostream &out): out_ { out } {
	write_header();
	root_id_ = reserve_obj();
	int pages = reserve_obj();
	{
		Obj_Writer_Ptr obj = open_obj(root_id_);
		{
			Map_Writer_Ptr map = obj->open_dict();
			map->tok_entry("/Type", "/Catalog");
			map->obj_entry("/Pages", pages);
		}
	}
	int page = reserve_obj();
	{
		Obj_Writer_Ptr obj = open_obj(pages);
		{
			Map_Writer_Ptr map = obj->open_dict();
			map->tok_entry("/Type", "/Pages");
			map->int_entry("/Count", 1);
			{
				List_Writer_Ptr lst { map->open_list_entry("/Kids") };
				lst->obj_entry(page);
			}
		}
	}
	int font_id = reserve_obj();
	int content_id = reserve_obj();
	{
		Obj_Writer_Ptr obj = open_obj(page);
		{
			Map_Writer_Ptr map = obj->open_dict();
			map->tok_entry("/Type", "/Page");
			map->obj_entry("/Parent", pages);
			{
				List_Writer_Ptr lst { map->open_list_entry("/MediaBox") };
				lst->int_entry(0);
				lst->int_entry(0);
				lst->int_entry(612);
				lst->int_entry(792);
			}
			{
				Map_Writer_Ptr resources = map->open_dict_entry("/Resources");
				{
					Map_Writer_Ptr font = resources->open_dict_entry("/Font");
					font->obj_entry("/F0", font_id);
				}
			}
			map->obj_entry("/Contents", content_id);
		}
	}
	{
		Obj_Writer_Ptr obj = open_obj(font_id);
		{
			Map_Writer_Ptr map = obj->open_dict();
			map->tok_entry("/Type", "/Font");
			map->tok_entry("/Subtype", "/Type1");
			map->tok_entry("/BaseFont", "/TimesRoman");
		}
	}

	{
		content_ = open_obj(content_id);
		Map_Writer_Ptr map { content_->open_dict() };
		content_->open_stream(std::move(map));
	}
	out_ << "BT\n";
	out_ << "    /F0 12 Tf\n";
	out_ << "    72 720 Td\n";
	out_ << "    14 TL\n";
}

Pdf_Writer::~Pdf_Writer() {
	out_ << "ET\n";
	content_.reset();
	write_trailer();
}

void Pdf_Writer::write_xref() {
	out_ << "xref\n";
	out_ << "0 " << next_obj_ << '\n';
	out_ << "0000000000 65535 f \n";
	for (int i = 1; i < next_obj_; ++i) {
		auto pos { obj_poss_[i] };
		if (pos) {
			out_ << std::setfill('0') << std::setw(10) << pos << ' ';
			out_ << "00000 n \n";
		} else { std::cerr << "no object for id " << i << "\n"; }
	}
}

void Pdf_Writer::write_trailer_dict() {
	Map_Writer_Ptr trailer = open_dict_();
	trailer->int_entry("/Size", next_obj_);
	if (root_id_ > 0) {
		trailer->obj_entry("/Root", root_id_);
	}
}

void Pdf_Writer::write_trailer() {
	auto xref { position() };
	write_xref();
	out_ << "trailer\n";
	write_trailer_dict();
	out_ << "startxref\n";
	out_ << xref << '\n';
	out_ << "%%EOF\n";
}

void Pdf_Writer::write_log(const std::string &line) {
	out_ << "    (";
	for (char c : line) {
		switch (c) {
		case '(': case ')': case '\\':
			out_ << '\\'; // fallthrough
		default:
			out_ << c;
		}
	}
	out_ << ") '\n";
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

