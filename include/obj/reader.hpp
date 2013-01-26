#ifndef OBJ_READER_HPP_INCLUDED
#define OBJ_READER_HPP_INCLUDED

#include <geom/triset.hpp>
#include <color/texture.hpp>
#include <string>
#include <map>
#include <vector>

namespace obj {

typedef void (*PROGRESSFN)(const std::string&,unsigned int,unsigned int);

class reader {
public:
	reader(const std::string& filename, PROGRESSFN pfn = 0);

	const geom::triset& faces() const;
private:
	geom::triset data;

	typedef std::map<std::string, color::texture> Textures;
	Textures textures;

	void readTextures(const std::string& texfile);
};

}

#endif
