
# generic compiler options
compiler = g++ -std=c++20 -I../src/ -I../libraries/glfw3/
libraries = -L../libraries/binaries/win64/ $\
	-lglfw3 -lgdi32 -lopengl32

# compile all of the tests
compile:
	$(compiler) core.cpp -o core $(libraries)
	
# run all of the tests
test: compile
	core