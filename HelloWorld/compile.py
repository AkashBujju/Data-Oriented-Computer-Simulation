import os

os.system("cls")

filenames = "main.c utils.c shader.c triangle.c cube.c rect.c math.c stb_image.c glad.c"

command = "gcc -O2 -ffast-math -IC:\\Users\\akash\\Documents\\Programming\\OpenGL_C\\glfw\\include -IC:\\Users\\akash\\Documents\\Programming\\OpenGL_C\\glad\\include -LC:\\Users\\akash\\Documents\\Programming\\OpenGL_C\\glfw\\lib -LC:\\Users\\akash\\Documents\\Programming\\OpenGL_C\\soil\\lib -IC:\\Users\\akash\\Documents\\Programming\\OpenGL_C\\soil\\src -lopengl32 -lgdi32 -lglfw3 -lSOIL " + filenames + " -o main.exe"

os.system(command)
