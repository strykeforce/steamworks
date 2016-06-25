RepoRoot := $(shell git rev-parse --show-toplevel)
include $(RepoRoot)/Static_Libs/Homemade_Libs/MakefileHeader

Default: src/Main Upload
	ssh admin@roboRIO-2767-frc.local
Upload: src/Main
	(echo "ls /home/admin" && \
	echo "put src/Main /home/admin/"$(parentname) && \
	echo "chmod 777 /home/admin/"$(parentname)) | \
	sftp admin@roboRIO-2767-frc.local
src/Main: src/*.cpp $(SharedPath)/*.cpp
	arm-frc-linux-gnueabi-g++ -std=c++1y $(libs) src/Main.cpp Main.o -o src/Main -lwpi -lnavx_frc_cpp -O1
	git add src/*
