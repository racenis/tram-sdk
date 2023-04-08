import os as os
import sys as sys
import configparser as configparser

# Read the config file
config = configparser.ConfigParser()
if len(config.read("maketool.ini")) != 1:
	sys.exit("Can't read the config file!")

# Extract the configuration variables
project_path = "./"
project_name = config["Project"]["name"]

is_on_windows = os.name == "nt"

is_library = config["Project"]["type"] == "LIBRARY"
is_project = config["Project"]["type"] == "PROJECT"

is_emscripten = config["Project"]["platform"] == "WEB"

compiler = config["Make"]["compiler"]
archiver = config["Make"]["archiver"]
tramsdk = config["Make"]["tramsdk"]

modules = config["Modules"]

deleter = "del" if is_on_windows else "rm"

if is_emscripten:
	binaries = "libraries/binaries/web/"
elif is_on_windows:
	binaries = "libraries/binaries/win64/"
else:
	binaries = "libraries/binaries/linux64/"

# Print the header
def print_header():
	print("TRAM SDK Makefile generator v0.0.4")

# Print help
def print_help():
	print_header()
	print("This tool generates makefiles for compiling the Tramway SDK and")
	print("projects built on top of it. Please check your configuration")
	print("before generating the Makefile for the first time.")
	print("\nCommand line options:")
	print("\t--help\t\tDisplays this message.")
	print("\t--version\tAlso displays this message.")
	print("\tconfig\t\tChecks the configuration.")
	print("\tgenerate\tGenerates a makefile in the project directory.")

# Print config
def print_config():
	print_header()
	print("Compiler:\t", compiler)
	print("Archiver:\t", archiver)
	print("Project type:\t", "LIBRARY" if is_library else "PROJECT")
	print("Modules:")
	for module, include in modules.items():
		print("    ", module, "\t", include)
	print("\nYou can change the configuration in ./maketool.ini")

# Generate makefile
def generate_makefile():
	makefile = ""
	
	if not os.path.isdir(project_path + "src"):
		sys.exit("Can't find the ./src/ directory!")
		
	print("Generating makefile...")
	
	units = []
	source_path = project_path + "src/"
	
	for root, _, files in os.walk(source_path):
		for name in files:
			filename = os.path.join(root, name).replace('\\', '/')
			
			append = filename.endswith(".cpp")
			
			for module, include in modules.items():
				if filename.startswith(source_path + module) and include == "EXCLUDE":
					append = False
			
			if is_library and filename == source_path + "main.cpp":
				append = False
				
			if append:
				units.append(filename)
	
	def objectify(unit):
		unit = unit.replace('./', '')
		unit = unit.replace('/', '_')
		return unit[:-4]
	
	# Generate translation unit rules
	for unit in units:
		makefile += objectify(unit) + ".o: " + unit + "\n"
		makefile += "\t" + compiler + " -c -g -O0 -std=c++20 -I./src"
		if is_emscripten:
			makefile += " -Wno-undefined-var-template"
		if is_library:
			makefile += " -Ilibraries"
			makefile += " -Ilibraries/bullet"
			if not is_emscripten:
				makefile += " -Ilibraries/glad"
				makefile += " -Ilibraries/glfw3"
				makefile += " -Ilibraries/openal"
		if not is_library:
			makefile += " -I" + tramsdk + "/src -I" + tramsdk + "/libraries"
			makefile += " -I" + tramsdk + "/src -I" + tramsdk + "/libraries/bullet"
			if not is_emscripten:
				makefile += " -I" + tramsdk + "/src -I" + tramsdk + "/libraries/glad"
				makefile += " -I" + tramsdk + "/src -I" + tramsdk + "/libraries/glfw3"
				makefile += " -I" + tramsdk + "/src -I" + tramsdk + "/libraries/openal"
		makefile += " " + unit + " -o " + objectify(unit) + ".o\n\n"
	
	# Generate clean rule
	makefile += "clean:\n"
	for unit in units:
		makefile += "\t" + deleter + " " + objectify(unit) + ".o\n"	
	makefile += "\n"
	
	# Generate build rule
	if is_library:
		makefile += "library: "
		for unit in units:
			makefile += objectify(unit) + ".o "
		makefile += "\n\t" + archiver
		if is_emscripten:
			makefile += " -crf "
		else:
			makefile += " rcs "
		makefile += "libtramsdk.a "
		
		
		for unit in units:
			makefile += objectify(unit) + ".o "
		
		makefile += "\n"
		
		#makefile += "libraryfull: library\n"
		#makefile += "\tar -crf tramsdkfull.a tramsdk.a "
		#makefile += binaries + "libBulletSoftBody.a "
		#makefile += binaries + "libBulletDynamics.a "
		#makefile += binaries + "libBulletCollision.a "
		#makefile += binaries + "libLinearMath.a "
		#makefile += binaries + "libglfw3.a "
		#makefile += binaries + "libOpenAL32.a\n"
		
	if not is_library:
		makefile += "project: "
		for unit in units:
			makefile += objectify(unit) + ".o "
		makefile += "\n\t" + compiler
		
		if is_emscripten:
			makefile += " -g "
			for unit in units:
				makefile += objectify(unit) + ".o "
			makefile += "-sASSERTIONS=2 -sSAFE_HEAP=0 -sALLOW_MEMORY_GROWTH "
			makefile += "-sSTACK_OVERFLOW_CHECK=1 -sUSE_BULLET=1 -sUSE_GLFW=3 "
			makefile += "-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -L./ "
			makefile += " -L" + tramsdk + "/" + binaries + " "
			makefile += "-ltramsdk "
			makefile += "-lBulletSoftBody "
			makefile += "-lBulletDynamics "
			makefile += "-lBulletCollision "
			makefile += "-lLinearMath "
			makefile += "-o " + project_name + ".html --preload-file ./"
		else:
			makefile += " -o " + project_name
			makefile += " -L" + tramsdk + "/" + binaries + " "
			makefile += " -L" + tramsdk + " "
			makefile += "-static "
			for unit in units:
				makefile += objectify(unit) + ".o "
			makefile += "-ltramsdk "
			makefile += "-lBulletSoftBody "
			makefile += "-lBulletDynamics "
			makefile += "-lBulletCollision "
			makefile += "-lLinearMath "
			makefile += "-lglfw3 "
			makefile += "-lOpenAL32 "
			makefile += "-lgdi32 "
			makefile += "-lopengl32\n"
	
	with open(project_path + "Makefile", "w") as makefile_file:
		makefile_file.write(makefile)
	
	print("ok.")
	
mode = sys.argv[1] if len(sys.argv) > 1 else -1

if mode == "config":
	print_config()
elif mode == "generate":
	generate_makefile()
else:
	print_help()






	