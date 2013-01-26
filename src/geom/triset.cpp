
#include <geom/triset.hpp>
#include <stdexcept>

namespace geom {

double min(const doubles& vs);
double max(const doubles& vs);

point::point(double x, double y, double z, double u, double v) : x(x), y(y), z(z), u(u), v(v) {
}

point point::operator-(const point& rhs) const {
	return point(x - rhs.x, y - rhs.y, z - rhs.z, u - rhs.u, v - rhs.v);
}

void point::scale(double sx, double sy, double sz) {
	x *= sx;
	y *= sy;
	z *= sz;
}

triangle::triangle(const point& p0, const point& p1, const point& p2, color::texture* texture) : p0(p0), p1(p1), p2(p2), texture(texture) {
}

triangle triangle::operator-(const point& rhs) const {
	return triangle(p0 - rhs, p1 - rhs, p2 - rhs, texture);
}

color::value triangle::color(double u, double v) const {
	if (this->texture) {
		return this->texture->texel(u, v);
	} else {
		return color::make(0xff, 0xff, 0xff, 0xff);
	}
}

void triangle::scale(double sx, double sy, double sz) {
	p0.scale(sx, sy, sz);
	p1.scale(sx, sy, sz);
	p2.scale(sx, sy, sz);
}

size_t triset::size() const {
	return this->textures.size();
}

triangle triset::operator[](unsigned int i) const {
	return at(i);
}

triangle triset::at(unsigned int i) const {
	if (i >= size()) {
		throw std::runtime_error("Triset index out of bounds.");
	} else {
		unsigned int bi = i * 3;
		return triangle(pointAt(bi), pointAt(bi + 1), pointAt(bi + 2), this->textures[i]);
	}
}

double triset::minX() const { return min(this->xs); }
double triset::maxX() const { return max(this->xs); }
double triset::minY() const { return min(this->ys); }
double triset::maxY() const { return max(this->ys); }
double triset::minZ() const { return min(this->zs); }
double triset::maxZ() const { return max(this->zs); }

point triset::pointAt(unsigned int i) const {
	if (i >= this->xs.size()) {
		throw std::runtime_error("Triset index out of bounds.");
	} else {
		return point(this->xs[i], this->ys[i], this->zs[i], this->us[i], this->vs[i]);
	}
}

void triset::append(const triangle& tri) {
	append(tri.p0);
	append(tri.p1);
	append(tri.p2);

	this->textures.push_back(tri.texture);
}

void triset::append(const point& p) {
	this->xs.push_back(p.x);
	this->ys.push_back(p.y);
	this->zs.push_back(p.z);

	this->us.push_back(p.u);
	this->vs.push_back(p.v);
}

void triset::clear() {
	this->xs.clear();
	this->ys.clear();
	this->zs.clear();

	this->us.clear();
	this->vs.clear();
	this->textures.clear();
}

double min(const doubles& vs) {
	if (vs.size() == 0) {
		return 0.0;
	} else {
		doubles::const_iterator i = vs.begin();
		double result = *i;
		++i;

		for (; i != vs.end(); ++i) {
			result = std::min(result, *i);
		}

		return result;
	}
}

double max(const doubles& vs) {
	if (vs.size() == 0) {
		return 0.0;
	} else {
		doubles::const_iterator i = vs.begin();
		double result = *i;
		++i;

		for (; i != vs.end(); ++i) {
			result = std::max(result, *i);
		}

		return result;
	}
}

}

