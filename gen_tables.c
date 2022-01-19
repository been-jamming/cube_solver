#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cube.h"

static int count_bits16(uint16_t num){
	num = (num&0x5555) + ((num&0xAAAA)>>1);
	num = (num&0x3333) + ((num&0xCCCC)>>2);
	num = (num&0x0F0F) + ((num&0xF0F0)>>4);
	num = (num&0x00FF) + ((num&0xFF00)>>8);

	return num;
}

static int count_bits8(uint8_t num){
	num = (num&0x55) + ((num&0xAA)>>1);
	num = (num&0x33) + ((num&0xCC)>>2);
	num = (num&0x0F) + ((num&0xF0)>>4);

	return num;
}

static unsigned int get_corner_permutation_index(rubiks_cube *cube){
	uint8_t occupied = 0;
	unsigned int factorial = 40320;
	unsigned int index = 0;
	int i;
	int count;

	for(i = 0; i < 8; i++){
		factorial /= 8 - i;

		count = count_bits8((~occupied)&((1<<cube->corner_permutation[i]) - 1));
		index += count*factorial;
		occupied |= 1<<cube->corner_permutation[i];
	}

	return index;
}

static unsigned int get_corner_orientation_index(rubiks_cube *cube){
	unsigned int index = 0;
	int i;

	for(i = 6; i >= 0; i--){
		index *= 3;
		index += cube->corner_orientation[i];
	}

	return index;
}

static uint32_t get_corner_index(rubiks_cube *cube){
	uint32_t permutation_index;
	uint32_t orientation_index;

	permutation_index = get_corner_permutation_index(cube);
	orientation_index = get_corner_orientation_index(cube);
	return permutation_index*2187 + orientation_index;
}

static void gen_corner_table(rubiks_cube *cube, int cost, int count, rubiks_move last_move, unsigned char *table){
	int next_cost;
	int next_count;
	uint32_t corner_index;
	int i;

	if(cost > 12){
		return;
	}

	corner_index = get_corner_index(cube);
	if(table[corner_index] <= cost){
		return;
	}
	table[corner_index] = cost;

	for(i = 0; i < 6; i++){
		if(last_move == DOWN && i == UP){
			continue;
		}
		if(last_move == RIGHT && i == LEFT){
			continue;
		}
		if(last_move == BACK && i == FRONT){
			continue;
		}
		if(i == last_move){
			if(count < 3){
				next_count = count + 1;
				next_cost = cost;
			} else {
				continue;
			}
		} else {
			next_count = 1;
			next_cost = cost + 1;
		}
		make_move(cube, i);
		gen_corner_table(cube, next_cost, next_count, i, table);
		undo_move(cube, i);
	}
}

void print_moves(rubiks_cube *cube, unsigned char *table){
	char *names[18] = {"F", "B", "L", "R", "U", "D", "F2", "B2", "L2", "R2", "U2", "D2", "F'", "B'", "L'", "R'", "U'", "D'"};
	int i;
	int moves;
	uint32_t corner_index;

	corner_index = get_corner_index(cube);
	moves = table[corner_index];
	if(moves == 0){
		return;
	}

	for(i = 0; i < 6; i++){
		make_move(cube, i);
		corner_index = get_corner_index(cube);
		if(table[corner_index] < moves){
			printf("%s", names[i]);
			print_moves(cube, table);
			return;
		}
		undo_move(cube, i);
	}
	for(i = 0; i < 6; i++){
		make_move(cube, i);
		make_move(cube, i);
		corner_index = get_corner_index(cube);
		if(table[corner_index] < moves){
			printf("%s", names[i + 6]);
			print_moves(cube, table);
			return;
		}
		undo_move(cube, i);
		undo_move(cube, i);
	}
	for(i = 0; i < 6; i++){
		make_move(cube, i);
		make_move(cube, i);
		make_move(cube, i);
		corner_index = get_corner_index(cube);
		if(table[corner_index] < moves){
			printf("%s", names[i + 12]);
			print_moves(cube, table);
			return;
		}
		undo_move(cube, i);
		undo_move(cube, i);
		undo_move(cube, i);
	}
	printf("WHAT\n");
}

void load_table(unsigned char *table){
	FILE *fp;

	fp = fopen("corner_table.dat", "rb");
	fread(table, sizeof(char), 88179840, fp);
	fclose(fp);
}

int main(int argc, char **argv){
	rubiks_cube *cube;
	unsigned char *table;
	int i;
	int max_cost;
	FILE *fp;
	float sum = 0;

	table = malloc(sizeof(unsigned char)*88179840);
	if(!table){
		fprintf(stderr, "Error: could not allocate table.\n");
		return 1;
	}
	memset(table, 255, 88179840);
	cube = create_cube();
	//gen_corner_table(cube, 0, 0, -1, table);
	load_table(table);
	make_move(cube, LEFT);
	make_move(cube, UP);
	make_move(cube, UP);
	undo_move(cube, LEFT);
	undo_move(cube, FRONT);
	make_move(cube, RIGHT);
	undo_move(cube, UP);
	make_move(cube, BACK);
	make_move(cube, UP);
	make_move(cube, BACK);
	undo_move(cube, LEFT);
	make_move(cube, DOWN);
	make_move(cube, RIGHT);
	print_moves(cube, table);
	printf("\n");
	max_cost = 0;
	for(i = 0; i < 88179840; i++){
		if(table[i] > max_cost && table[i] != 255){
			max_cost = table[i];
		}
	}
	printf("%d\n", max_cost);
	//fp = fopen("corner_table.dat", "wb");
	//fwrite(table, sizeof(unsigned char), 88179840, fp);
	//fclose(fp);
	for(i = 0; i < 88179840; i++){
		sum += table[i];
	}
	printf("%f\n", sum/88179840);
	free(cube);
	free(table);

	return 0;
}
