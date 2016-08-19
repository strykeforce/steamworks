RepoRoot := $(shell git rev-parse --show-toplevel)
include $(RepoRoot)/Shared_Libs/MakefileHeader

Default: src/Main Upload
	ssh admin@10.27.67.2
Upload: src/Main
	(echo "ls /home/admin" && \
	echo "put src/Main /home/admin/"$(parentname) && \
	echo "chmod 777 /home/admin/"$(parentname)) | \
	sftp admin@10.27.67.2
src/Main: src/*.cpp
	make -C $(SharedPath_RIO_DIR) TerminalEscapes.o
	make -C $(SharedPath_RIO_DIR) JerrysGrapher.o
	make -C $(SharedPath_RIO_DIR) SocketHandler.o
	make -C $(SharedPath_RIO_DIR) CodeUtils.o
	make -C $(SharedPath_RIO_DIR) RIOUtils.o
	arm-frc-linux-gnueabi-g++ -Wno-write-strings -std=c++1y $(libs_rio) $(SharedPath_RIO_DIR)/RIOUtils.o $(SharedPath_RIO_DIR)/CodeUtils.o $(SharedPath_RIO_DIR)/TerminalEscapes.o $(SharedPath_RIO_DIR)/JerrysGrapher.o $(SharedPath_RIO_DIR)/SocketHandler.o src/Main.cpp -o src/Main -lwpi -lnavx_frc_cpp -O1
