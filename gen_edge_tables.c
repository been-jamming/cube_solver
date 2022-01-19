#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "cube.h"
#define TABLE_SIZE 510935040

static int edge_translation[12] = {8, 11, 10, 9, 7, 6, 5, 4, 0, 3, 2, 1};
static unsigned char *edge_table;
static unsigned char *corner_table;

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

static uint32_t get_edge_permutation_index(rubiks_cube *cube){
	uint16_t occupied = 0;
	uint32_t factorial = 3991680;//(12!)/(5!)
	uint32_t index = 0;
	int i;
	int count;

	for(i = 0; i < 6; i++){
		factorial /= 12 - i;

		count = count_bits16((~occupied)&((1<<cube->edge_permutation[i]) - 1));
		index += count*factorial;
		occupied |= 1<<cube->edge_permutation[i];
	}

	return index;
}

static int get_edge_permutation_index2(rubiks_cube *cube){
	uint16_t occupied = 0;
	uint32_t factorial = 3991680;//(12!)/(6!)
	uint32_t index = 0;
	int i;
	int count;

	for(i = 0; i < 6; i++){
		factorial /= 12 - i;

		count = count_bits16((~occupied)&((1<<edge_translation[cube->edge_permutation[edge_translation[i]]]) - 1));
		index += count*factorial;
		occupied |= 1<<edge_translation[cube->edge_permutation[edge_translation[i]]];
	}

	return index;
}

static unsigned int get_edge_orientation_index(rubiks_cube *cube){
	unsigned int index = 0;
	int i;

	for(i = 0; i < 7; i++){
		index *= 2;
		index += cube->edge_orientation[i];
	}

	return index;
}

static unsigned int get_edge_orientation_index2(rubiks_cube *cube){
	unsigned int index = 0;
	int i;

	for(i = 0; i < 7; i++){
		index *= 2;
		index += cube->edge_orientation[edge_translation[i]];
	}

	return index;
}

static uint32_t get_edge_index(rubiks_cube *cube){
	uint32_t permutation_index;
	unsigned int orientation_index;

	permutation_index = get_edge_permutation_index(cube);
	orientation_index = get_edge_orientation_index(cube);
	
	return permutation_index*128 + orientation_index;
}

static uint32_t get_edge_index2(rubiks_cube *cube){
	uint32_t permutation_index;
	unsigned int orientation_index;

	permutation_index = get_edge_permutation_index2(cube);
	orientation_index = get_edge_orientation_index2(cube);
	
	return permutation_index*128 + orientation_index;
}

static void gen_edge_table(rubiks_cube *cube, int cost, int count, rubiks_move last_move, unsigned char *table){
	int next_cost;
	int next_count;
	uint32_t edge_index;
	int i;

	if(cost > 20){
		return;
	}

	edge_index = get_edge_index(cube);
	if(table[edge_index] <= cost){
		return;
	}
	table[edge_index] = cost;

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
		gen_edge_table(cube, next_cost, next_count, i, table);
		undo_move(cube, i);
	}
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

void load_edge_table(){
	FILE *fp;

	fp = fopen("edge_table.dat", "rb");
	fread(edge_table, sizeof(unsigned char), TABLE_SIZE, fp);
	fclose(fp);
}

void load_corner_table(){
	FILE *fp;

	fp = fopen("corner_table.dat", "rb");
	fread(corner_table, sizeof(unsigned char), 88179840, fp);
	fclose(fp);
}

static int max(int a, int b){
	if(a > b){
		return a;
	} else {
		return b;
	}
}

static int min(int a, int b){
	if(a < b){
		return a;
	} else {
		return b;
	}
}

int heuristic(rubiks_cube *cube){
	int h0;
	int h1;
	int h2;

	h0 = edge_table[get_edge_index(cube)];
	h1 = edge_table[get_edge_index2(cube)];
	h2 = corner_table[get_corner_index(cube)];

	return max(max(h0, h1), h2);
}

//Sort the list of 6 values by weights
int sort6(int *weights, int *values, int *output){
	int index0;
	int index1;
	int index2;
	int index3;
	int index4;
	int index5;

	index0 = (weights[0] > weights[1]) + (weights[0] > weights[2]) + (weights[0] > weights[3]) + (weights[0] > weights[4]) + (weights[0] > weights[5]);
	index1 = (weights[1] >= weights[0]) + (weights[1] > weights[2]) + (weights[1] > weights[3]) + (weights[1] > weights[4]) + (weights[1] > weights[5]);
	index2 = (weights[2] >= weights[0]) + (weights[2] >= weights[1]) + (weights[2] > weights[3]) + (weights[2] > weights[4]) + (weights[2] > weights[5]);
	index3 = (weights[3] >= weights[0]) + (weights[3] >= weights[1]) + (weights[3] >= weights[2]) + (weights[3] > weights[4]) + (weights[3] > weights[5]);
	index4 = (weights[4] >= weights[0]) + (weights[4] >= weights[1]) + (weights[4] >= weights[2]) + (weights[4] >= weights[3]) + (weights[4] > weights[5]);
	index5 = 15 - index0 - index1 - index2 - index3 - index4;

	output[index0] = values[0];
	output[index1] = values[1];
	output[index2] = values[2];
	output[index3] = values[3];
	output[index4] = values[4];
	output[index5] = values[5];
}

