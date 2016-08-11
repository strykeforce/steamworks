RepoRoot := $(shell git rev-parse --show-toplevel)
include $(RepoRoot)/Shared_Libs/Homemade_Libs/MakefileHeader

Default: src/Main Upload
	ssh admin@10.27.67.2
Upload: src/Main
	(echo "ls /home/admin" && \
	echo "put src/Main /home/admin/"$(parentname) && \
	echo "chmod 777 /home/admin/"$(parentname)) | \
	sftp admin@10.27.67.2
src/Main: src/*.cpp $(SharedLibs_RIO)
	arm-frc-linux-gnueabi-g++ -Wno-write-strings -std=c++1y $(libs_rio) src/Main.cpp -o src/Main -lwpi -lnavx_frc_cpp -O1
	git add src/*
depends:
	make -C ../JerrysGrapher_RIO
