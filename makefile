main:
	g++ -std=c++11 Futoshiki-Solve.cpp Futoshiki-Solve.h -o futoshiki-solver.out
clean:
	rm -f futoshiki-solver.out results.csv

