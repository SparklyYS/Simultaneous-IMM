
test: test.o graph_reader.o
	g++ test.o graph_reader.o -o test

test.o:	test.cpp graph_reader.h
	g++ -c test.cpp -o test.o

graph_reader.o: graph_reader.cpp graph_reader.h
	g++ -c graph_reader.cpp -o graph_reader.o

clean:
	rm -rf *.o test