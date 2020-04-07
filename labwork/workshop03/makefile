task1: task1.c
	gcc task1.c -o task1 -std=c99 -Wall

task2a: task2a.c
	gcc task2a.c -o task2a -std=c99 -Wall

task2b: task2b.c
	gcc task2b.c -o task2b -std=c99 -Wall

task3a: task3a.c
	gcc task3a.c -o task3a -std=c99 -Wall

task3b: task3b.c
	gcc task3b.c -o task3b -std=c99 -Wall

task4a: task4a.c
	gcc task4a.c -o task4a -std=c99 -Wall

task4b: task4b.c
	gcc task4b.c -o task4b -std=c99 -Wall

test5: task5
	./task5

clean5:
	rm task5 task5.o reverse.o

task5: reverse.o task5.o 
	gcc -o task5 task5.o reverse.o -Wall

task5.o: task5.c ctap.h reverse.h
	gcc -c -o task5.o task5.c -Wall

reverse.o: reverse.c reverse.h
	gcc -c -o reverse.o reverse.c -Wall

