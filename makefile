all: main

LDLIBS+=-lSDL2 -lSDL2_ttf

main: main.o graphics_sdl.o
main.o: main.cpp graphics_sdl.h
graphics_sdl.o: graphics_sdl.cpp graphics_sdl.h

%: %.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
