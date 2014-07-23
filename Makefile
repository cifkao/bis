CC=cc

bis: main.o bwt.o rle.o mtf.o huffman.o util.o fileutil.o
	cc -o bis main.o bwt.o rle.o mtf.o huffman.o util.o fileutil.o

main.o: main.c main.h bwt.h rle.h mtf.h huffman.h util.h fileutil.h

bwt.o: bwt.c bwt.h util.h

rle.o: rle.c rle.h util.h

mtf.o: mtf.c mtf.h util.h

huffman.o: huffman.c huffman.h util.h

util.o: util.c util.h main.h

fileutil.o: fileutil.c fileutil.h

clean:
	rm bis *.o
