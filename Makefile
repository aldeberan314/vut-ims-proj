CC=g++ -DDEBUG -g



all: ims clean_objs

ims: model.o
	$(CC) -o ims model.o

model.o: model.cpp
	$(CC) -c model.cpp

clean_objs:
	rm *.o

run: all
	./ims



clean:
	rm -f ims *.o

