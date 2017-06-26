#
# Cross Platform Makefile
# Compatible with Ubuntu 14.04.1 and Mac OS X
#
#
# if you using Mac OS X:
# You'll need glfw
#    http://www.glfw.org
#

#CXX = g++

EXE = opengl2_example
#OBJS = main.o imgui_impl_glfw.o
OBJS = ../imgui/imgui_impl_sdl.o  ../imgui/imgui.o ../imgui/imgui_demo.o ../imgui/imgui_draw.o main.o
UNAME_S := $(shell uname -s)


ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS = -lGL `pkg-config --static --libs glfw3` `sdl2-config --libs`

	CXXFLAGS = -I../imgui/ -I. `pkg-config --cflags glfw3` `sdl2-config --cflags`

	CXXFLAGS += -Wall -Wformat
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	#LIBS += -L/usr/local/lib -lglfw3
	LIBS += -L/usr/local/lib -lglfw

	CXXFLAGS = -I../../ -I/usr/local/include
	CXXFLAGS += -Wall -Wformat
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), MINGW64_NT-6.3)
   ECHO_MESSAGE = "Windows"
   LIBS = -lglfw3 -lgdi32 -lopengl32 -limm32

   CXXFLAGS = -I../../ -I../libs/gl3w `pkg-config --cflags glfw3`    CXXFLAGS += -Wall -Wformat
   CFLAGS = $(CXXFLAGS)
endif

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $(EXE) $(OBJS) $(CXXFLAGS) $(LIBS)

clean:
	rm $(EXE) $(OBJS)

