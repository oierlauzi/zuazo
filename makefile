 
CC := g++ # This is the main compiler
SRCDIR := src
BUILDDIR := build
TARGET := test

SRCEXT     := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS :=-Wall -std=c++17 -g
LIB := -lpthread -lGL -lGLU -lglfw -lavformat -lavcodec -lavutil -lswscale -ltinyxml2 -lfreetype -ldl -lMagick++-6.Q16 -lMagickWand-6.Q16 -lMagickCore-6.Q16

INC := -I/usr/include/freetype2 -I/usr/include/ImageMagick-6/ -I/usr/include/x86_64-linux-gnu/ImageMagick-6/ -Iinclude

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean
