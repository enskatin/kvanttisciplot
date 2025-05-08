CC = gcc
PKG_CONFIG = pkg-config
CFLAGS = -g $(shell $(PKG_CONFIG) --cflags gtk4)
LDFLAGS = $(shell $(PKG_CONFIG) --libs gtk4)
TARGET = kvanttisciplot1
SRC = kvanttisciplot.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)