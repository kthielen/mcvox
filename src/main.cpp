
#include <obj/reader.hpp>
#include <voxelize/image.hpp>
#include <voxelize/triset.hpp>
#include <mc/schematic.hpp>
#include <Magick++.h>

#include <sys/time.h>
#include <time.h>

void resetCounter();
void progress(const std::string& msg, unsigned int s, unsigned int c);

void usage(int argc, char** argv) {
	std::cout << "usage: " << argv[0] << " -i <input> -o <output> [-m <max-extent>]"   << std::endl
			  << "  where"                                                             << std::endl
			  << "    input      : The .OBJ or image file to import."                  << std::endl
			  << "    output     : The voxelized Minecraft .schematic file to export." << std::endl
			  << "    max-extent : The maximum number of blocks on any axis (1-256)."  << std::endl
			  << std::endl;

	exit(-1);
}

// read program configuration from the command-line
struct config {
	unsigned int maximumDimension;
	std::string  inputObjFile;
	std::string  outputSchematicFile;
};

config readConfiguration(int argc, char** argv) {
	config result;
	result.maximumDimension = 0;

	for (int arg = 1; arg < argc; ++arg) {
		std::string a = argv[arg];
		std::string b = ((arg+1) == argc ? "" : argv[arg+1]);

		if (a == "-m" || a == "--maxEdge" || a == "--maxExtent") {
			result.maximumDimension = str::from_string<unsigned int>(b);
			++arg;
		} else if (a == "-i" || a == "--input") {
			result.inputObjFile = b;
			++arg;
		} else if (a == "-o" || a == "--output" || a == "--outputSchematic") {
			result.outputSchematicFile = b;
			++arg;
		} else {
			std::cout << "Warning, argument ignored: " << a << std::endl;
		}
	}

	// did we read a valid input?
	if ((result.maximumDimension == 0 || result.maximumDimension > 256) || result.inputObjFile.empty() || result.outputSchematicFile.empty()) {
		usage(argc, argv);
	}

	return result;
}

// perform OBJ -> MC-schematic voxelization
int main(int argc, char** argv) {
	config input = readConfiguration(argc, argv);

	try {
		std::cout << "Converting mesh '" << input.inputObjFile << "' to MC-schematic '" << input.outputSchematicFile << "'.";

		Magick::InitializeMagick(argv[0]);

		// process input
		resetCounter();
		obj::reader in(input.inputObjFile, &progress);

		// prepare output voxels
		resetCounter();
		voxelize::triset volume(input.maximumDimension, in.faces(), &progress);

		// write voxels to MC file
		resetCounter();
		mc::save(volume, input.outputSchematicFile, &progress);

		// hooray!  we did it!
		std::cout << std::endl << "Done." << std::endl;
		return 0;
	} catch (std::exception& ex) {
		// failure is an option
		std::cout << std::endl << ex.what() << std::endl;
		return -1;
	}
}

// print out conversion progress incrementally
double ticks() {
	timeval tv; memset(&tv, 0, sizeof(tv));
	gettimeofday(&tv, 0);
	return double(tv.tv_sec) * 1000.0 + double(tv.tv_usec) / 1000.0;
}

double initTick = 0.0;
void resetCounter() {
	initTick = ticks();
	std::cout << std::endl;
}

void descProgress(std::ostream& out, unsigned int s, unsigned int c) {
	if (s == 0 || c == 0) return;

	double p = double(s + 1) / double(c);
	out << " (" << 100.0 * p << "%, ";

	double e = ticks() - initTick;
	double r = (e / p) - e;

	static const double s1 = 1000.0;
	static const double m1 = s1 * 60.0;
	static const double h1 = m1 * 60.0;
	static const double d1 = h1 * 24.0;

	if (r < s1) {
		out << r << "ms";
	} else if (r < m1) {
		out << r / s1 << "s";
	} else if (r < h1) {
		out << r / m1 << "m";
	} else if (r < d1) {
		out << r / h1 << "h";
	} else {
		out << r / d1 << "d";
	}

	out << " remain)";
}

void resetTerminal() {
	std::cout << char(0x0D);
	for (int i = 0; i < 70; ++i) {
		std::cout << ' ';
	}
	std::cout << char(0x0D);
}

void progress(const std::string& msg, unsigned int s, unsigned int c) {
	resetTerminal();

	std::cout << msg;
	descProgress(std::cout, s, c);
	std::cout << std::flush;
}

