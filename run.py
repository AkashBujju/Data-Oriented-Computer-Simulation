import os
import sys

if len(sys.argv) != 2:
    print("Error: \n ----- Pass the specific compiler output to run - gcc | vc\n");
    sys.exit()

arg = sys.argv[1]

if arg == "gcc":
    os.chdir("bin\\gcc")
elif arg == "vc":
    os.chdir("bin\\vc")
else:
    print("Error: \n ----- Pass the specific compiler output to run - gcc | vc\n");
    sys.exit()

os.system("main.exe fullscreen")
