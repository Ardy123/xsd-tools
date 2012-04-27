CC :=gcc
RM :=rm
TEST := testA014
CFLAGS := -g -I. -I./$(TEST)-xsdb
VPATH = ./$(TEST)-xsdb

.SUFFIXES: 

all: $(TEST)

$(TEST): $(TEST)-bin.o xmlb_$(TEST).o
	$(CC) -o $@ $^ $(CFLAGS)

$(TEST)-bin.o: $(TEST)-bin.c

xmlb_$(TEST).o: xmlb_$(TEST).c

$(TEST)-bin.c: xmlb_common.h xmlb_$(TEST).h

xmlb_$(TEST).c: xmlb_common.h xmlb_$(TEST).h

clean:
	-$(RM) $(TEST) $(TEST)-bin.o xmlb_$(TEST).o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
 
