
#include <voxelize/triset.hpp>
#include <geom/line.hpp>
#include <iostream>

namespace voxelize {

inline void putVoxel(colors* c, color::value x) {
	c->push_back(x);
}

// rasterize a 3D triangle (in bounds-space) to our voxel grid
void triset::rasterize(const geom::triangle& tri) {
	double ls0[] = { tri.p0.x, tri.p0.y, tri.p0.z, tri.p0.u, tri.p0.v, /**/ tri.p1.x, tri.p1.y, tri.p1.z, tri.p1.u, tri.p1.v };
	double ls1[] = { tri.p2.x, tri.p2.y, tri.p2.z, tri.p2.u, tri.p2.v, /**/ tri.p2.x, tri.p2.y, tri.p2.z, tri.p2.u, tri.p2.v };

	// now triangulate
	geom::line<10> area(ls0, ls1);
	while (!area.done()) {
		const double* ln = area.point();

		double x0 = ln[0];
		double y0 = ln[1];
		double z0 = ln[2];
		double u0 = ln[3];
		double v0 = ln[4];
		double p0[] = { x0, y0, z0, u0, v0 };

		double x1 = ln[5];
		double y1 = ln[6];
		double z1 = ln[7];
		double u1 = ln[8];
		double v1 = ln[9];
		double p1[] = { x1, y1, z1, u1, v1 };

		geom::line<5> line(p0, p1);
		while (!line.done()) {
			const double* pt = line.point();
			
			double x = pt[0];
			double y = pt[1];
			double z = pt[2];
			double u = pt[3];
			double v = pt[4];

			if (isnan(x)) { x = 0; }
			if (isnan(y)) { y = 0; }
			if (isnan(z)) { z = 0; }

			int pxs[] = { int(floor(x)), int(ceil(x)) };
			int pys[] = { int(floor(y)), int(ceil(y)) };
			int pzs[] = { int(floor(z)), int(ceil(z)) };
			color::value c = tri.color(u, v);

			for (int xi = 0; xi < 2; ++xi) {
				for (int yi = 0; yi < 2; ++yi) {
					for (int zi = 0; zi < 2; ++zi) {
						int vx = std::min<int>(pxs[xi], width()  - 1);
						int vy = std::min<int>(pys[yi], height() - 1);
						int vz = std::min<int>(pzs[zi], depth()  - 1);

						putVoxel(cell(vx, vy, vz), c);
					}
				}
			}

			++line;
		}

		++area;
	}
}

// the basic triset/volume wrapper
triset::triset(unsigned int maxVoxExt, const geom::triset& tris, PROGRESSFN pfn) {
	aabb bounds(tris.minX(), tris.maxX(), tris.minY(), tris.maxY(), tris.minZ(), tris.maxZ());

	initVolume(maxVoxExt, bounds.width(), bounds.height(), bounds.depth());
	alloc();

	// allow triangle coordinates to be normalized to voxel space
	geom::point to = geom::point(bounds.x0, bounds.y0, bounds.z0);
	double      sx = double(width() - 1) / bounds.width();
	double      sy = double(height() - 1) / bounds.height();
	double      sz = double(depth() - 1) / bounds.depth();

	size_t n = tris.size();
	for (unsigned int i = 0; i < n; ++i) {
		if (pfn) {
			pfn("Voxelizing triangle", i, n);
		}

		// convert this triangle into voxel volume coordinates
		geom::triangle tri = tris[i] - to;
		tri.scale(sx, sy, sz);

		// put voxels on this surface into the voxel volume
		rasterize(tri);
	}
}

triset::~triset() {
	free();
}

void triset::initVolume(unsigned int maxVoxExt, double cx, double cy, double cz) {
	if (cx > cy && cx > cz) {
		this->w = maxVoxExt;
		this->h = (unsigned int)(double(maxVoxExt) * (cy / cx));
		this->d = (unsigned int)(double(maxVoxExt) * (cz / cx));
	} else if (cz > cy) {
		this->d = maxVoxExt;
		this->w = (unsigned int)(double(maxVoxExt) * (cx / cz));
		this->h = (unsigned int)(double(maxVoxExt) * (cy / cz));
	} else {
		this->h = maxVoxExt;
		this->d = (unsigned int)(double(maxVoxExt) * (cz / cy));
		this->w = (unsigned int)(double(maxVoxExt) * (cx / cy));
	}

	if (this->w <= 1) { this->w = 1; }
	if (this->h <= 1) { this->h = 1; }
	if (this->d <= 1) { this->d = 1; }
}

unsigned int triset::width()  const { return this->w; }
unsigned int triset::height() const { return this->h; }
unsigned int triset::depth()  const { return this->d; }

color::value triset::voxel(unsigned int x, unsigned int y, unsigned int z) const {
	const colors* cs = lookup(x, y, z);
	if (cs == 0) {
		return color::make(0,0,0,0);
	} else {
		return color::average(*cs);
	}
}

const colors* triset::lookup(unsigned int x, unsigned int y, unsigned int z) const {
	return this->data[index(x, y, z)];
}

colors* triset::cell(unsigned int x, unsigned int y, unsigned int z) {
	unsigned int i = index(x, y, z);
	colors* c = this->data[i];
	if (c == 0) {
		c = new colors();
		this->data[i] = c;
	}
	return c;
}

unsigned int triset::index(unsigned int x, unsigned int y, unsigned int z) const {
	return x + (width() * y) + (width() * height() * z);
}

void triset::alloc() {
	unsigned int sz = width() * height() * depth();
	this->data = new colors*[sz];
	for (unsigned int i = 0; i < sz; ++i) {
		this->data[i] = 0;
	}
}

void triset::free() {
	delete[] this->data;
	this->data = 0;
}

// an axis-aligned bounding box in 3D space
aabb::aabb(double x0, double x1, double y0, double y1, double z0, double z1) : x0(x0), x1(x1), y0(y0), y1(y1), z0(z0), z1(z1) {
}

double aabb::width()  const { return this->x1 - this->x0; }
double aabb::height() const { return this->y1 - this->y0; }
double aabb::depth()  const { return this->z1 - this->z0; }

}

