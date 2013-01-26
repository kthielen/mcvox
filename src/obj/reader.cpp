
#include <obj/reader.hpp>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace obj {

inline std::string basepath(const std::string& filename) {
	str::string_pair p = str::rsplit<char>(filename, "/");
	return p.first.empty() ? "." : p.first;
}

typedef std::vector<double> doubles;

struct objdata {
	doubles xs;
	doubles ys;
	doubles zs;
	doubles us;
	doubles vs;

	int vertex(int rv) const {
		if (rv < 0) {
			return rv + int(this->xs.size());
		} else if (rv > 0 && rv <= this->xs.size()) {
			return rv - 1;
		} else {
			throw std::runtime_error("Invalid OBJ vertex index: " + str::to_string(rv) + " (out of " + str::to_string(this->xs.size()) + ")");
		}
	}

	int texture(int rv) const {
		if (rv < 0) {
			return rv + int(this->us.size());
		} else if (rv > 0 && rv <= this->us.size()) {
			return rv - 1;
		} else {
			throw std::runtime_error("Invalid OBJ texture index: " + str::to_string(rv) + " (out of " + str::to_string(this->us.size()) + ")");
		}
	}

	geom::point point(int vtx, int tex) const {
		vtx = vertex(vtx);
		tex = texture(tex);

		return geom::point(this->xs[vtx], this->ys[vtx], this->zs[vtx], this->us[tex], this->vs[tex]);
	}

	geom::point point(int vtx) const {
		vtx = vertex(vtx);

		return geom::point(this->xs[vtx], this->ys[vtx], this->zs[vtx], 0.0, 0.0);
	}

	geom::point point(const std::string& pd) const {
		str::string_pair p = str::lsplit<char>(pd, "/");
		int vi = str::from_string<int>(p.first);
		int ti = str::from_string<int>(p.second);

		if (ti != 0) {
			return point(vi, ti);
		} else {
			return point(vi);
		}
	}

	void appendVertex(double x, double y, double z) {
		this->xs.push_back(x);
		this->ys.push_back(y);
		this->zs.push_back(z);
	}

	void appendVertex(const std::string& x, const std::string& y, const std::string& z) {
		appendVertex(str::from_string<double>(x), str::from_string<double>(y), str::from_string<double>(z));
	}

	void appendTexCoord(double u, double v) {
		this->us.push_back(u);
		this->vs.push_back(v);
	}

	void appendTexCoord(const std::string& u, const std::string& v) {
		appendTexCoord(str::from_string<double>(u), str::from_string<double>(v));
	}
};

reader::reader(const std::string& filename, PROGRESSFN pfn) {
	std::ifstream f(filename.c_str());
	if (!f.is_open()) {
		throw std::runtime_error("Couldn't open OBJ file '" + filename + "' for reading.");
	}

	objdata fd;
	color::texture* currentTexture = 0;

	if (pfn) { pfn("Loading '" + filename + "'", 0, 0); }

	while (f) {
		std::string line;
		std::getline(f, line);
		line = str::trim(line);
		if (line.size() == 0 || line[0] == '#') continue;

		str::StrVec cmd = str::csplit<char>(line, " ");
		if (cmd[0] == "v") {
			if (cmd.size() != 4) {
				throw std::runtime_error("Invalid OBJ vertex command: " + line);
			} else {
				fd.appendVertex(cmd[1], cmd[2], cmd[3]);
			}
		} else if (cmd[0] == "vt") {
			if (cmd.size() < 3) {
				throw std::runtime_error("Invalid OBJ texture coord command: " + line);
			} else {
				fd.appendTexCoord(cmd[1], cmd[2]);
			}
		} else if (cmd[0] == "f") {
			if (cmd.size() != 4) {
				throw std::runtime_error("Invalid OBJ face command: " + line);
			} else {
				this->data.append(geom::triangle(fd.point(cmd[1]), fd.point(cmd[2]), fd.point(cmd[3]), currentTexture));
			}
		} else if (cmd[0] == "usemtl") {
			Textures::iterator t = this->textures.find(cmd[1]);
			if (t != this->textures.end()) {
				currentTexture = &(t->second);
			} else {
				throw std::runtime_error("No such texture: " + cmd[1]);
			}
		} else if (cmd[0] == "g") {
			// redundant 'usemtl' command
		} else if (cmd[0] == "mtllib") {
			readTextures(basepath(filename) + "/" + cmd[1]);
		} else {
			throw std::runtime_error("Unsupported OBJ file command: " + line);
		}
	}
}

const geom::triset& reader::faces() const {
	return this->data;
}

void reader::readTextures(const std::string& texFile) {
	std::ifstream f(texFile.c_str());
	if (!f.is_open()) {
		throw std::runtime_error("Cannot open texture file, '" + texFile + "'.");
	}

	std::string texName = "";

	while (f) {
		std::string line;
		std::getline(f, line);
		line = str::trim(line);
		if (line.size() == 0 || line[0] == '#') continue;

		str::StrVec cmd = str::csplit<char>(line, " ");
		if (cmd[0] == "newmtl") {
			texName = cmd[1];
			this->textures[texName] = color::texture();
		} else if (cmd[0] == "map_Kd") {
			this->textures[texName].load(basepath(texFile) + "/" + cmd[1]);
		}
	}
}

}
