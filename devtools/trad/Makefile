default: engine_libs.o trad

trad: main.cpp
	g++ -std=c++20 main.cpp engine_libs.o -g -o trad.exe -I../../src/ -I../../libraries/

trad_release: main.cpp
	g++ -std=c++20 main.cpp engine_libs.o -o trad.exe -static -static-libstdc++ -I../../src/ -I../../libraries/
	
tradfast: main.cpp
	g++ -std=c++20 -O3 -fno-math-errno -fno-trapping-math main.cpp engine_libs.o -o trad.exe -I../../src/ -I../../libraries/
	
engine_libs.o: engine_libs.cpp
	gcc -std=c++20 -c engine_libs.cpp -o engine_libs.o -I../../src/ -I../../libraries/
	
engine_libsfast: engine_libs.cpp
	gcc -std=c++20 -O3 -fno-math-errno -fno-trapping-math -c engine_libs.cpp -o engine_libs.o -I../../src/ -I../../libraries/
	
clean:
	del engine_libs.o