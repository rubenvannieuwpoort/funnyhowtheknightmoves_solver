// solves https://www.funnyhowtheknightmoves.com/ in 158 steps
// compile with gcc solver.c -o solver

#include <stdio.h>    // printf
#include <string.h>   // memset
#include <assert.h>   // assert
#include <stdbool.h>  // true, bool
#include <stdint.h>   // uint8_t, uint64_t
#include <stdlib.h>   // abs

#define X(p) ((p) & 7)
#define Y(p) ((p) >> 3)

#define POS(x, y) (((y) << 3) + (x))

uint64_t pos1, pos2;  // bit maps to indicate if position is occupied
uint8_t lastpos[64];  // map of indices to last position, to backtrace solution

bool is_valid(int x, int y) {
	// x and y should be within 0..7 and not reachable by the queen
	return 0 <= x && x <= 7 && 0 <= y && y <= 7 && (abs(x - 4) != abs(y - 3)) && x != 4 && y != 3;
}

char buf[2];
char *to_human_readable(uint8_t p) {
	buf[0] = 'h' - X(p);
	buf[1] = '8' - Y(p);
	return &buf[0];
}

// offsets for a knight's jump
int offset_x[8] = { -1, -2, -2, -1, 1, 2,  2,  1 };
int offset_y[8] = { -2, -1,  1,  2, 2, 1, -1, -2 };

void print_sequence(int root_x, int root_y, int target_x, int target_y) {
	// clear data
	memset(lastpos, 0, 64);
	uint64_t *currentpos = &pos1, *nextpos = &pos2;

	*currentpos = ((uint64_t)1) << ((target_y << 3) + target_x);
	uint8_t rp = POS(target_x, target_y);
	lastpos[rp] = 255;

	// breadth-first search from target to root (it's inversed so that we can
	// backtrack & print in one go without needing to reverse the solution)
	while (true) {
		*nextpos = 0;

		for (int idx = 0; idx < 64; idx++) {
			int xx = idx & 7, yy = idx >> 3;

			if (*currentpos & (((uint64_t)1) << idx)) {
				// this will visit only positions marked in the bit map
				int cx = idx & 7, cy = idx >> 3;

				// loop over positions reachable by a knight's jump
				for (int i = 0; i < 8; i++) {
					int nx = xx + offset_x[i], ny = yy + offset_y[i];

					if (nx == root_x && ny == root_y) {
						// we have reached the target, print solution
						uint8_t p = POS(cx, cy);
						while (p != rp) {
							printf("%s\n", to_human_readable(p));
							p = lastpos[p];
						}
						return;
					}

					// mark reachable and valid positions in nextpos
					if (is_valid(nx, ny) && !lastpos[(ny << 3) + nx]) {
						lastpos[(ny << 3) + nx] = POS(cx, cy);
						*nextpos |= ((uint64_t)1) << (ny << 3) + nx;
					}
				}
			}
		}

		// swap bit maps for current and next occupied positions
		uint64_t *temp = currentpos;
		currentpos = nextpos;
		nextpos = temp;
	}
}

int main(int argc, char *argv[]) {
	int cx = 0, cy = 0, tx, ty;  // current position, next target
	for (uint8_t i = 1; i < 64; i++) {
		int x = X(i), y = Y(i);
		if (is_valid(x, y)) {
			tx = x;
			ty = y;
			printf("\t%s\n", to_human_readable(POS(cx, cy))); // targets with indent
			print_sequence(cx, cy, tx, ty); // print path to next target
			cx = tx;
			cy = ty;
		}
	}
	printf("\ta1\n");
	return 0;
}
