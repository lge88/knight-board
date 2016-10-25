
l1: l1.cc
	g++ -std=c++11 -g l1.cc -o l1

t1: l1
	tests/t1

clean:
	rm -fr l1 l2 l3 l4 l5 *.dSYM
