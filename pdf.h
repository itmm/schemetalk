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

class Pdf_Writer {
	std::ostream &out_;
	int position_ { 0 };
	int next_obj_ { 1 };
	std::map<int, int> obj_poss_;
	int obj_id_ { -1 };
	int root_id_ { -1 };
	int stream_start_ { -1 };
	int stream_length_id_ { -1 };

	Map_Writer_Ptr open_dict_();
	void open_plain_obj_(int id);
public:
	explicit Pdf_Writer(std::ostream &out);
	~Pdf_Writer();

	void append_to_line(const std::string &line);
	void write_line(const std::string &line);

	[[nodiscard]] int reserve_obj() { return next_obj_++; }

	void write_log(const std::string &line);

	Map_Writer_Ptr open_obj(int id);
	void close_obj();
	void open_stream(Map_Writer_Ptr map);
};

void add_pdf_commands(const Node_Ptr &state);
