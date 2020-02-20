all: httpd test

LIBS = -lpthread

httpd: httpd.c
	gcc -g -W -Wall -o $@ $< $(LIBS)
test: test.c
	gcc -g -W -Wall -o $@ $< 

clean: 
	rm httpd test
