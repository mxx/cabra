
test:	*.cpp sqlite3.o
	$(CXX) -ggdb -I../sqlite3 -L . -o db_test $^ -lpthread -ldl 

sqlite3.o:	../sqlite3/sqlite3.c
	gcc -c -o sqlite3.o $^
