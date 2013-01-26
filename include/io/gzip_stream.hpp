
#ifndef GZIPSTREAM_H_INCLUDED
#define GZIPSTREAM_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <zlib.h>

namespace io {

// read gz files
template < typename Char = char, typename Traits = std::char_traits<Char> >
class gzip_istream_buffer : public std::basic_streambuf<Char, Traits> {
public:
    typedef typename std::basic_streambuf<Char, Traits>::int_type  int_type;
    typedef typename std::basic_streambuf<Char, Traits>::char_type char_type;
    typedef typename std::basic_streambuf<Char, Traits> BaseT;

    gzip_istream_buffer(const std::string& fname) {
		this->file = gzopen(fname.c_str(), "r");
		if (this->file == 0) {
			throw std::runtime_error("Unable to open the gzip file '" + fname + "' for reading.");
		}

	    setg((Char*)0, (Char*)0, (Char*)0);
    }

    ~gzip_istream_buffer() {
		gzclose(this->file);
    }
private:
    typedef typename std::vector<Char> BufferT;

	gzFile  file;
    BufferT input;

    int_type underflow() {
        if (BaseT::gptr() != BaseT::egptr()) {
            return Traits::to_int_type(*BaseT::gptr());
		} else if (gzeof(this->file)) {
			return Traits::eof();
		}

        input.clear();
        input.resize(2048);

        int rret = 0;

		rret = gzread(this->file, &(*(input.begin())), input.size() * sizeof(Char));
		input.resize(rret);

		if (input.size() == 0) {
			return Traits::eof();
		} else {
			setg(&(*(input.begin())), &(*(input.begin())), &(*(input.begin() + input.size())));
			return Traits::to_int_type(*(input.begin()));
		}
    }

	gzip_istream_buffer();
	gzip_istream_buffer(const gzip_istream_buffer<Char, Traits>& rhs);
	void operator=(const gzip_istream_buffer<Char, Traits>& rhs);
};

template < typename Char = char, typename Traits = std::char_traits<Char> >
class gzip_istream : public std::basic_istream<Char, Traits> {
public:
    gzip_istream(const std::string& fname) : std::basic_istream<Char, Traits>(&buffer), buffer(fname) {
    }
private:
    typedef gzip_istream_buffer<Char, Traits> Buffer;
    Buffer buffer;

    gzip_istream();
    gzip_istream(const gzip_istream<Char, Traits>& rhs);
    void operator=(const gzip_istream<Char, Traits>& rhs);
};

// write gz files
template < typename Char = char, typename Traits = std::char_traits<Char> >
class gzip_ostream_buffer : public std::basic_streambuf<Char, Traits> {
public:
    typedef typename std::basic_streambuf<Char, Traits>::int_type  int_type;
    typedef typename std::basic_streambuf<Char, Traits>::char_type char_type;
    typedef typename std::basic_streambuf<Char, Traits> BaseT;

    gzip_ostream_buffer(const std::string& fname) {
		this->file = gzopen(fname.c_str(), "w");
		if (this->file == 0) {
			throw std::runtime_error("Unable to open the gzip file '" + fname + "' for writing.");
		}
    }

    ~gzip_ostream_buffer() {
		sync();
		gzclose(this->file);
    }

	unsigned int max_buffer_size() const {
		return 4 * 1024; // 4k
	}
private:
	typedef typename std::vector<Char> BufferT;

	gzFile  file;
	BufferT output;

	int_type overflow(int_type c) {
		if (c == Traits::eof()) {
			sync();
		} else {
			this->output.push_back(Traits::to_char_type(c));
			if (this->output.size() >= max_buffer_size()) {
				sync();
			}
		}

		return Traits::not_eof(c);
	}

	int_type sync() {
		if (this->output.size() > 0) {
			if (gzwrite(this->file, &(*(this->output.begin())), this->output.size()) == 0) {
				throw std::runtime_error("Failed to write buffer to gzip file.");
			}
	
			this->output.clear();
		}

		return 0;
	}

	gzip_ostream_buffer();
	gzip_ostream_buffer(const gzip_ostream_buffer<Char, Traits>& rhs);
	void operator=(const gzip_ostream_buffer<Char, Traits>& rhs);
};

template < typename Char = char, typename Traits = std::char_traits<Char> >
class gzip_ostream : public std::basic_ostream<Char, Traits> {
public:
    gzip_ostream(const std::string& fname) : std::basic_ostream<Char, Traits>(&buffer), buffer(fname) {
    }
private:
    typedef gzip_ostream_buffer<Char, Traits> Buffer;
    Buffer buffer;

    gzip_ostream();
    gzip_ostream(const gzip_ostream<Char, Traits>& rhs);
    void operator=(const gzip_ostream<Char, Traits>& rhs);
};

}

#endif 
