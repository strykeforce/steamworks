CC = g++
CFLAGS = -c -Wall -std=c++11 -g
LDFLAGS = -lstdc++

SOURCES = NetworkPorts.cpp NVIDIAUtils.cpp UDPHandler.cpp Main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = deadeye

CFLAGS += `pkg-config --cflags opencv`
LDFLAGS += `pkg-config --libs opencv`

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
