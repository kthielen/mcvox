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
	// our final face-set, suitable for rendering
	geom::triset data;

	typedef std::map<std::string, color::texture> Textures;
	Textures textures;
	void readTextures(const std::string& texfile);

	// process commands in the source OBJ file
	typedef std::vector<std::string> ObjCmd;

	void processCommand(const std::string& basedir, const std::string& cn, const ObjCmd& args);

	// reader state -- these are vertices/texture-coords, and procedures for operating on them
	color::texture* currentTexture;

	typedef std::vector<double> doubles;
	doubles xs;
	doubles ys;
	doubles zs;
	doubles us;
	doubles vs;

	void appendVertex(const std::string& x, const std::string& y, const std::string& z);
	void appendVertex(double x, double y, double z);
	void appendTexCoord(const std::string& u, const std::string& v);
	void appendTexCoord(double u, double v);

	geom::point point(const std::string& pd) const;
	geom::point point(int vtx, int tex) const;
	geom::point point(int vtx) const;

	int vertex (int rv) const; // normalize OBJ vertex indices
	int texture(int rv) const; // normalize OBJ texture indices
private:
	// turn off unwanted construction options
	reader();
	reader(const reader&);
	reader& operator=(const reader&);
};

}

#endif
