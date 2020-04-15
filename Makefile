CC = cc

example: example.o exif.o
	$(CC) -o example example.o exif.o

example.o: example.c exif.h
	$(CC) -c example.c

exif.o: exif.c exif.h list.h
	$(CC) -c exif.c

clean:
	rm *.o example
