GLFLAG=-lGLEW -lglfw -lGL -ldl
FLAGS=-Wall -g

run: testGL
	./testGL

testGL: looplog.o frame_timer.o model.o object.o camera.o
	g++ testGL.cpp looplog.o frame_timer.o model.o object.o camera.o -o testGL $(GLFLAG) $(FLAGS)

camera.o:
	g++ -c camera.cpp -o camera.o $(GLFLAG) $(FLAGS)

object.o:
	g++ -c object.cpp -o object.o $(GLFLAG) $(FLAGS)

model.o:
	g++ -c model.cpp -o model.o $(GLFLAG) $(FLAGS)

frame_timer.o:
	g++ -c frame_timer.cpp -o frame_timer.o $(FLAGS)

looplog.o:
	g++ -c looplog.cpp -o looplog.o $(FLAGS)

open_documentation: Documentation
	open ./Documentation/html/index.html

Documentation:
	doxygen Doxyfile

clean:
	rm model.o object.o camera.o -f
	rm looplog.o frame_timer.o -f
	rm testGL -f

clean_doc:
	rm Documentation -rf
