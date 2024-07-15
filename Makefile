# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -I/usr/local/include
LDFLAGS = -L/usr/local/lib -lsfml-graphics -lsfml-window -lsfml-system

# Target and source files
TARGET = 3D_Game-app
SRCS = 3D_Game.cpp

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

# Clean target
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean
