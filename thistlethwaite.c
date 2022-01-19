#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "cube.h"

unsigned int three_powers[8] = {1, 3, 9, 27, 81, 243, 729, 0};
unsigned char tetrad_write_value[576];
unsigned char tetrad_twist_table[576] = {
	0, 3, 1, 2, 4, 5, 3, 0, 4, 5, 1, 2, 2, 1, 5, 4, 0, 3, 5, 4, 
	2, 1, 3, 0, 3, 0, 4, 5, 1, 2, 0, 3, 1, 2, 4, 5, 5, 4, 2, 1, 
	3, 0, 2, 1, 5, 4, 0, 3, 1, 2, 0, 3, 5, 4, 2, 1, 5, 4, 0, 3, 
	3, 0, 4, 5, 1, 2, 4, 5, 3, 0, 2, 1, 4, 5, 3, 0, 2, 1, 5, 4, 
	2, 1, 3, 0, 0, 3, 1, 2, 4, 5, 1, 2, 0, 3, 5, 4, 2, 1, 5, 4, 
	0, 3, 1, 2, 0, 3, 5, 4, 4, 5, 3, 0, 2, 1, 3, 0, 4, 5, 1, 2, 
	5, 4, 2, 1, 3, 0, 4, 5, 3, 0, 2, 1, 1, 2, 0, 3, 5, 4, 0, 3, 
	1, 2, 4, 5, 3, 0, 4, 5, 1, 2, 0, 3, 1, 2, 4, 5, 5, 4, 2, 1, 
	3, 0, 2, 1, 5, 4, 0, 3, 0, 3, 1, 2, 4, 5, 3, 0, 4, 5, 1, 2, 
	2, 1, 5, 4, 0, 3, 5, 4, 2, 1, 3, 0, 2, 1, 5, 4, 0, 3, 1, 2, 
	0, 3, 5, 4, 4, 5, 3, 0, 2, 1, 3, 0, 4, 5, 1, 2, 5, 4, 2, 1, 
	3, 0, 4, 5, 3, 0, 2, 1, 1, 2, 0, 3, 5, 4, 0, 3, 1, 2, 4, 5, 
	1, 2, 0, 3, 5, 4, 2, 1, 5, 4, 0, 3, 3, 0, 4, 5, 1, 2, 4, 5, 
	3, 0, 2, 1, 4, 5, 3, 0, 2, 1, 5, 4, 2, 1, 3, 0, 0, 3, 1, 2, 
	4, 5, 1, 2, 0, 3, 5, 4, 4, 5, 3, 0, 2, 1, 5, 4, 2, 1, 3, 0, 
	0, 3, 1, 2, 4, 5, 1, 2, 0, 3, 5, 4, 1, 2, 0, 3, 5, 4, 2, 1, 
	5, 4, 0, 3, 3, 0, 4, 5, 1, 2, 4, 5, 3, 0, 2, 1, 5, 4, 2, 1, 
	3, 0, 4, 5, 3, 0, 2, 1, 1, 2, 0, 3, 5, 4, 0, 3, 1, 2, 4, 5, 
	2, 1, 5, 4, 0, 3, 1, 2, 0, 3, 5, 4, 4, 5, 3, 0, 2, 1, 3, 0, 
	4, 5, 1, 2, 0, 3, 1, 2, 4, 5, 3, 0, 4, 5, 1, 2, 2, 1, 5, 4, 
	0, 3, 5, 4, 2, 1, 3, 0, 3, 0, 4, 5, 1, 2, 0, 3, 1, 2, 4, 5, 
	5, 4, 2, 1, 3, 0, 2, 1, 5, 4, 0, 3, 5, 4, 2, 1, 3, 0, 4, 5, 
	3, 0, 2, 1, 1, 2, 0, 3, 5, 4, 0, 3, 1, 2, 4, 5, 2, 1, 5, 4, 
	0, 3, 1, 2, 0, 3, 5, 4, 4, 5, 3, 0, 2, 1, 3, 0, 4, 5, 1, 2, 
	4, 5, 3, 0, 2, 1, 5, 4, 2, 1, 3, 0, 0, 3, 1, 2, 4, 5, 1, 2, 
	0, 3, 5, 4, 1, 2, 0, 3, 5, 4, 2, 1, 5, 4, 0, 3, 3, 0, 4, 5, 
	1, 2, 4, 5, 3, 0, 2, 1, 3, 0, 4, 5, 1, 2, 0, 3, 1, 2, 4, 5, 
	5, 4, 2, 1, 3, 0, 2, 1, 5, 4, 0, 3, 0, 3, 1, 2, 4, 5, 3, 0, 
	4, 5, 1, 2, 2, 1, 5, 4, 0, 3, 5, 4, 2, 1, 3, 0
};

const unsigned int TABLE0_SIZE = 1<<11;
const unsigned int TABLE1_SIZE = 1082565;//3^7 times 12 choose 4
const unsigned int TABLE2_SIZE = 29400;//(8 choose 4)^2 * 6
const unsigned int TABLE3_SIZE = 663552;//(4!)^5/12
unsigned char *edge_orient_table;
unsigned char *G1_table;
unsigned char *G2_table;
unsigned char *G3_table;

static int count_bits8(uint8_t num){
	num = (num&0x55) + ((num&0xAA)>>1);
	num = (num&0x33) + ((num&0xCC)>>2);
	num = (num&0x0F) + ((num&0xF0)>>4);

	return num;
}

//Assign an index for b choices of a
unsigned int combination_index(int a, int b, int choices, uint16_t bits){
	unsigned int output = 0;

	while(bits){
		if(bits&1){
			a--;
			b--;
			if(a != 0){
				choices = choices*b/a;
			}
		} else {
			output += choices;
			if(a > 1){
				choices = choices*(a - b)/(a - 1);
				a--;
			}
		}
		bits >>= 1;
	}

	return output;
}

//This is an index for the orientation of all of the edges, ie the coset of G0
unsigned int thistlethwaite_index0(rubiks_cube *cube){
	unsigned int index = 0;
	int i;

	for(i = 0; i < 12; i++){
		index |= 0x7FF&((cube->edge_orientation[i])<<(cube->edge_permutation[i]));
	}

	return index;
}

//Assign an index fo the orientation of the corners of the cube
unsigned int corner_index(rubiks_cube *cube){
	unsigned int index = 0;
	int i;

	for(i = 0; i < 8; i++){
		index += cube->corner_orientation[i]*three_powers[cube->corner_permutation[i]];
	}

	return index;
}

//This is an index for the cosets G1\G0
unsigned int thistlethwaite_index1(rubiks_cube *cube){
	int i;
	uint16_t combination = 0;
	unsigned int edge_choice_index;
	unsigned int corner_orientation_index;

	//The F/B Slice
	combination |= 1<<cube->edge_permutation[1];
	combination |= 1<<cube->edge_permutation[3];
	combination |= 1<<cube->edge_permutation[9];
	combination |= 1<<cube->edge_permutation[11];

	edge_choice_index = combination_index(12, 4, 165, combination);//165 is 11 choose 3
	corner_orientation_index = corner_index(cube);

	return corner_orientation_index*495 + edge_choice_index;//495 is 12 choose 4
}

//Generate table which orients the edges of the cube
void thistlethwaite_gen_table0(rubiks_cube *cube, int cost, int count, rubiks_move last_move, unsigned char *table){
	unsigned int index;
	int i;

	if(cost > 20){
		return;
	}

	index = thistlethwaite_index0(cube);
	if(table[index] <= cost){
		return;
	}
	table[index] = cost;

	for(i = 0; i < 6; i++){
		if(last_move == FRONT && i == BACK){
			continue;
		}
		if(last_move == LEFT && i == RIGHT){
			continue;
		}
		if(last_move == UP && i == DOWN){
			continue;
		}
		if(last_move == i){
			if(count < 3){
				make_move(cube, i);
				thistlethwaite_gen_table0(cube, cost, count + 1, i, table);
				undo_move(cube, i);
			} else {
				continue;
			}
		} else {
			make_move(cube, i);
			thistlethwaite_gen_table0(cube, cost + 1, 1, i, table);
			undo_move(cube, i);
		}
	}
}

