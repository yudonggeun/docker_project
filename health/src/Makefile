
# made by yudonggeun 2017154022
# 
# health check ping

CC=gcc
CFLAGS=-Wall -O2 -s
LDIR=-L/path/to/json_c/install/lib
LDBS= -lpthread $(LDIR) -ljson-c
SRCS= ping.c getConInfo.c health_check.c
OBJS=$(SRCS:.c=.o)
TARGET=health

all: $(TARGET) clean_o 

clean_o:
	rm -f *.o

clean:
	rm -f *.o
	rm -f $(TARGET)

$(TARGET): $(OBJS)
	export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:/usr/local/lib
	$(CC) -o $@ $(OBJS) $(CFLAGS) $(LDBS)
