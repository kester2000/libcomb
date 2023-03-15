all:
	make -C comb -f makefile
	mv comb/libcomb.so .
	g++ -o main test.cpp -L. -lcomb -lpthread

release:
	g++ -o main test.cpp comb/comb.cpp -lpthread

debug:
	g++ -o main test.cpp comb/comb.cpp -lpthread -g