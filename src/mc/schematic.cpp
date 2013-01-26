
#include <mc/schematic.hpp>
#include <mc/value.hpp>
#include <io/gzip_stream.hpp>

namespace mc {

struct color_entry {
	color_entry(color::value c, unsigned char block, unsigned char data) : c(c), block(block), data(data) { }

	color::value  c;
	unsigned char block;
	unsigned char data;
};
typedef std::vector<color_entry> color_entries;

color_entries initCE() {
	color_entries result;
#	define CE(rgb,block,data) result.push_back(color_entry(color::make(rgb), block, data))
	CE(0xffffff, 0x23, 0x00);
	CE(0xd5712f, 0x23, 0x01);
	CE(0xb65abe, 0x23, 0x02);
	CE(0x6586c7, 0x23, 0x03);
	CE(0xb3a828, 0x23, 0x04);
	CE(0x43b63b, 0x23, 0x05);
	CE(0xd38ca0, 0x23, 0x06);
	CE(0x404040, 0x23, 0x07);
	CE(0xaaaaaa, 0x23, 0x08);
	CE(0x2e6f8a, 0x23, 0x09);
	CE(0x8240ba, 0x23, 0x0a);
	CE(0x313c94, 0x23, 0x0b);
	CE(0x573722, 0x23, 0x0c);
	CE(0x36491c, 0x23, 0x0d);
	CE(0xa43935, 0x23, 0x0e);
	CE(0x101010, 0x23, 0x0f);
	return result;
}

static color_entries color_map = initCE();

void toMCVoxel(color::value c, unsigned char& block, unsigned char& data) {
	if (color::alpha(c) <= 128) {
		// this voxel looks clear, make it air
		block = 0;
		data  = 0;
	} else {
		unsigned int i  = 0;
		double       md = color::distsq(c, color_map[0].c);

		for (unsigned int t = 1; t < color_map.size(); ++t) {
			double td = color::distsq(c, color_map[t].c);
			if (td < md) {
				md = td;
				i  = t;
			}
		}

		block = color_map[i].block;
		data  = color_map[i].data;
	}
}

void save(const geom::volume& v, const std::string& filename, PROGRESSFN pfn) {
	unsigned int cx = v.width();
	unsigned int cy = v.height();
	unsigned int cz = v.depth();

	// try to open the compressed output stream
	io::gzip_ostream<char> out(filename);

	// make the MC value to wrap this data
	tuple* x = new tuple("Schematic");
	x->unbox().push_back(new int2("Width",  short(cx)));
	x->unbox().push_back(new int2("Length", short(cz)));
	x->unbox().push_back(new int2("Height", short(cy)));
	x->unbox().push_back(new string("Materials", "Alpha"));

	bytes* blocks       = new bytes("Blocks");
	bytes* datas        = new bytes("Data");
	array* entities     = new array("Entities", hvalues(tuple::tagID(), values()));
	array* tileEntities = new array("TileEntities", hvalues(tuple::tagID(), values()));

	x->unbox().push_back(blocks);
	x->unbox().push_back(datas);
	x->unbox().push_back(entities);
	x->unbox().push_back(tileEntities);

	// voxelize the input volume
	typedef std::vector<unsigned char> BVec;
	BVec& blocksv = blocks->unbox();
	BVec& datasv  = datas->unbox();
	
	blocksv.reserve(cx * cy * cz);
	datasv.reserve(cx * cy * cz);

	for (unsigned int y = 0; y < cy; ++y) {
		for (unsigned int z = 0; z < cz; ++z) {
			if (pfn) {
				pfn("Writing voxels", y + (cy * z), cy * cz);
			}

			for (unsigned int x = 0; x < cx; ++x) {
				unsigned char block = 0;
				unsigned char data  = 0;

				toMCVoxel(v.voxel(x, y, z), block, data);
				blocksv.push_back(block);
				datasv.push_back(data);
			}
		}
	}

	// write the complete volume and we're done
	write(x, out);
	delete x;
}

}

