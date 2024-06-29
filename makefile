CXX = g++
LD = $(CXX)

CXXFLAGS = -g -Wall
CPPFLAGS = -I $(CORE_DIR)

LDFLAGS = -g
LDLIBS = -lGLEW -lglfw -lGL -ldl

BUILD_DIR = build
CORE_DIR = core

CORE_SRCS = looplog.cpp frame_timer.cpp model.cpp object.cpp camera.cpp shaders.cpp
APP_SRCS = testGL.cpp
SRCS = $(APP_SRCS) $(addprefix $(CORE_DIR)/, $(CORE_SRCS))
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(APP_SRCS) $(CORE_SRCS))
TARGET = $(BUILD_DIR)/testGL

all: $(BUILD_DIR) $(TARGET)

run: $(BUILD_DIR) $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(CPPFLAGS)

$(BUILD_DIR)/%.o: $(CORE_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(CPPFLAGS)

$(BUILD_DIR):
	mkdir $(BUILD_DIR) -p

open_documentation: Documentation
	open ./Documentation/html/index.html

Documentation:
	doxygen Doxyfile

clean:
	rm $(BUILD_DIR) -rf
	rm Documentation -rf

.PHONY: all run open_documentation clean
