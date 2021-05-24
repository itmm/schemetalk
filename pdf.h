#pragma once

#include <ostream>
#include <map>
#include <list>
#include <vector>

class Pdf_Writer {
	std::ostream &out_;
	int position_ { 0 };
	int next_obj_ { 1 };
	std::map<int, int> obj_poss_;
	int obj_id_ { -1 };
	int in_dict_ { 0 };
	int root_id_ { -1 };
	int stream_start_ { -1 };
	int stream_length_id_ { -1 };

	void write_line(const std::string &line);

	void open_dict_();
	void close_dict_();
	void open_plain_obj_(int id);
public:
	explicit Pdf_Writer(std::ostream &out);
	~Pdf_Writer();

	[[nodiscard]] int reserve_obj() { return next_obj_++; }

	void int_entry(const std::string &name, int value);
	void obj_entry(const std::string &name, int id);
	void obj_list_entry(const std::string &name, const std::vector<int> &ids);
	void int_list_entry(const std::string &name, const std::vector<int> &ints);
	void tok_entry(const std::string &name, const std::string &tok);
	void open_dict_entry(const std::string &name);

	void write_log(const std::string &line) {
		write_line("    (" + line + ") '");
	}

	void open_obj(int id);
	void close_obj();
	void open_stream();
};