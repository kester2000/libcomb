release:
	g++ -o main test.cpp comb/comb.cpp -lpthread

debug:
	g++ -o main test.cpp comb/comb.cpp -lpthread -g

find:
	g++ -o main find.cpp comb/comb.cpp -lpthread