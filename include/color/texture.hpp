#ifndef COLOR_TEXTURE_HPP_INCLUDED
#define COLOR_TEXTURE_HPP_INCLUDED

#include <color/data.hpp>
#include <Magick++.h>
#include <string>

namespace color {

class texture {
public:
	texture(const std::string& filename);
	texture();

	void load(const std::string& filename);

	unsigned int width() const;
	unsigned int height() const;

	color::value texel(double u, double v) const;
	color::value texel(int tx, int ty) const;
private:
	Magick::Image image;
	unsigned int  cx, cy;

	const Magick::PixelPacket* pixels;
};

}

#endif
