
#include <voxelize/image.hpp>

namespace voxelize {

image::image(unsigned int maxVoxExt, const std::string& file) {
	this->img.read(file);

	Magick::Geometry ext = img.boundingBox();
	double pixelsPerVoxel = double((ext.width() > ext.height()) ? ext.width() : ext.height()) / double(maxVoxExt);

	Magick::Geometry resized(size_t(double(ext.width()) / pixelsPerVoxel), size_t(double(ext.height()) / pixelsPerVoxel));
	img.sample(resized);
}

unsigned int image::width()  const {
	return this->img.boundingBox().width();
}

unsigned int image::height() const {
	return this->img.boundingBox().height();
}

unsigned int image::depth()  const {
	return 1;
}

color::value image::voxel(unsigned int x, unsigned int y, unsigned int z) const {
	if (x >= width() || y > height() || z != 0) {
		return color::make(0x00, 0x00, 0x00, 0x00);
	} else {
		const Magick::PixelPacket* p = this->img.getConstPixels(x, height() - y, 1, 1);
		return color::make(p->red, p->green, p->blue, 0xff - p->opacity);
	}
}

}

