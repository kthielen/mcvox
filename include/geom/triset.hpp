#ifndef GEOM_TRISET_HPP_INCLUDED
#define GEOM_TRISET_HPP_INCLUDED

#include <color/data.hpp>
#include <color/texture.hpp>
#include <vector>

namespace geom {

struct point {
	double x, y, z; // spatial coordinates
	double u, v;    // texture coordinates

	point(double x, double y, double z, double u = 0.0, double v = 0.0);

	point operator-(const point& rhs) const;

	void scale(double sx, double sy, double sz);
};

struct triangle {
	point p0, p1, p2;
	color::texture* texture;

	triangle(const point& p0, const point& p1, const point& p2, color::texture* texture = 0);

	triangle operator-(const point& rhs) const;
	color::value color(double u, double v) const;

	void scale(double sx, double sy, double sz);
};

typedef std::vector<double>          doubles;
typedef std::vector<color::texture*> Textures;

class triset {
public:
	size_t size() const;
	triangle operator[](unsigned int i) const;
	triangle at(unsigned int i) const;

	double minX() const;
	double maxX() const;
	double minY() const;
	double maxY() const;
	double minZ() const;
	double maxZ() const;

	void append(const triangle& tri);
	void clear();
private:
	doubles xs;
	doubles ys;
	doubles zs;

	doubles us;
	doubles vs;

	Textures textures;

	point pointAt(unsigned int i) const;
	void append(const point& p);
};

}

#endif
