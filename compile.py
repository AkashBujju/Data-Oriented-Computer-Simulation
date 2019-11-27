import os
import sys

os.system("cls")
os.chdir("src")

command = ""
compiler_not_found_str = "Pass a compiler as argument - gcc(MinGW) | vc(VisualC++)";

if len(sys.argv) != 2:
    print("ScriptError: \n ----- " + compiler_not_found_str + "\n")
    sys.exit()

if sys.argv[1] == "gcc":
    print("gcc ----- Not available for now!!\n")
    sys.exit()
    cc = "gcc"
    filenames = "*.c"
    #filenames = "main.c utils.c shader.c triangle.c cube.c cube_uv.c math.c rectangle.c grid.c line.c"
    external_files = "..\\external\\stb_image.c ..\\external\\glad.c"
    includes = "-I..\\external\\glfw\\mingw\\include -I..\\external\\glad\\include"
    libs_dirs = "-L..\\external\\glfw\\mingw\\lib"
    libs = "-lglfw3"
    #args = "-O2 -ffast-math"
    args = "-Wreturn-type -Wall"
    output = "-o ..\\bin\\gcc\\main.exe"
    command = cc + " " + args + " " + filenames + " " + external_files + " " + includes + " " + libs_dirs + " " + libs + " " + output
    os.system(command)
elif sys.argv[1] == "vc":
    cc = "cl"
    filenames = "*.cpp"
    external_files = "..\\external\\stb_image.c ..\\external\\glad.c"
    includes = "/I..\\external\\glfw\\vc\\x64\\include /I..\\external\\glad\\include /I..\\external\\easy_profiler\\include"
    libs_dirs = "/link /LIBPATH:..\\external\\glfw\\vc\\x64\\lib /LIBPATH:..\\external\\easy_profiler\\lib"
    libs = "gdi32.lib opengl32.lib kernel32.lib user32.lib shell32.lib glfw3.lib easy_profiler.lib"
    args = "/DBUILD_WITH_EASY_PROFILER /MD"
    output = "/out:..\\bin\\vc\\main.exe"
    command = cc + " " + args + " " + filenames + " " + external_files + " " + includes + " " + libs_dirs + " " + libs + " " + output
    os.system(command)
    os.system("del *.obj")
else:
    print("ScriptError: \n ----- " + compiler_not_found_str + "\n")
    sys.exit()
