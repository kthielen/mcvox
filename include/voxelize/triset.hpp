#ifndef VOXELIZE_TRISET_HPP_INCLUDED
#define VOXELIZE_TRISET_HPP_INCLUDED

#include <geom/triset.hpp>
#include <geom/voxel.hpp>
#include <color/data.hpp>
#include <list>
#include <string>

namespace voxelize {

typedef void (*PROGRESSFN)(const std::string&,unsigned int,unsigned int);

struct aabb {
	double x0, x1;
	double y0, y1;
	double z0, z1;

	aabb(double x0, double x1, double y0, double y1, double z0, double z1);

	double width() const;
	double height() const;
	double depth() const;
};

typedef std::list<color::value> colors;

class triset : public geom::volume {
public:
	triset(unsigned int maxVoxExt, const geom::triset& tris, PROGRESSFN pfn = 0);
	~triset();

	unsigned int width()  const;
	unsigned int height() const;
	unsigned int depth()  const;

	color::value voxel(unsigned int x, unsigned int y, unsigned int z) const;
private:
	// the main mesh -> voxel rasterization process
	void rasterize(const geom::triangle& tri);
private:
	unsigned int w;
	unsigned int h;
	unsigned int d;
	void initVolume(unsigned int maxVoxExt, double cx, double cy, double cz);

	const colors* lookup(unsigned int x, unsigned int y, unsigned int z) const;
	colors* cell(unsigned int x, unsigned int y, unsigned int z);
	unsigned int index(unsigned int x, unsigned int y, unsigned int z) const;
	void alloc();
	void free();
	colors** data;
};

}

#endif
