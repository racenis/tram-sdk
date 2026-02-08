
# generic compiler options
compiler = g++ -std=c++20 -I../src/ -I../libraries/glfw3/ -I../libraries/
libraries = -L../libraries/binaries/win64/ $\
	-lglfw3 -lgdi32 -lopengl32

# compile all of the tests
compile: core uid event aabb_tree hashmap octree pool queue stack stackpool

# run all of the tests
test: compile
	core
	uid
	event
	
	aabb_tree
	hashmap
	octree
	pool
	queue
	stack
	stackpool
	
clean:
	del core.exe
	del uid.exe
	del event.exe
	
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