# LABSO2022-1


# target folder (default: current folder):
DEST=.

# name of binary:
BIN=coda

# source to compile:
SRC=main.c

# - print informations
# - check for target folder and compile inside, rising error otherwise
# - possibly move to destination (note: if in current folder rises an error, but it's ignored) then dump content of folder
$(DEST)/$(BIN): $(SRC)
	@[ -d $(DEST) ] && gcc -std=gnu90 -o $@ main.c || echo "?ERROR" 1>&2
	
