
#include <mc/value.hpp>
#include <str/Util.hpp>
#include <stdexcept>

namespace mc {

// br :: Byte<N> -> Byte<N>
//   byte-reverse
template <typename T>
	struct b {
		static T r(T x) {
			unsigned char* px = (unsigned char*)&x;
			static const unsigned int n = sizeof(T) / 2;
			static const unsigned int e = sizeof(T) - 1;
	
			for (unsigned int i = 0; i < n; ++i) {
				unsigned char b = px[i];
				px[i]     = px[e - i];
				px[e - i] = b;
			}
			return x;
		}
	};

template <typename T>
	struct b< std::vector<T> > {
		static std::vector<T> r(const std::vector<T>& xs) {
			std::vector<T> result;
			for (typename std::vector<T>::const_iterator x = xs.begin(); x != xs.end(); ++x) {
				result.push_back(b<T>::r(*x));
			}
			return result;
		}
	};

template <typename T>
	T br(const T& x) {
		return b<T>::r(x);
	}

// showNyb :: Char/2 -> Char
inline char showNyb(unsigned char x) {
	static const char cs[] = "0123456789abcdef";
	if (x < sizeof(cs)) {
		return cs[x];
	} else {
		return '?';
	}
}

// showByte :: Char -> String
inline std::string showByte(unsigned char x) {
	const char sx[] = { showNyb(x >> 4),  showNyb(x & 0x0f), 0 };
	return sx;
}

// put :: Pack a => Ostream -> a -> ()
#define DECL_PUT(X) \
	inline void put(std::ostream& out, X x) { \
		X nx = br<X>(x); \
		out.write((const char*)(&nx), sizeof(X)); \
	}

DECL_PUT(unsigned char);
DECL_PUT(short);
DECL_PUT(int);
DECL_PUT(long);
DECL_PUT(float);
DECL_PUT(double);

inline void put(std::ostream& out, const char* x, short n) {
	put(out, n);
	out.write(x, n);
}

inline void put(std::ostream& out, const std::string& x) {
	put(out, x.c_str(), short(x.size()));
}

inline void put(std::ostream& out, const std::vector<unsigned char>& xs) {
	put(out, int(xs.size()));
	if (xs.size() > 0) {
		out.write((const char*)&(*(xs.begin())), xs.size());
	}
}

inline void put(std::ostream& out, const std::vector<int>& xs) {
	std::vector<int> rxs = br(xs);
	put(out, int(rxs.size()));
	if (rxs.size() > 0) {
		out.write((const char*)&(*(rxs.begin())), sizeof(int) * rxs.size());
	}
}

inline void put(std::ostream& out, value* v) {
	put(out, v->tag());
	put(out, v->name());
	v->write(out);
}

inline void put(std::ostream& out, const array::data& x) {
	put(out, x.first);
	put(out, int(x.second.size()));

	unsigned int n = x.second.size();
	for (unsigned int i = 0; i < n; ++i) {
		x.second[i]->write(out);
	}
}

inline void put(std::ostream& out, const tuple::data& x) {
	unsigned int n = x.size();
	for (unsigned int i = 0; i < n; ++i) {
		put(out, x[i]);
	}
	put(out, (unsigned char)0);
}

// get :: Pack a => Istream -> a
#define DECL_GET(T) \
	void get(std::istream& in, T& x) { \
		in.read((char*)&x, sizeof(T)); \
		x = br(x); \
	}

DECL_GET(unsigned char);
DECL_GET(short);
DECL_GET(int);
DECL_GET(long);
DECL_GET(float);
DECL_GET(double);

void get(std::istream& in, std::string& x) {
	short len = 0;
	get(in, len);

	x.resize(len);
	if (len > 0) {
		in.read(&(*(x.begin())), len);
	}
}

void get(std::istream& in, std::vector<unsigned char>& xs) {
	int len = 0;
	get(in, len);

	xs.resize(len);
	if (len > 0) {
		in.read((char*)&(*(xs.begin())), len);
	}
}

void get(std::istream& in, std::vector<int>& xs) {
	int len = 0;
	get(in, len);

	xs.resize(len);
	if (len > 0) {
		in.read((char*)&(*(xs.begin())), sizeof(int) * len);
		xs = br(xs);
	}
}

value* make(tagid tid, const char* name, std::istream& in) {
#	define CC(T) if (tid == T::tagID()) { return (name == 0) ? new T(in) : new T(name, in); }

	CC(byte);
	CC(int2);
	CC(int4);
	CC(int8);
	CC(float4);
	CC(float8);
	CC(bytes);
	CC(string);
	CC(array);
	CC(tuple);
	CC(int4s);

	throw std::runtime_error("Failed to decode value -- invalid tag #" + str::to_string(tid));
}

void get(std::istream& in, value*& x) {
	tagid tid = 0;
	get(in, tid);
	x = make(tid, 0, in);
}

void get(std::istream& in, hvalues& xs) {
	tagid tid = 0;
	get(in, tid);
	
	int len = 0;
	get(in, len);
	
	xs.first = tid;
	xs.second.clear();
	for (int i = 0; i < len; ++i) {
		xs.second.push_back(make(tid, "", in));
	}
}

void get(std::istream& in, values& xs) {
	xs.clear();
	while (true) {
		tagid tid = 0;
		get(in, tid);
		if (tid == 0) {
			break;
		}

		xs.push_back(make(tid, 0, in));
	}
}

template <typename T>
	T get(std::istream& in) {
		T result = T();
		get(in, result);
		return result;
	}

// value boxes
value::value(const char* vname) : vname(vname) { }
value::value(std::istream& in)  : vname(get<std::string>(in)) { }
value::~value() { }
const char* value::name() const { return this->vname.c_str(); }

#define DECL_BOX(T, ID) \
	T :: T(const char* vname, T::data v) : value(vname), x(v) { } \
	T :: T(std::istream& in) : value(in) { this->x = get<T::data>(in); } \
	T :: T(const char* vname, std::istream& in) : value(vname) { this->x = get<T::data>(in); } \
	const T :: data& T :: unbox() const { return this->x; } \
	T :: data& T :: unbox() { return this->x; } \
	tagid T :: tag() const { return ID; } \
	tagid T :: tagID()     { return ID; } \
	void T :: write(std::ostream& out) const { put(out, this->x); }

#define DECL_BOX_FMT(T, ID, FMTE) \
	DECL_BOX(T, ID); \
	void T :: show(std::ostream& out) const { out << FMTE; }

DECL_BOX_FMT(byte,   1, "0x" << showByte(x));
DECL_BOX_FMT(int2,   2, x << "S");
DECL_BOX_FMT(int4,   3, x);
DECL_BOX_FMT(int8,   4, x << "L");
DECL_BOX_FMT(float4, 5, x << "F");
DECL_BOX_FMT(float8, 6, x);
DECL_BOX    (bytes,  7);
DECL_BOX_FMT(string, 8, "\"" << x << "\"");
DECL_BOX    (array,  9);
DECL_BOX    (tuple,  10);
DECL_BOX    (int4s,  11);

// byte arrays
void bytes::show(std::ostream& out) const {
	out << "<" << this->x.size() << "-byte array>";
	return;

	out << "0X";
	for (data::const_iterator b = this->x.begin(); b != this->x.end(); ++b) {
		out << showByte(*b);
	}
}

// int arrays
void int4s::show(std::ostream& out) const {
	out << "|";
	if (this->x.size() > 0) {
		out << this->x[0];
		for (int i = 1; i < this->x.size(); ++i) {
			out << ";";
			out << this->x[i];
		}
	}
	out << "|";
}

// value sequence of homogeneous type
void array::show(std::ostream& out) const {
	out << "[ty=" << int(this->x.first) << ";";
	if (this->x.second.size() > 0) {
		mc::show((value*)this->x.second[0], out);
		for (int i = 1; i < this->x.second.size(); ++i) {
			out << ", ";
			mc::show((value*)this->x.second[i], out);
		}
	}
	out << "]";
}

// value sequence of heterogeneous type
void tuple::show(std::ostream& out) const {
	out << "(";
	if (this->x.size() > 0) {
		mc::show((value*)this->x[0], out);
		for (int i = 1; i < this->x.size(); ++i) {
			out << ", ";
			mc::show((value*)this->x[i], out);
		}
	}
	out << "}";
}

// the main interface to reading and writing the Minecraft NBT format
value* read(std::istream& input) {
	return get<value*>(input);
}

void write(value* x, std::ostream& output) {
	put(output, x);
}

void show(value* x, std::ostream& output) {
	output << x->name() << "=";
	x->show(output);
}

}

