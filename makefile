release:
	g++ -o main test.cpp comb/comb.cpp -lpthread

debug:
	g++ -o main test.cpp comb/comb.cpp -lpthread -g