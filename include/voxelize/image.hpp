#ifndef VOXELIZE_IMAGE_HPP_INCLUDED
#define VOXELIZE_IMAGE_HPP_INCLUDED

#include <Magick++.h>
#include <geom/voxel.hpp>

namespace voxelize {

class image : public geom::volume {
public:
	image(unsigned int maxVoxExt, const std::string& file);

	unsigned int width()  const;
	unsigned int height() const;
	unsigned int depth()  const;

	color::value voxel(unsigned int x, unsigned int y, unsigned int z) const;
private:
	Magick::Image img;
};

}

#endif
