ifeq ($(OS),Windows_NT)
    CXX = C:\mingw64\bin\x86_64-w64-mingw32-g++.exe
    RM = del /Q
    EXE = .exe
else
    CXX = g++
    RM = rm -f
    EXE =
endif

CXXFLAGS = -O2 -std=c++17 -Wall -Wextra

SRCS = main.cpp read_hypergraph.cpp degeneracy.cpp HypergraphCSR.cpp triangles_and_contained.cpp stars.cpp open_patterns.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = hypertriangles_fast$(EXE)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug: CXXFLAGS = -g -O0 -Wall -std=c++17
debug: $(TARGET)

clean:
	$(RM) *.o $(TARGET)