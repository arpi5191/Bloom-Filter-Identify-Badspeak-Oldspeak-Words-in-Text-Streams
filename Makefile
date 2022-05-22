CC=clang
CFLAGS=-Werror -Wall -Wextra -Wpedantic
TARGET=banhammer
LFLAGS=-lm

OBJECTS=banhammer.o parser.o speck.o node.o ht.o bf.o bv.o bst.o
HEADERS=parser.h salts.h speck.h node.h messages.h ht.h bf.h bv.h bst.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
				$(CC) $(LFLAGS) $^ -o $@

%.o: %.c %.h
				$(CC) $(CFLAGS) -c $<

tidy:
				$(RM) *.o

format:
				clang-format -i -style=file *.[ch]

clean: tidy
			  $(RM) $(TARGET)

