import os

os.system("cls")

filenames = "main.c utils.c shader.c triangle.c cube.c math.c glad.c"

command = "gcc -IC:\\Users\\akash\\Documents\\Programming\\OpenGL_C\\glfw\\include -IC:\\Users\\akash\\Documents\\Programming\\OpenGL_C\\glad\\include -LC:\\Users\\akash\\Documents\\Programming\\OpenGL_C\\glfw\\lib -lglfw3 " + filenames + " -o main.exe"

os.system(command)