void thistlethwaite_gen_table1(rubiks_cube *cube, int cost, int count, rubiks_move last_move, unsigned char *table){
	unsigned int index;
	int i;
	char move_names[6] = {'F', 'B', 'L', 'R', 'U', 'D'};

	if(cost > 20){
		return;
	}

	index = thistlethwaite_index1(cube);
	if(table[index] <= cost){
		return;
	}
	table[index] = cost;

	for(i = 0; i < 6; i++){
		if(last_move == FRONT && i == BACK){
			continue;
		}
		if(last_move == LEFT && i == RIGHT){
			continue;
		}
		if(last_move == UP && i == DOWN){
			continue;
		}
		if(last_move == i){
			if(count < 3 && i != UP && i != DOWN){
				make_move(cube, i);
				thistlethwaite_gen_table1(cube, cost, count + 1, i, table);
				undo_move(cube, i);
			} else {
				continue;
			}
		} else {
			make_move(cube, i);
			if(i == UP || i == DOWN){
				make_move(cube, i);
			}
			thistlethwaite_gen_table1(cube, cost + 1, 1, i, table);
			undo_move(cube, i);
			if(i == UP || i == DOWN){
				undo_move(cube, i);
			}
		}
	}
}

static unsigned int permutation_index4(int permutation[4]){
	uint8_t occupied = 0;
	unsigned int factorial = 24;
	unsigned int index = 0;
	int i;
	int count;

	for(i = 0; i < 4; i++){
		factorial /= 4 - i;

		count = count_bits8((~occupied)&((1<<permutation[i]) - 1));
		index += count*factorial;
		occupied |= 1<<permutation[i];
	}

	return index;
}

unsigned int get_tetrad_index(rubiks_cube *cube){
	int perm[8];
	int perm_tetrad0[4];
	int perm_tetrad1[4];
	int tetrad0_index = 0;
	int tetrad1_index = 0;
	int i;

	for(i = 0; i < 8; i++){
		perm[cube->corner_permutation[i]] = i;
	}

	for(i = 0; i < 8; i++){
		if((perm[i] > 4 && (perm[i]%2)) || (perm[i] < 4 && !(perm[i]%2))){
			perm_tetrad1[tetrad1_index] = perm[i]/2;
			tetrad1_index++;
		} else {
			perm_tetrad0[tetrad0_index] = perm[i]/2;
			tetrad0_index++;
		}
	}

	return 24*permutation_index4(perm_tetrad0) + permutation_index4(perm_tetrad1);
}

unsigned int thistlethwaite_index2(rubiks_cube *cube){
	uint8_t combination = 0;
	unsigned int edge_choice_translation[12] = {0, 0, 1, 0, 2, 3, 4, 5, 6, 0, 7, 0};
	unsigned int edge_choice_index;
	unsigned int corner_choice_index;

	//The L/R Slice
	combination |= 1<<edge_choice_translation[cube->edge_permutation[0]];
	combination |= 1<<edge_choice_translation[cube->edge_permutation[2]];
	combination |= 1<<edge_choice_translation[cube->edge_permutation[8]];
	combination |= 1<<edge_choice_translation[cube->edge_permutation[10]];

	edge_choice_index = combination_index(8, 4, 35, combination);//35 is 7 choose 3

	//The first tetrad
	combination = 0;
	combination |= 1<<cube->corner_permutation[1];
	combination |= 1<<cube->corner_permutation[3];
	combination |= 1<<cube->corner_permutation[4];
	combination |= 1<<cube->corner_permutation[6];

	corner_choice_index = combination_index(8, 4, 35, combination);//35 is 7 choose 3

	return edge_choice_index*420 + corner_choice_index*6 + tetrad_twist_table[get_tetrad_index(cube)];
}

