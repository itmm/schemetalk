#pragma once

#include <ostream>
#include <map>
#include <list>
#include <vector>
#include <cassert>
#include "node.h"

class Map_Writer;
using Map_Writer_Ptr = std::unique_ptr<Map_Writer>;

class List_Writer;
using List_Writer_Ptr = std::unique_ptr<List_Writer>;

class Pdf_Writer;

class Map_Writer {
	Pdf_Writer *root_;
public:
	explicit Map_Writer(Pdf_Writer *root);
	~Map_Writer();

	void int_entry(const std::string &name, int value);
	void obj_entry(const std::string &name, int id);
	void tok_entry(const std::string &name, const std::string &tok);
	Map_Writer_Ptr open_dict_entry(const std::string &name);
	List_Writer_Ptr open_list_entry(const std::string &name);

};

class List_Writer {
	Pdf_Writer *root_;
public:
	explicit List_Writer(Pdf_Writer *root);
	~List_Writer();

	void int_entry(int value);
	void obj_entry(int id);
};

class Obj_Writer {
	Pdf_Writer *root_;
	std::ostream::pos_type stream_start_ { -1 };
	int stream_length_id_ { -1 };
public:
	explicit Obj_Writer(int id, Pdf_Writer *root);
	~Obj_Writer();

	Map_Writer_Ptr open_dict();
	void open_stream(Map_Writer_Ptr map);
};

using Obj_Writer_Ptr = std::unique_ptr<Obj_Writer>;

class Pdf_Writer {
	std::ostream &out_;
	int next_obj_ { 1 };
	std::map<int, std::ostream::pos_type> obj_poss_;
	int root_id_ { -1 };
	Obj_Writer_Ptr content_;

	Map_Writer_Ptr open_dict_();
	void write_header();
	void write_trailer();
	void write_xref();
	void write_trailer_dict();
public:
	explicit Pdf_Writer(std::ostream &out);
	~Pdf_Writer();

	[[nodiscard]] auto position() const { return out_.tellp(); }

	template<typename T>
	std::ostream &operator<<(T any) { return out_ << any; }

	[[nodiscard]] int reserve_obj() { return next_obj_++; }

	void write_log(const std::string &line);

	Obj_Writer_Ptr open_obj(int id);

};

void add_pdf_commands(const Node_Ptr &state);
