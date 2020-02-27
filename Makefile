all: hw2

CC = g++
INSTDIR = /usr/local/bin
CFlAGS = -w
LD_PRLOAD:=$(PWD)/sandbox.so

hw2: sandbox.so
    #LD_PRLOAD:=$(PWD)/sandbox.so
	$(CC) $(CFlAGS) -g main.cpp ./sandbox.so -o sandbox -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC

sandbox.so: 
	$(CC) -w -o sandbox.so -shared -fPIC sandbox.cpp -D_GNU_SOURCE -ldl

.PHONY: clean
clean:
	rm *.so