int search(rubiks_cube *cube, int heuristic_val, int count, rubiks_move last_move, rubiks_move last_distinct, int bound, char *moves, int place){
	char move_names[6] = {'F', 'B', 'L', 'R', 'U', 'D'};
	int move_ids[6] = {0, 1, 2, 3, 4, 5};
	int sorted_moves[6];
	int heuristic_values[6];
	int next_bound;
	int next_count;
	int next_last_distinct;
	int num_moves;
	int i;

	if(heuristic_val == 0){
		printf("%d %s\n", bound, moves);
		return 0;
	}

	if(heuristic_val >= bound){
		return -1;
	}

	for(i = 0; i < 6; i++){
		make_move(cube, i);
		heuristic_values[i] = heuristic(cube);
		undo_move(cube, i);
	}

	sort6(heuristic_values, move_ids, sorted_moves);

	for(i = 0; i < 6; i++){
		if(last_move == FRONT && sorted_moves[i] == BACK){
			continue;
		}
		if(last_move == LEFT && sorted_moves[i] == RIGHT){
			continue;
		}
		if(last_move == UP && sorted_moves[i] == DOWN){
			continue;
		}
		//REMEMBER: figure out what to do with last_distinct
		if(last_move == sorted_moves[i]){
			if(count < 3){
				moves[place] = move_names[sorted_moves[i]];
				moves[place + 1] = '\0';
				make_move(cube, sorted_moves[i]);
				search(cube, heuristic_values[sorted_moves[i]], count + 1, sorted_moves[i], last_distinct, bound, moves, place + 1);
				undo_move(cube, sorted_moves[i]);
			} else {
				continue;
			}
		} else {
			moves[place] = move_names[sorted_moves[i]];
			moves[place + 1] = '\0';
			make_move(cube, sorted_moves[i]);
			search(cube, heuristic_values[sorted_moves[i]], 1, sorted_moves[i], last_move, bound - 1, moves, place + 1);
			undo_move(cube, sorted_moves[i]);
		}
	}

	return bound;
}

int main(int argc, char **argv){
	char move_names[6] = {'F', 'B', 'L', 'R', 'U', 'D'};
	char scramble[64];
	rubiks_cube *cube;
	int i;
	int max_cost;
	int bound = 20;
	FILE *fp;
	float sum = 0;
	char moves[64];
	int h;
	rubiks_move move;

	srand(time(NULL));

	edge_table = malloc(sizeof(unsigned char)*TABLE_SIZE);
	corner_table = malloc(sizeof(unsigned char)*88179840);
	if(!edge_table || !corner_table){
		fprintf(stderr, "Error: could not allocate table.\n");
		return 1;
	}
	memset(edge_table, 255, TABLE_SIZE);
	memset(corner_table, 255, 88179840);
	cube = create_cube();
	//gen_edge_table(cube, 0, 0, -1, edge_table);
	//fp = fopen("edge_table.dat", "wb");
	//fwrite(edge_table, sizeof(unsigned char), TABLE_SIZE, fp);
	//fclose(fp);
	//return 0;

	load_edge_table();
	load_corner_table();

	printf("enter scramble: ");
	scanf("%s", scramble);
	for(i = 0; i < 64; i++){
		if(scramble[i] == 'F'){
			make_move(cube, FRONT);
		}
		if(scramble[i] == 'B'){
			make_move(cube, BACK);
		}
		if(scramble[i] == 'L'){
			make_move(cube, LEFT);
		}
		if(scramble[i] == 'R'){
			make_move(cube, RIGHT);
		}
		if(scramble[i] == 'U'){
			make_move(cube, UP);
		}
		if(scramble[i] == 'D'){
			make_move(cube, DOWN);
		}
		if(scramble[i] == '\0'){
			break;
		}
	}

	for(h = heuristic(cube); h <= 20; h++){
		printf("h_val: %d\n", h);
		search(cube, heuristic(cube), 0, -1, -1, h + 1, moves, 0);
	}

	free(cube);
	free(edge_table);
	free(corner_table);

	return 0;
}

