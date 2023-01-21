solver: solver.c
	gcc solver.c -o solver

.PHONY: clean

clean:
	rm solver
