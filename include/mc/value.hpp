
#ifndef MC_VALUE_HPP_INCLUDED
#define MC_VALUE_HPP_INCLUDED

/*
 * value : Minecraft tagged-objects
 */
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <map>

namespace mc {

typedef unsigned char tagid;

class value {
public:
	const char* name() const;
	virtual ~value();

	virtual void show(std::ostream& out) const = 0;
	virtual void write(std::ostream& out) const = 0;
	virtual tagid tag() const = 0;
protected:
	value(const char* vname);
	value(std::istream& in);
private:
	std::string vname;
};

typedef std::vector<value*>      values;
typedef std::pair<tagid, values> hvalues;

#define DEF_BOX_CLASS(N, CT) \
	class N : public value { \
	public: \
		typedef CT data; \
		\
		N(const char* vname, CT v = data()); \
		N(const char* vname, std::istream& in); \
		N(std::istream& in); \
		\
		CT& unbox(); \
		const CT& unbox() const; \
		\
		void show(std::ostream& out) const; \
		void write(std::ostream& out) const; \
		\
		tagid tag() const; \
		static tagid tagID(); \
	private: \
		CT x; \
	}

DEF_BOX_CLASS(byte,   unsigned char);
DEF_BOX_CLASS(int2,   short);
DEF_BOX_CLASS(int4,   int);
DEF_BOX_CLASS(int8,   long);
DEF_BOX_CLASS(float4, float);
DEF_BOX_CLASS(float8, double);
DEF_BOX_CLASS(bytes,  std::vector<unsigned char>);
DEF_BOX_CLASS(int4s,  std::vector<int>);
DEF_BOX_CLASS(string, std::string);
DEF_BOX_CLASS(array,  hvalues);
DEF_BOX_CLASS(tuple,  values);

// the main interface to reading and writing the Minecraft NBT format
value* read(std::istream& input);
void write(value* x, std::ostream& output);
void show(value* x, std::ostream& output);

}

#endif
