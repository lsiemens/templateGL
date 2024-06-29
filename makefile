CXX = g++
LD = $(CXX)

CXXFLAGS = -g -Wall
CPPFLAGS =

LDFLAGS = -g
LDLIBS = -lGLEW -lglfw -lGL -ldl

SRCS = testGL.cpp looplog.cpp frame_timer.cpp model.cpp object.cpp camera.cpp shaders.cpp
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))
TARGET = $(BUILD_DIR)/testGL

BUILD_DIR = build

all: $(BUILD_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(CPPFLAGS)

$(BUILD_DIR):
	mkdir $(BUILD_DIR) -p

open_documentation: Documentation
	open ./Documentation/html/index.html

Documentation:
	doxygen Doxyfile

clean:
	rm $(BUILD_DIR) -rf

clean_doc:
	rm Documentation -rf

.PHONY: all run open_documentation clean clean_doc
