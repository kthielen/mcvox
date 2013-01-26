
CC=g++
PWD=`pwd`

INCDIRS := ./include/
LIBS := z

SOURCES = \
	src/color/texture.cpp \
	src/geom/triset.cpp \
	src/geom/voxel.cpp \
	src/main.cpp \
	src/mc/schematic.cpp \
	src/mc/value.cpp \
	src/obj/reader.cpp \
	src/voxelize/image.cpp \
	src/voxelize/triset.cpp

ifdef DEBUG
	OPTARG := -g
	TDIR   := debug
	EXNAME := dmcvox
else
	OPTARG := -O4
	TDIR   := release
	EXNAME := mcvox
endif

ifdef PROFILE
	PROFARG := -pg
else
	PROFARG :=
endif

OBJECTS := $(addprefix build/obj/$(TDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

CPPFLAGS := -pthread $(INCDIRS:%=-I%) $(OPTARG) $(PROFARG) -m64 -Wall -Wno-deprecated `Magick++-config --cppflags`
LIBTEXT  := $(addprefix -l, $(LIBS)) `Magick++-config --ldflags`

mcvox: dirs $(OBJECTS)
	$(CC) $(CPPFLAGS) $(OBJECTS) $(LIBTEXT) -o build/bin/$(EXNAME)

build/obj/$(TDIR)/%.o:%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

dirs:
	mkdir -p build/obj/$(TDIR)/
	mkdir -p build/bin/
	find . -type d | grep -v "build" | grep -v '\.$$' | awk '{print "build/obj/$(TDIR)/" $$0}' | xargs mkdir -p

clean:
	rm -rf build
