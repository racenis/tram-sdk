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

compiler = config["Make"]["compiler"]
linker = config["Make"]["linker"]
archiver = config["Make"]["archiver"]

modules = config["Modules"]

deleter = "del" if is_on_windows else "rm"
binaries = "libraries/binaries/win64/" if is_on_windows else "libraries/binaries/linux64/"

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
	print("Linker:\t\t", linker)
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
	
	for unit in units:
		makefile += objectify(unit) + ".o: " + unit + "\n"
		makefile += "\t" + compiler + " -c -g -O0 -std=c++20 -Ilibraries -I./src " + unit
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
		
	if not is_library:
		makefile += "project: "
		for unit in units:
			makefile += objectify(unit) + ".o "
		makefile += "\n\t" + linker + " -o " + project_name
		makefile += " -L" + binaries + " "
		for unit in units:
			makefile += objectify(unit) + ".o "
		makefile += "-lBulletSoftBody "
		makefile += "-lBulletDynamics "
		makefile += "-lBulletCollision "
		makefile += "-lLinearMath "
		makefile += "-lglfw3 "
		makefile += "-lOpenAL32 "
		makefile += "-ltramsdk\n"
	
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






	