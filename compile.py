import os

os.system("cls")
os.chdir("src")

cc = "gcc"
filenames = "main.c utils.c shader.c triangle.c cube.c cube_uv.c math.c"
external_files = "..\\external\\stb_image.c ..\\external\\glad.c"
includes = "-I..\\external\\glfw\\include -I..\\external\\glad\\include"
libs_dirs = "-L..\\external\\glfw\\lib"
libs = "-lglfw3"
args = "-O2 -ffast-math"
output = "-o ..\\bin\\main.exe"

command = cc + " " + args + " " + filenames + " " + external_files + " " + includes + " " + libs_dirs + " " + libs + " " + output
os.system(command)
