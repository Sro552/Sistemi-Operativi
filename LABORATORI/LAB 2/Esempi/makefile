sintassi:
	#commento
	
	echo Visualizzato ed eseguito
	@echo solo eseguito
target1: target2
	@echo '[1] Entering target 1'	
	@echo '[1] rule (3)'
	@echo '[1] rule (4)'
target2: target3
	@echo '[2] Entering target 2'
	@echo '[2] rule (2)'
target3:
	@echo '[3] Entering target 3'
	@echo '[3] rule (1)'

ind:
	@pwd
	cd ../..
	@pwd
	cd ../.. ; pwd

build: deployable.out
	@echo finished building file
deployable.out: main.o
	gcc $< -o $@
%.o: %.s
	gcc -c $< -o $@


main.s: main.i
	gcc -S main.i -o main.s
main.i: main.c
	gcc -E main.c -o main.i	

ONCE:=hello $(LATER)
EVERY=hello $(LATER)
LATER=world

vars:
	#echo 'ONCE'
	@echo $(ONCE)
	#echo 'EVERY'
	@echo $(EVERY)

specials:
	#eval LATER+="forever"
	$(eval LATER+="forever")
	#echo EVERY
	@echo $(EVERY)
	LATER+="and ever"
	#echo EVERY
	@echo $(EVERY)

clean:
	rm *.i *.s *.o *.out
.PHONY: clean build ind vars target3 target2 target1 sintassi
#.INTERMEDIATE: main.i main.s

