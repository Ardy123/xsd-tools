CC ?=gcc
RM ?=rm
TEST := testA014
CFLAGS ?= -Wall -Wextra -Werror -g -I. -I./$(TEST)-xsdb -I../libb64/include/
VPATH = ./$(TEST)-xsdb

.SUFFIXES: 

all: $(TEST)

$(TEST): $(TEST)-bin.o xml_$(TEST).o ../libb64/src/libb64.a
	$(CC) -o $@ $^ $(CFLAGS) -lexpat

../libb64/src/libb64.a:
	$(MAKE) -C ../libb64/ -f Makefile

$(TEST)-bin.o: $(TEST)-bin.c

xml_$(TEST).o: xml_$(TEST).c

$(TEST)-bin.c: xml_common.h xml_$(TEST).h

xml_$(TEST).c: xml_common.h xml_$(TEST).h

clean:
	-$(RM) $(TEST) $(TEST)-bin.o xml_$(TEST).o
	-$(MAKE) -s -C ../libb64/ -f Makefile clean

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
 
