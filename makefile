

objects = logstr.o logmsg.o tick.o dict.o snprintf.o testmain.o catagory.o config.o layout.o logger.o\
	tinylog.o

all:$(objects)


$(objects): %.o:%.c
	gcc -c $(CFLAGS) $< -o $@

