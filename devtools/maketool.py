import os as os
import sys as sys
import configparser as configparser

project_path = "../"
config_path = "maketool.ini"

config = configparser.ConfigParser()

if len(config.read(config_path)) != 1:
	project_path = ""
	config_path = "devtools/maketool.ini"
	
	if len(config.read(config_path)) != 1:
		sys.exit("Can't read the config file!")
	
is_on_windows = os.name == "nt"

is_library = config["Project"]["type"] == "LIBRARY"

compiler = config["Make"]["compiler"]
linker = config["Make"]["linker"]
archiver = config["Make"]["archiver"]

deleter = "del" if is_on_windows else "rm"
binaries = "libraries/binaries/win64/" if is_on_windows else "libraries/binaries/linux64/"

def print_header():
	print("TRAM SDK Makefile generator v0.0.4")

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
	print("\tmake\t\tGenerates a makefile and also makes it.")
	
def print_config():
	print_header()
	print("Compiler:\t", compiler)
	print("Linker:\t\t", linker)
	print("Archiver:\t", archiver)
	print("Project type:\t", "LIBRARY" if is_library else "PROJECT")
	print("\nYou can change the configuration in", config_path)

def generate_makefile():
	makefile = ""
	
	if not os.path.isdir(project_path + "src"):
		sys.exit("Can't find the src directory!")
		
	print("Generating makefile...")
	
	units = []
	
	for root, _, files in os.walk(project_path + "src/"):
		for name in files:
			filename = os.path.join(root, name).replace('\\', '/')
			
			append = filename.endswith(".cpp")
			
			if is_library and filename == "src/main.cpp":
				append = False
				
			if append:
				units.append(filename)
	
	def objectify(unit):
		unit = unit.replace('/', '_')
		return unit[:-4]
	
	for unit in units:
		makefile += objectify(unit) + ".o: " + unit + "\n"
		makefile += "\t" + compiler + " -c -O0 -std=c++20 -Ilibraries -I./src " + unit
		makefile += " -o " + objectify(unit) + ".o\n\n"
	
	makefile += "clean:\n"
	for unit in units:
		makefile += "\t" + deleter + " " + objectify(unit) + ".o\n"	
	makefile += "\n"
		
	if is_library:
		makefile += "library: "
		for unit in units:
			makefile += objectify(unit) + ".o "
		makefile += "\n\t" + archiver + " -crf libtramsdk.a "
		
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






	