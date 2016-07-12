RepoRoot := $(shell git rev-parse --show-toplevel)
include $(RepoRoot)/Static_Libs/Homemade_Libs/MakefileHeader

Default: src/Main Upload
	ssh admin@10.27.67.2
Upload: src/Main
	(echo "ls /home/admin" && \
	echo "put src/Main /home/admin/"$(parentname) && \
	echo "chmod 777 /home/admin/"$(parentname)) | \
	sftp admin@10.27.67.2
src/Main: src/*.cpp $(SharedPath)/*.cpp JerrysGrapher_RIO.o
	arm-frc-linux-gnueabi-g++ -std=c++1y $(libs) src/Main.cpp JerrysGrapher_RIO.o -o src/Main -lwpi -lnavx_frc_cpp -O1
	git add src/*
