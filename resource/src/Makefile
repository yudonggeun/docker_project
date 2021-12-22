
# made by yudonggeun 2017154022
# 
# json

CC=gcc
CFLAGS=-Wall -O2 -s
LDIR=-L/path/to/json_c/install/lib
LDBS= -lpthread $(LDIR) -ljson-c
SRCS= resource.c extract_info.c 
OBJS=$(SRCS:.c=.o)
TARGET=resource

all: $(TARGET)

clean:
	rm -f *.o
	rm -f $(TARGET)

$(TARGET): $(OBJS)
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
	$(CC) -o $@ $ $(OBJS) $(CFLAGS) $(LDBS)
