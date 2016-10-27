CC=g++
CPP_FLAGS+=-std=c++11
CPP_FLAGS+=-g
# CPP_FLAGS+=-O3
EXES=l1 l2 l3 l4 l5

t1: l1
	PROG=l1 tests/t1

t2: l2
	PROG=l2 tests/t2

%: %.cc
	$(CC) $(CPP_FLAGS) $< -o $@

clean:
	rm -fr $(EXES) *.dSYM
