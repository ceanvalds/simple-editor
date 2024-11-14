CC = gcc

all: compile link cleanup run

compile:
	mkdir -p build
	$(CC) -c src/buf.c -o build/buf.o
	$(CC) -c src/editor.c -o build/editor.o
	$(CC) -c src/entry.c -o build/entry.o
	$(CC) -c src/file.c -o build/file.o
	$(CC) -c src/in.c -o build/in.o
	$(CC) -c src/output.c -o build/output.o
	$(CC) -c src/row.c -o build/row.o

link:
	$(CC) build/buf.o build/editor.o build/entry.o build/file.o build/in.o build/output.o build/row.o -o ued

cleanup:
	rm -rf build/

run:
	./ued