void thistlethwaite_gen_table2(rubiks_cube *cube, int cost, int count, rubiks_move last_move, unsigned char *table){
	unsigned int index;
	int i;

	if(cost > 13){
		return;
	}

	index = thistlethwaite_index2(cube);
	if(table[index] <= cost){
		return;
	}
	table[index] = cost;

	for(i = 0; i < 6; i++){
		if(last_move == FRONT && i == BACK){
			continue;
		}
		if(last_move == LEFT && i == RIGHT){
			continue;
		}
		if(last_move == UP && i == DOWN){
			continue;
		}
		if(last_move == i){
			if(count < 3 && (i == FRONT || i == BACK)){
				make_move(cube, i);
				thistlethwaite_gen_table2(cube, cost, count + 1, i, table);
				undo_move(cube, i);
			} else {
				continue;
			}
		} else {
			make_move(cube, i);
			if(i != FRONT && i != BACK){
				make_move(cube, i);
			}
			thistlethwaite_gen_table2(cube, cost + 1, 1, i, table);
			undo_move(cube, i);
			if(i != FRONT && i != BACK){
				undo_move(cube, i);
			}
		}
	}
}

void gen_tetrad_twist_table(rubiks_cube *cube, int coset_list[3], int cost, rubiks_move last_move, int write_value){
	unsigned int tetrad_index;
	int i;

	for(i = 0; i < 3; i++){
		if(coset_list[i] >= 0){
			make_move(cube, coset_list[i]);
		}
	}
	tetrad_index = get_tetrad_index(cube);
	for(i = 2; i >= 0; i--){
		if(coset_list[i] >= 0){
			undo_move(cube, coset_list[i]);
		}
	}
	if(tetrad_twist_table[tetrad_index] <= cost){
		return;
	}
	if(tetrad_write_value[tetrad_index] != write_value && tetrad_write_value[tetrad_index] != 255){
		printf("WHAT?");
	}
	tetrad_twist_table[tetrad_index] = cost;
	tetrad_write_value[tetrad_index] = write_value;
	//printf("index: %d table: %d cost: %d\n", tetrad_index, tetrad_twist_table[tetrad_index], cost);

	for(i = 0; i < 6; i++){
		if(last_move == UP && i == DOWN){
			continue;
		}
		if(last_move == LEFT && i == RIGHT){
			continue;
		}
		if(last_move == FRONT && i == BACK){
			continue;
		}
		if(last_move == i){
			continue;
		}
		make_move(cube, i);
		make_move(cube, i);
		gen_tetrad_twist_table(cube, coset_list, cost + 1, i, write_value);
		undo_move(cube, i);
		undo_move(cube, i);
	}
}

unsigned int get_permutation_index(int *permutation, int size, int num_entries, int factorial){//Factorial should be set to size!/(size - num_entries)!
	uint8_t occupied = 0;
	unsigned int index = 0;
	int i;
	int count;

	for(i = 0; i < num_entries; i++){
		factorial /= size - i;
		count = count_bits8((~occupied)&((1<<permutation[i]) - 1));
		index += count*factorial;
		occupied |= 1<<permutation[i];
	}

	return index;
}

