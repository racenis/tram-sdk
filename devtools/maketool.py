#!/usr/bin/python3

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
project_type = config["Project"]["type"]
platform = config["Project"]["platform"]
	
is_library = project_type == "LIBRARY"
is_project = project_type == "PROJECT"

compiler = config["Make"]["compiler"]
archiver = config["Make"]["archiver"]
tramsdk = config["Make"]["tramsdk"]

module_enables = config["Modules"]

deleter = "del" if os.name == "nt" else "rm"

win32_bin = f" -L{tramsdk}libraries/binaries/win32/"
win64_bin = f" -L{tramsdk}libraries/binaries/win64/"
linux_bin = ""
web_bin = f" -L{tramsdk}libraries/binaries/web/"

class Module:
	def __init__(self, name, group, compile_includes, linker_includes, linker_options):
		self.name = name
		self.group = group
		self.compile_includes = compile_includes
		self.linker_includes = linker_includes
		self.linker_options = linker_options
		
		self.enabled = False
	def	enable(self):
		self.enabled = True
	def	disable(self):
		self.enabled = False
	def is_enabled(self):
		return self.enabled
	def get_group(self):
		return self.group
		
modules = {
	"platform/glfw": Module("platform/glfw",
		"platform", 
		{"WEB": "", "WIN32": f" -I{tramsdk}libraries/glfw", "WIN64": f" -I{tramsdk}libraries/glfw", "LINUX": ""},
		{"WEB": "", "WIN32": "", 		"WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "-lglfw3 -lgdi32", "WIN64": "-lglfw3 -lgdi32", "LINUX": "-lglfw"}),
	"platform/sdl": Module("platform/sdl",
		"platform", 
		{"WEB": "", "WIN32": f" -I{tramsdk}libraries/sdl2 -I{tramsdk}libraries/glad", "WIN64": f" -I{tramsdk}libraries/sdl2 -I{tramsdk}libraries/glad", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "-lSDL2.dll -lgdi32 -ld3d9", "WIN64": "-lSDL2.dll -lgdi32 -ld3d9", "LINUX": "-lsdl2"}),
		
	"audio/openal": Module("audio/openal",
		"audio", 
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": win32_bin, "WIN64": win64_bin, "LINUX": ""},
		{"WEB": "", "WIN32": "-lOpenAL32", "WIN64": "-lOpenAL32", "LINUX": "-lopenal"}),
	"audio/spatial": Module("audio/spatial",
		"audio", 
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""}),
	"audio/template": Module("audio/template",
		"audio", 
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""}),
		
	"render/opengl": Module("render/opengl",
		"render", 
		{"WEB": "", "WIN32": f" -I{tramsdk}libraries/glad", "WIN64": f" -I{tramsdk}libraries/glad", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": "-lGL"}),
	"render/direct3d": Module("render/direct3d",
		"render", 
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""}),
	"render/software": Module("render/software",
		"render", 
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""}),
	"render/raytrace": Module("render/raytrace",
		"render", 
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""}),
		
	"physics/bullet": Module("physics/bullet",
		"physics", 
		{"WEB": f" -I{tramsdk}libraries/bullet", "WIN32": f" -I{tramsdk}libraries/bullet", "WIN64": f" -I{tramsdk}libraries/bullet", "LINUX": f" -I/usr/include/bullet"},
		{"WEB": web_bin, "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "-lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath ", "WIN32": "-lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath ", "WIN64": "-lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath ", "LINUX": "-lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath "}),
		
	"extensions/camera": Module("extensions/camera",
		"extensions", 
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""}),
	"extensions/menu": Module("extensions/menu",
		"extensions", 
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""},
		{"WEB": "", "WIN32": "", "WIN64": "", "LINUX": ""}),
	"extensions/scripting/lua": Module("extensions/scripting/lua",
		"extensions", 
		{"WEB": f" -I{tramsdk}libraries/lua", "WIN32": f" -I{tramsdk}libraries/lua", "WIN64": f" -I{tramsdk}libraries/lua", "LINUX": " -I/usr/include/lua5.4"},
		{"WEB": "", "WIN32": "",		"WIN64": "", 		"LINUX": ""},
		{"WEB": "-llua", "WIN32": "-llua",	"WIN64": "-llua",	"LINUX": "-llua5.4"})
}

