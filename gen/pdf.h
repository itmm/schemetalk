#pragma once
#include <map>
#include <ostream>

class Pdf_Writer {
	int root_id_ { 0 };
	std::map<int, int> obj_positions_;
	int last_obj_id_ { 0 };
	void write_xref();
	void write_trailer_dict();
	std::ostream &out_;
	int position_ { 0 };
	void write(const std::string &str);
	void write_header();
	void write_trailer();
public:
	Pdf_Writer &operator<<(const std::string &str);
	Pdf_Writer &operator<<(int value);
	Pdf_Writer &operator<<(char ch);
	void define_obj(int id);
	int reserve_obj_id() {
		return ++last_obj_id_;
	}
	[[nodiscard]] auto position() const {
		return position_;
	}
	explicit Pdf_Writer(std::ostream &out);
	~Pdf_Writer();
};
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
class Map_Writer: public Sub_Writer {
public:
	explicit Map_Writer(Pdf_Writer *root);
	~Map_Writer();
};