unsigned int thistlethwaite_index3(rubiks_cube *cube){
	unsigned int index;
	int permutation[4];
	int edge_translation[12] = {0, 0, 1, 1, 0, 1, 2, 3, 2, 2, 3, 3};
	int corner_translation[8] = {0, 0, 1, 1, 2, 2, 3, 3};
	int i;

	permutation[0] = edge_translation[cube->edge_permutation[1]];
	permutation[1] = edge_translation[cube->edge_permutation[3]];
	permutation[2] = edge_translation[cube->edge_permutation[9]];
	permutation[3] = edge_translation[cube->edge_permutation[11]];
	index = get_permutation_index(permutation, 4, 4, 24);

	permutation[0] = edge_translation[cube->edge_permutation[0]];
	permutation[1] = edge_translation[cube->edge_permutation[2]];
	permutation[2] = edge_translation[cube->edge_permutation[8]];
	permutation[3] = edge_translation[cube->edge_permutation[10]];
	index *= 24;
	index += get_permutation_index(permutation, 4, 4, 24);

	permutation[0] = edge_translation[cube->edge_permutation[4]];
	permutation[1] = edge_translation[cube->edge_permutation[5]];
	index *= 12;
	index += get_permutation_index(permutation, 4, 2, 12);

	permutation[0] = corner_translation[cube->corner_permutation[0]];
	permutation[1] = corner_translation[cube->corner_permutation[2]];
	permutation[2] = corner_translation[cube->corner_permutation[5]];
	permutation[3] = corner_translation[cube->corner_permutation[7]];
	index *= 24;
	index += get_permutation_index(permutation, 4, 4, 24);

	index *= 4;
	index += corner_translation[cube->corner_permutation[1]];

	return index;
}

void thistlethwaite_gen_table3(rubiks_cube *cube, int cost, rubiks_move last_move, unsigned char *table){
	unsigned int index;
	int i;

	if(cost > 15){
		return;
	}

	index = thistlethwaite_index3(cube);
	if(table[index] <= cost){
		return;
	}
	table[index] = cost;

	for(i = 0; i < 6; i++){
		if(last_move == FRONT && i == BACK){
			continue;
		}
		if(last_move == LEFT && i == RIGHT){
			continue;
		}
		if(last_move == UP && i == DOWN){
			continue;
		}
		if(last_move == i){
			continue;
		}
		make_move(cube, i);
		make_move(cube, i);
		thistlethwaite_gen_table3(cube, cost + 1, i, table);
		undo_move(cube, i);
		undo_move(cube, i);
	}
}

void solve_cube_orientation(rubiks_cube *cube){
	int start_num_moves;
	int num_moves;
	int i;
	char move_names[6] = {'F', 'B', 'L', 'R', 'U', 'D'};

	start_num_moves = edge_orient_table[thistlethwaite_index0(cube)];
	if(!start_num_moves){
		return;
	}

	for(i = 0; i < 6; i++){
		make_move(cube, i);
		if(edge_orient_table[thistlethwaite_index0(cube)] < start_num_moves){
			printf("%c", move_names[i]);
			solve_cube_orientation(cube);
			return;
		}
		undo_move(cube, i);
	}
	for(i = 0; i < 6; i++){
		make_move(cube, i);
		make_move(cube, i);
		if(edge_orient_table[thistlethwaite_index0(cube)] < start_num_moves){
			printf("%c2", move_names[i]);
			solve_cube_orientation(cube);
			return;
		}
		undo_move(cube, i);
		undo_move(cube, i);
	}
	for(i = 0; i < 6; i++){
		undo_move(cube, i);
		if(edge_orient_table[thistlethwaite_index0(cube)] < start_num_moves){
			printf("%c'", move_names[i]);
			solve_cube_orientation(cube);
			return;
		}
		make_move(cube, i);
	}
	printf("OH NO.\n");
}

