
#include <obj/reader.hpp>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace obj {

inline std::string basepath(const std::string& filename) {
	str::string_pair p = str::rsplit<char>(filename, "/");
	return p.first.empty() ? "." : p.first;
}

const geom::triset& reader::faces() const {
	return this->data;
}

reader::reader(const std::string& filename, PROGRESSFN pfn) : currentTexture(0) {
	std::ifstream f(filename.c_str());
	if (!f.is_open()) {
		throw std::runtime_error("Couldn't open OBJ file '" + filename + "' for reading.");
	}

	if (pfn) { pfn("Loading '" + filename + "'", 0, 1); }

	std::string basedir = basepath(filename);

	while (f) {
		std::string line;
		std::getline(f, line);
		line = str::trim(line);
		if (line.size() == 0 || line[0] == '#') continue;

		ObjCmd cmd = str::csplit<char>(line, " ");
		if (cmd.size() == 0) {
			throw std::runtime_error("Null OBJ command: " + line); // should be impossible
		}

		processCommand(basedir, cmd[0], ObjCmd(cmd.begin() + 1, cmd.end()));
	}

	if (pfn) { pfn("Loading '" + filename + "'", 1, 1); }
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

inline std::string line(const std::string& cn, const str::StrVec& args) {
	if (args.size() == 0) {
		return cn;
	} else {
		return cn + " " + str::cdelim<char>(args, " ");
	}
}

void reader::processCommand(const std::string& basedir, const std::string& cn, const ObjCmd& args) {
	if (cn == "v") {
		if (args.size() == 3) {
			appendVertex(args[0], args[1], args[2]);
		} else {
			throw std::runtime_error("Invalid OBJ vertex command: " + line(cn, args));
		}
	} else if (cn == "vt") {
		if (args.size() >= 2) {
			appendTexCoord(args[0], args[1]);
		} else {
			throw std::runtime_error("Invalid OBJ texture coord command: " + line(cn, args));
		}
	} else if (cn == "f") {
		if (args.size() == 3) {
			this->data.append(geom::triangle(point(args[0]), point(args[1]), point(args[2]), this->currentTexture));
		} else if (args.size() == 4) {
			this->data.append(geom::triangle(point(args[0]), point(args[1]), point(args[2]), this->currentTexture));
			this->data.append(geom::triangle(point(args[1]), point(args[2]), point(args[3]), this->currentTexture));
		} else {
			throw std::runtime_error("Invalid OBJ face command: " + line(cn, args));
		}
	} else if (cn == "usemtl") {
		Textures::iterator t = this->textures.find(args[0]);
		if (t != this->textures.end()) {
			this->currentTexture = &(t->second);
		} else {
			throw std::runtime_error("No such texture: " + args[0]);
		}
	} else if (cn == "g" || cn == "o" || cn == "s") {
		// redundant 'usemtl' command
	} else if (cn == "mtllib") {
		readTextures(basedir + "/" + args[0]);
	} else {
		throw std::runtime_error("Unsupported OBJ file command: " + cn + " " + str::cdelim<char>(args, " "));
	}
}

int reader::vertex(int rv) const {
	if (rv < 0) {
		return rv + int(this->xs.size());
	} else if (rv > 0 && rv <= int(this->xs.size())) {
		return rv - 1;
	} else {
		throw std::runtime_error("Invalid OBJ vertex index: " + str::to_string(rv) + " (out of " + str::to_string(this->xs.size()) + ")");
	}
}

int reader::texture(int rv) const {
	if (rv < 0) {
		return rv + int(this->us.size());
	} else if (rv > 0 && rv <= int(this->us.size())) {
		return rv - 1;
	} else {
		throw std::runtime_error("Invalid OBJ texture index: " + str::to_string(rv) + " (out of " + str::to_string(this->us.size()) + ")");
	}
}

geom::point reader::point(int vtx, int tex) const {
	vtx = vertex(vtx);
	tex = texture(tex);

	return geom::point(this->xs[vtx], this->ys[vtx], this->zs[vtx], this->us[tex], this->vs[tex]);
}

geom::point reader::point(int vtx) const {
	vtx = vertex(vtx);

	return geom::point(this->xs[vtx], this->ys[vtx], this->zs[vtx], 0.0, 0.0);
}

geom::point reader::point(const std::string& pd) const {
	str::string_pair p = str::lsplit<char>(pd, "/");
	int vi = str::from_string<int>(p.first);
	int ti = str::from_string<int>(p.second);

	if (ti != 0) {
		return point(vi, ti);
	} else {
		return point(vi);
	}
}

void reader::appendVertex(double x, double y, double z) {
	this->xs.push_back(x);
	this->ys.push_back(y);
	this->zs.push_back(z);
}

void reader::appendVertex(const std::string& x, const std::string& y, const std::string& z) {
	appendVertex(str::from_string<double>(x), str::from_string<double>(y), str::from_string<double>(z));
}

void reader::appendTexCoord(double u, double v) {
	this->us.push_back(u);
	this->vs.push_back(v);
}

void reader::appendTexCoord(const std::string& u, const std::string& v) {
	appendTexCoord(str::from_string<double>(u), str::from_string<double>(v));
}

}
