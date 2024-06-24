run: testGL; ./testGL

testGL: testGL.cpp; g++ -g testGL.cpp -o testGL -lGLEW -lglfw -lGL -ldl

clean: ; rm -f testGL