void solve_cube_G1(rubiks_cube *cube){
	int start_num_moves;
	int num_moves;
	int i;
	char move_names[6] = {'F', 'B', 'L', 'R', 'U', 'D'};

	start_num_moves = G1_table[thistlethwaite_index1(cube)];
	if(!start_num_moves){
		return;
	}

	for(i = 0; i < 6; i++){
		if(i == UP || i == DOWN){
			continue;
		}
		make_move(cube, i);
		if(G1_table[thistlethwaite_index1(cube)] < start_num_moves){
			printf("%c", move_names[i]);
			solve_cube_G1(cube);
			return;
		}
		undo_move(cube, i);
	}
	for(i = 0; i < 6; i++){
		make_move(cube, i);
		make_move(cube, i);
		if(G1_table[thistlethwaite_index1(cube)] < start_num_moves){
			printf("%c2", move_names[i]);
			solve_cube_G1(cube);
			return;
		}
		undo_move(cube, i);
		undo_move(cube, i);
	}
	for(i = 0; i < 6; i++){
		if(i == UP || i == DOWN){
			continue;
		}
		undo_move(cube, i);
		if(G1_table[thistlethwaite_index1(cube)] < start_num_moves){
			printf("%c'", move_names[i]);
			solve_cube_G1(cube);
			return;
		}
		make_move(cube, i);
	}
	printf("OH NO.\n");
}

void solve_cube_G2(rubiks_cube *cube){
	int start_num_moves;
	int num_moves;
	int i;
	char move_names[6] = {'F', 'B', 'L', 'R', 'U', 'D'};

	start_num_moves = G2_table[thistlethwaite_index2(cube)];
	if(!start_num_moves){
		return;
	}

	for(i = 0; i < 6; i++){
		if(i != FRONT && i != BACK){
			continue;
		}
		make_move(cube, i);
		if(G2_table[thistlethwaite_index2(cube)] < start_num_moves){
			printf("%c", move_names[i]);
			solve_cube_G2(cube);
			return;
		}
		undo_move(cube, i);
	}
	for(i = 0; i < 6; i++){
		make_move(cube, i);
		make_move(cube, i);
		if(G2_table[thistlethwaite_index2(cube)] < start_num_moves){
			printf("%c2", move_names[i]);
			solve_cube_G2(cube);
			return;
		}
		undo_move(cube, i);
		undo_move(cube, i);
	}
	for(i = 0; i < 6; i++){
		if(i != FRONT && i != BACK){
			continue;
		}
		undo_move(cube, i);
		if(G2_table[thistlethwaite_index2(cube)] < start_num_moves){
			printf("%c'", move_names[i]);
			solve_cube_G2(cube);
			return;
		}
		make_move(cube, i);
	}
	printf("OH NO.\n");
}

void solve_cube_G3(rubiks_cube *cube){
	int start_num_moves;
	int i;
	char move_names[6] = {'F', 'B', 'L', 'R', 'U', 'D'};

	start_num_moves = G3_table[thistlethwaite_index3(cube)];
	if(!start_num_moves){
		return;
	}

	for(i = 0; i < 6; i++){
		make_move(cube, i);
		make_move(cube, i);
		if(G3_table[thistlethwaite_index3(cube)] < start_num_moves){
			printf("%c2", move_names[i]);
			solve_cube_G3(cube);
			return;
		}
		undo_move(cube, i);
		undo_move(cube, i);
	}

	printf("OH NO.\n");
}

void load_edge_orient_table(){
	FILE *fp;

	edge_orient_table = malloc(sizeof(unsigned char)*TABLE0_SIZE);
	fp = fopen("edge_orient_table.dat", "rb");
	fread(edge_orient_table, sizeof(unsigned char), TABLE0_SIZE, fp);
	fclose(fp);
}

void load_G1_table(){
	FILE *fp;

	G1_table = malloc(sizeof(unsigned char)*TABLE1_SIZE);
	fp = fopen("G1_table.dat", "rb");
	fread(G1_table, sizeof(unsigned char), TABLE1_SIZE, fp);
	fclose(fp);
}

void load_G2_table(){
	FILE *fp;

	G2_table = malloc(sizeof(unsigned char)*TABLE2_SIZE);
	fp = fopen("G2_table.dat", "rb");
	fread(G2_table, sizeof(unsigned char), TABLE2_SIZE, fp);
	fclose(fp);
}

void load_G3_table(){
	FILE *fp;

	G3_table = malloc(sizeof(unsigned char)*TABLE3_SIZE);
	fp = fopen("G3_table.dat", "rb");
	fread(G3_table, sizeof(unsigned char), TABLE3_SIZE, fp);
	fclose(fp);
}

