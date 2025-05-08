# Kääntäjä ja sen liput (Compiler and flags)
CC = gcc
CFLAGS = -Wall -O0 -g `pkg-config --cflags gtk4 glib-2.0`
LDFLAGS = `pkg-config --libs gtk4 glib-2.0` -lm

# Lähdekooditiedostot ja käännettävien tiedostojen nimet (Source files and object files)
SRC = main.c kvanttisciplot.c
OBJ = $(SRC:.c=.o)
TARGET = main

# Rakenna kohde käännetyistä tiedostoista (Build target from object files)
all: $(TARGET)

# Käännä kohde (Compile the target)
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Käännä lähdekooditiedostot objekteiksi (Compile source files to object files)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Poista ylimääräiset tiedostot (Clean up)
clean:
	rm -f $(OBJ) $(TARGET)

# Phony kohteet (Phony targets)
.PHONY: all clean
