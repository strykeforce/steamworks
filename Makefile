#
# Makefile for deadeye
#

CC := g++
SRCDIR := src
BUILDDIR := build
BINDIR := bin
TARGET := $(BINDIR)/deadeye

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -c -std=c++11 -g -Wall
LDFLAGS := -lstdc++
INC := -Ivendor/spdlog/include -Iinclude

# link to OpenCV
CFLAGS += `pkg-config --cflags opencv`
LDFLAGS += `pkg-config --libs opencv`


$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@mkdir -p $(BINDIR)
	@echo " $(CC) $^ -o $(TARGET) $(LDFLAGS)"; $(CC) $^ -o $(TARGET) $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean
