import os
import sys
import platform

if len(sys.argv) != 2:
    print("Error: \n ----- Pass the specific compiler output to run - gcc | vc\n");
    sys.exit()

arg = sys.argv[1]
opr_sys = platform.system()

if opr_sys == 'Darwin':
    if arg == "gcc":
        os.chdir("bin/gcc")
    elif arg == "vc":
        os.chdir("bin/vc")
    else:
        print("Error: \n ----- Pass the specific compiler output to run - gcc | vc\n");
        sys.exit()
elif opr_sys == 'Windows':
    if arg == "gcc":
        os.chdir("bin\\gcc")
    elif arg == "vc":
        os.chdir("bin\\vc")
    else:
        print("Error: \n ----- Pass the specific compiler output to run - gcc | vc\n");
        sys.exit()

os.system("main.exe fullscreen")
