default: engine_libs tbsp

tbsp: main.cpp
	g++ -std=c++20 main.cpp engine_libs.o -o tbsp.exe -I../../src/ -I../../libraries/

tbsp_release: main.cpp
	g++ -std=c++20 main.cpp engine_libs.o -o tbsp.exe -static -static-libstdc++ -I../../src/ -I../../libraries/

	
engine_libs: engine_libs.cpp
	gcc -std=c++20 -c engine_libs.cpp -o engine_libs.o -I../../src/ -I../../libraries/
	
clean:
	del engine_libs.o