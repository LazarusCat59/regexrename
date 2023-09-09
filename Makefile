SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

TEST_SRC=$(wildcard tests/*.c)
TESTS=$(patsubst %.c, %, $(TEST_SRC))

TARGET=bin/renameutil

CFLAGS=-Iinclude -O2
LDLIBS=-lpcre2-8

all: bin $(TARGET) cleanobj

dev: CFLAGS+=-fsanitize=undefined -g -Wall -Wextra 
dev: all

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJECTS) $(LDLIBS) -o $(TARGET)

bin:
	mkdir -p bin

.PHONY: tests clean cleanobj

clean:
	rm -rf bin $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

cleanobj:
	rm -rf $(OBJECTS)