for module, enable in module_enables.items():
	if not module in modules:
		print(f"Module '{module}' was not found!")
	if enable == "INCLUDE":
		modules[module].enable()
	elif enable == "EXCLUDE":
		modules[module].disable()
	else:
		print(f"Module '{module}' enable state '{enable}' not recognized!")

		
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
	print("Project type:\t", project_type)
	print("Modules:")
	prev_group = None
	for module, include in modules.items():
		if not prev_group is None and include.group != prev_group:
			print(" ")
		prev_group = include.group
		print("\t[X]  " if include.is_enabled() else "\t[ ]  ", module)
	print("\nYou can change the configuration in ./maketool.ini")

def validate_config():
	is_valid = True

	if not os.path.isdir(project_path):
		print(f"Selected path '{project_path}' is not a directory.")
		is_valid = False

	if ' ' in project_name:
		print(f"Project name '{project_name}' contains spaces.")
		is_valid = False
		
	if not platform in ["WEB", "WIN32", "WIN64", "LINUX"]:
		print(f"Plaform '{platform}' not recognized.")
		is_valid = False

	if not project_type in ["LIBRARY", "PROJECT"]:
		print(f"Project type '{project_type}' is not recognized.")
		is_valid = False
		
	
	groups = set(map(lambda mod: mod.group, modules.values()))
	groups_included = {}
	for group in groups:
		groups_included[group] = 0
	for module in modules.values():
		if module.is_enabled():
			groups_included[module.group] += 1
	for group in groups:
		if groups_included[group] == 0 and group != "extensions":
			print(f"At least one module in {group} needs to be enabled.")
			is_valid = False
		if groups_included[group] > 1 and group != "extensions":
			print(f"Only one module {group} in can be enabled.")
			is_valid = False
	
	return is_valid
	
# Generate makefile
def generate_makefile():
	if is_library:
		makefile = "default: library\n\n"
	else:
		makefile = "default: project\n\n"
	
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
				if filename.startswith(source_path + module) and not include.is_enabled():
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
		makefile += "\t" + compiler + " -c -g -O0 -Wno-narrowing -std=c++20 -I./src"
		makefile += " -I" + tramsdk + "libraries"
		makefile += " -I" + tramsdk + "src"
		if platform == "WEB":
			makefile += " -Wno-undefined-var-template"
		for module in modules.values():
			if module.is_enabled():
				makefile += module.compile_includes[platform]
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
		if platform != "WEB":
			makefile += " -crf "
		else:
			makefile += " rcs "
		makefile += "libtramsdk.a "
		
		for unit in units:
			makefile += objectify(unit) + ".o "
		
		makefile += "\n"
		
	if not is_library:
		makefile += "project: "
		for unit in units:
			makefile += objectify(unit) + ".o "
		makefile += "\n\t" + compiler
		
		if platform == "WEB":
			makefile += " -g "
			for unit in units:
				makefile += objectify(unit) + ".o "
			makefile += "-sASSERTIONS=2 -sSAFE_HEAP=0 -sALLOW_MEMORY_GROWTH "
			makefile += "-sSTACK_OVERFLOW_CHECK=1 -sUSE_BULLET=1 -sUSE_GLFW=3 "
			makefile += "-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -L./ "
			makefile += "-o " + project_name + ".html --preload-file ./"
		else:
			makefile += " -o " + project_name
			makefile += " -L" + tramsdk + " "
			if platform != "LINUX":
				makefile += "-static "
			for unit in units:
				makefile += objectify(unit) + ".o "
			if project_name != "tramsdk":
				makefile += "-ltramsdk "
		for module in modules.values():
			if module.is_enabled():
				makefile += module.linker_includes[platform] + " "
				makefile += module.linker_options[platform] + " "
	
	
	with open(project_path + "Makefile", "w") as makefile_file:
		makefile_file.write(makefile)
	
	print("ok.")
	
mode = sys.argv[1] if len(sys.argv) > 1 else -1

if mode == "config":
	print_config()
	print("")
	validate_config()
elif mode == "generate":
	if validate_config():
		generate_makefile()
else:
	print_help()






	
