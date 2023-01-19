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

	
mode = sys.argv[1] if len(sys.argv) > 1 else -1

if mode == "config":
	print_config()
if mode == "generate":
	generate_makefile()
else:
	print_help()






	