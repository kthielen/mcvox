#ifndef MC_SCHEMATIC_HPP_INCLUDED
#define MC_SCHEMATIC_HPP_INCLUDED

/*
 * schematic: save voxels to a Minecraft schematic file
 */
#include <color/data.hpp>
#include <geom/voxel.hpp>

#include <iostream>
#include <string>

namespace mc {

typedef void (*PROGRESSFN)(const std::string&,unsigned int,unsigned int);

void save(const geom::volume& v, const std::string& filename, PROGRESSFN pfn = 0);

}

#endif
