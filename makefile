 
CC := 		g++ # This is the main compiler
SRCDIR := 	src
BUILDDIR := 	build
TARGET := 	bin/libzuazo.so
 
SRCEXT := 	cpp
SOURCES := 	$(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := 	$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=))
CFLAGS :=	-Wall -Wextra -std=c++17 -g -fPIC
LDFLAGS := 	-shared 
LIB := 		-lpthread -lGL -lGLU -lGLEW -lglfw -lavformat -lavcodec -lavutil -lswscale -ltinyxml2 -lfreetype -ljpeg
INC := 		-Iinclude -I/usr/include/freetype2

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	$(CC) $^ $(LDFLAGS) -o $(TARGET) $(LIB)
	
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo "Compiling..."
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo "Cleaning..."; 
	$(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean
