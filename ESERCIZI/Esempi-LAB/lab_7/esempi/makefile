C_SOURCES:=$(wildcard *.c)

.PHONY: $(C_SOURCES) clean

$(C_SOURCES):
	@gcc $@ -o $(basename $@)