void gen_G0_table(rubiks_cube *cube){
	edge_orient_table = malloc(sizeof(unsigned char)*TABLE0_SIZE);
	memset(edge_orient_table, 255, sizeof(unsigned char)*TABLE0_SIZE);
	thistlethwaite_gen_table0(cube, 0, 0, -1, edge_orient_table);
}

void gen_G1_table(rubiks_cube *cube){
	G1_table = malloc(sizeof(unsigned char)*TABLE1_SIZE);
	memset(G1_table, 255, sizeof(unsigned char)*TABLE1_SIZE);
	thistlethwaite_gen_table1(cube, 0, 0, -1, G1_table);
}

void gen_G2_table(rubiks_cube *cube){
	G2_table = malloc(sizeof(unsigned char)*TABLE2_SIZE);
	memset(G2_table, 255, sizeof(unsigned char)*TABLE2_SIZE);
	thistlethwaite_gen_table2(cube, 0, 0, -1, G2_table);
}

void gen_G3_table(rubiks_cube *cube){
	G3_table = malloc(sizeof(unsigned char)*TABLE3_SIZE);
	memset(G3_table, 255, sizeof(unsigned char)*TABLE3_SIZE);
	thistlethwaite_gen_table3(cube, 0, -1, G3_table);
}

void write_G1_table(){
	FILE *fp;

	fp = fopen("G1_table.dat", "wb");
	fwrite(G1_table, sizeof(unsigned char), TABLE1_SIZE, fp);
	fclose(fp);
}

void write_G2_table(){
	FILE *fp;

	fp = fopen("G2_table.dat", "wb");
	fwrite(G2_table, sizeof(unsigned char), TABLE2_SIZE, fp);
	fclose(fp);
}

void write_G3_table(){
	FILE *fp;

	fp = fopen("G3_table.dat", "wb");
	fwrite(G3_table, sizeof(unsigned char), TABLE3_SIZE, fp);
	fclose(fp);
}

void print_tetrad_twist_table(){
	int i;

	printf("unsigned char tetrad_twist_table[576] = {");
	for(i = 0; i < 576; i++){
		if(i%20 == 0){
			printf("\n");
		}
		printf("%d, ", tetrad_write_value[i]);
	}
}

void enter_scramble(rubiks_cube *cube){
	char scramble[256];
	char move_names[6] = {'F', 'B', 'L', 'R', 'U', 'D'};
	int i;
	int j;
	rubiks_move last_move = 0;

	fgets(scramble, 256, stdin);
	for(i = 0; scramble[i]; i++){
		for(j = 0; j < 6; j++){
			if(scramble[i] == move_names[j]){
				last_move = j;
				break;
			}
		}
		if(j < 6){
			make_move(cube, j);
		} else if(scramble[i] == '2'){
			make_move(cube, last_move);
		} else if(scramble[i] == '\''){
			undo_move(cube, last_move);
			undo_move(cube, last_move);
		}
	}
}

int main(int argc, char **argv){
	FILE *fp;
	rubiks_cube *cube;
	char move_names[6] = {'F', 'B', 'L', 'R', 'U', 'D'};
	rubiks_move move;
	int i;

	srand(time(NULL));
	cube = create_cube();

	load_edge_orient_table();
	load_G1_table();
	load_G2_table();
	load_G3_table();

	printf("enter scramble: ");
	enter_scramble(cube);

	printf("\nedge orient solution: ");
	solve_cube_orientation(cube);
	printf("\nG1 solution: ");
	solve_cube_G1(cube);
	printf("\nG2 solution: ");
	solve_cube_G2(cube);
	printf("\nG3 solution: ");
	solve_cube_G3(cube);
	printf("\n");

	write_G3_table();

	free(edge_orient_table);
	free(G1_table);
	free(G2_table);
	free(G3_table);
	free(cube);

	return 0;
}

