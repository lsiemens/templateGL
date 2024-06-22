run: hi; ./hi

hi: hi.cpp; g++ hi.cpp -o hi

clean: ; rm -f hi
