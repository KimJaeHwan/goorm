all: 1-s-main.o 2-s-input.o 3-c-main.o 4-c-input.o

1-s-main.o: ./user1_sc/1-s-main.c
	gcc -o ./user1_sc/1-s-main.o ./user1_sc/1-s-main.c

2-s-input.o: ./user1_sc/2-s-input.c
	gcc -o ./user1_sc/2-s-input.o ./user1_sc/2-s-input.c

3-c-main.o: ./user2_sc/3-c-main.c
	gcc -o ./user2_sc/3-c-main.o ./user2_sc/3-c-main.c

4-c-input.o: ./user2_sc/4-c-input.c
	gcc -o ./user2_sc/4-c-input.o ./user2_sc/4-c-input.c

clean:
	rm ./user2_sc/*.o
	rm ./user1_sc/*.o
	
