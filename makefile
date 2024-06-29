CXX = g++
LD = $(CXX)

CXXFLAGS = -g -Wall
CPPFLAGS =

LDFLAGS = -g
LDLIBS = -lGLEW -lglfw -lGL -ldl

SRCS = testGL.cpp looplog.cpp frame_timer.cpp model.cpp object.cpp camera.cpp shaders.cpp
OBJS = $(subst .cpp,.o, $(SRCS))
TARGET = testGL

run: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

$(OBJS): %.o: %.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS) $(CPPFLAGS)

open_documentation: Documentation
	open ./Documentation/html/index.html

Documentation:
	doxygen Doxyfile

clean:
	rm $(OBJS) -f
	rm $(TARGET) -f

clean_doc:
	rm Documentation -rf

.PHONY: clean clean_doc
