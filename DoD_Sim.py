import os

cmd_1 = "start gvim C:\\Users\\Akash\\Documents\\GitHub\\LearningOpenGL-In-C\\src\\main.cpp"
os.system(cmd_1)

os.chdir("C:\\Users\\Akash\\Documents\\GitHub\\LearningOpenGL-In-C")
cmd_2 = "start cmd /k \" \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat\""
os.system(cmd_2)
