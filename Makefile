all: libradamsa.c radamsa.h
	$(CC) -O3 -I $(PWD) -o libradamsa.a -c libradamsa.c

gen: gen.c
	cc -O2 -I $(PWD) -o gen gen.c libradamsa.a 

clean:
	rm -f libradamsa.a 
