OBJECTS += $(patsubst %.cpp,%.o,$(wildcard src/*.cpp))

CPPFLAGS += -Iinclude -I../
CXXFLAGS += -fPIC

LDFLAGS += -L../gimxlog
LDLIBS += -lgimxlog

ifeq ($(OS),Windows_NT)
LDLIBS += -lshell32 -luuid -lole32
endif

include Makedefs
