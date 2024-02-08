
#compiler = gcc -c -std=c++20 -I../../tram-sdk/src/ -I../../tram-sdk/libraries/ -Ilibraries
#compilerlibsloc = -L../../tram-sdk/ -L../../tram-sdk/libraries/binaries/win64/ -Llibraries
#compilerlibs = -ltramsdk -lglfw3 -lgdi32 -lopengl32 -lOpenAl32 -lBulletDynamics -lBulletCollision -lLinearMath -llua

#default: main.o
	#g++ -std=c++20 -o poo main.o $(compilerlibsloc) $(compilerlibs)
	#g++ -std=c++20 -o poo main.o $(compilerlibsloc) $(compilerlibs)

default: engine_libs trad

trad: main.cpp
	g++ -std=c++20 main.cpp engine_libs.o -o trad.exe -I../../src/ -I../../libraries/

engine_libs: engine_libs.cpp
	gcc -std=c++20 -c engine_libs.cpp -o engine_libs.o -I../../src/ -I../../libraries/
	
clean:
	del main.o


	