FILES=$(subst .c,.out,$(wildcard *.c))

all: $(FILES)
	@echo all files built
	@chmod +x $(FILES)

%.out: %.c
	@echo building files $<
	@gcc $< -o $@

clean:
	-rm *.out *.o

	
#build: msgDispatcher.c
#@gcc msgDispatcher.c -o msgDispatcher.out
