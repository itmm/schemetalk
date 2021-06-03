#pragma once

#include <ostream>
#include <map>
#include <list>
#include <utility>
#include <vector>
#include <cassert>
#include "node.h"

class Pdf_Writer;

class Sub_Writer {
	Pdf_Writer *root_;
	std::string prefix_;
public:
	[[nodiscard]] Pdf_Writer *root() const { return root_; }
	explicit Sub_Writer(Pdf_Writer *root, std::string prefix);
	template<typename T> std::ostream &operator<<(T any);
};

class Map_Writer: public Sub_Writer {
public:
	explicit Map_Writer(Pdf_Writer *root);
	~Map_Writer();
};

class List_Writer: public Sub_Writer {
public:
	explicit List_Writer(Pdf_Writer *root);
	~List_Writer();
};

class Obj_Writer: public Sub_Writer {
	std::ostream::pos_type stream_start_ { -1 };
	int stream_length_id_ { -1 };
public:
	explicit Obj_Writer(int id, Pdf_Writer *root);
	~Obj_Writer();

	void open_stream(std::unique_ptr<Map_Writer> map);
};

class Pdf_Writer {
	std::ostream &out_;
	int last_obj_id_ { 0 };
	std::map<int, std::ostream::pos_type> obj_positions_;
	int root_id_ { -1 };
	std::unique_ptr<Obj_Writer> content_;

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

	[[nodiscard]] int reserve_obj_id() { return ++last_obj_id_; }
	void define_obj(int id);

	void write_log(const std::string &line);
};

void add_pdf_commands(const Node_Ptr &state);

inline Sub_Writer::Sub_Writer(Pdf_Writer *root, std::string prefix):
	root_ { root }, prefix_ { std::move( prefix ) }
{ }

template<typename T> inline std::ostream &Sub_Writer::operator<<(T any) {
	return *root_ << prefix_ << any;
}