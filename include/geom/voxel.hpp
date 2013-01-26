#ifndef GEOM_VOXEL_HPP_INCLUDED
#define GEOM_VOXEL_HPP_INCLUDED

#include <color/data.hpp>

namespace geom {

struct volume {
	virtual unsigned int width()  const = 0;
	virtual unsigned int height() const = 0;
	virtual unsigned int depth()  const = 0;

	virtual color::value voxel(unsigned int x, unsigned int y, unsigned int z) const = 0;

	virtual ~volume();
};

}

#endif
