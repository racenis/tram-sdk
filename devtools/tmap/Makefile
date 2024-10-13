default: engine_libs tmap

tmap: main.cpp
	g++ -std=c++20 main.cpp engine_libs.o -o tmap.exe -I../../src/ -I../../libraries/

tmap_release: main.cpp
	g++ -std=c++20 main.cpp engine_libs.o -o tmap.exe -static -static-libstdc++ -I../../src/ -I../../libraries/
	
engine_libs: engine_libs.cpp
	gcc -std=c++20 -c engine_libs.cpp -o engine_libs.o -I../../src/ -I../../libraries/
	
clean:
	del engine_libs.o