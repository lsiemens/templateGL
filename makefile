GLFLAG=-lGLEW -lglfw -lGL -ldl
FLAGS=-Wall -g

run: testGL
	./testGL

testGL: looplog.o
	g++ testGL.cpp looplog.o -o testGL $(GLFLAG) $(FLAGS)

looplog.o:
	g++ -c looplog.cpp -o looplog.o $(FLAGS)

open_documentation: Documentation
	open ./Documentation/html/index.html

Documentation:
	doxygen Doxyfile

clean:
	rm looplog.o -f
	rm testGL -f

clean_doc:
	rm Documentation -rf
