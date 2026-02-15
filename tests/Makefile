
# generic compiler options
compiler = g++ -std=c++20 -I../src/ -I../libraries/glfw3/ -I../libraries/
libraries = -L../libraries/binaries/win64/ $\
	-lglfw3 -lgdi32 -lopengl32

.PHONY: compile test test_all clean

# compile all of the tests
compile: core uid event value file settings aabb_tree hashmap octree pool queue stack stackpool

# run all of the tests
test:
	core
	uid
	event
	value
	file
	settings
	
	aabb_tree
	hashmap
	octree
	pool
	queue
	stack
	stackpool

# run all tests until end, even if one fails
test_all:
	-core
	-uid
	-event
	-value
	-file
	-settings
	
	-aabb_tree
	-hashmap
	-octree
	-pool
	-queue
	-stack
	-stackpool
	
clean:
	del core.exe
	del uid.exe
	del event.exe
	del value.exe
	del file.exe
	del settings.exe
	
	del aabb_tree.exe
	del hashmap.exe
	del octree.exe
	del pool.exe
	del queue.exe
	del stack.exe
	del stackpool.exe
	
# tests themselves

core:
	$(compiler) core.cpp -o core $(libraries)
uid:
	$(compiler) uid.cpp -o uid $(libraries)
event:
	$(compiler) event.cpp -o event $(libraries)
value:
	$(compiler) value.cpp -o value $(libraries)
file:
	$(compiler) file.cpp -o file $(libraries)
settings:
	$(compiler) settings.cpp -o settings $(libraries)
	
aabb_tree:
	$(compiler) aabb_tree.cpp -o aabb_tree $(libraries)
hashmap:
	$(compiler) hashmap.cpp -o hashmap $(libraries)
octree:
	$(compiler) octree.cpp -o octree $(libraries)
pool:
	$(compiler) pool.cpp -o pool $(libraries)
queue:
	$(compiler) queue.cpp -o queue $(libraries)
stack:
	$(compiler) stack.cpp -o stack $(libraries)
stackpool:
	$(compiler) stackpool.cpp -o stackpool $(libraries)