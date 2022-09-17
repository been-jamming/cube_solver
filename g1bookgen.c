#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "cube.h"

const unsigned int three_powers[8] = {1, 3, 9, 27, 81, 243, 729, 0};
const char header[] = 
"\\documentclass{article}\n"
"\\usepackage{multicol}\n"
"\\usepackage[left=2cm, right=0.5cm, bottom=0.5cm, top=0.5cm]{geometry}"
"\\begin{document}{\\begin{multicols}{5}\\tiny\n";

const char footer[] =
"\\end{multicols}}\\end{document}\n";

const unsigned int TABLE1_SIZE = 1082565;//3^7 times 12 choose 4
unsigned char *G1_table;
rubiks_cube *cube;
rubiks_cube solved;

void load_G1_table(){
	FILE *fp;

	G1_table = malloc(sizeof(unsigned char)*TABLE1_SIZE);
	fp = fopen("G1_table.dat", "rb");
	fread(G1_table, sizeof(unsigned char), TABLE1_SIZE, fp);
	fclose(fp);
}

unsigned int corner_index(rubiks_cube *cube){
	unsigned int index = 0;
	int i;

	for(i = 0; i < 8; i++){
		index += cube->corner_orientation[i]*three_powers[cube->corner_permutation[i]];
	}

	return index;
}

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

//Inverts combination_index
uint16_t combination_inverse(int a, int b, int choices, unsigned int index){
	uint16_t output = 0;
	uint16_t bit = 1;

	while(a > 0){
		if(index >= choices){
			index -= choices;
			if(a > 1){
				choices = choices*(a - b)/(a - 1);
			}
			a--;
		} else {
			output |= bit;
			a--;
			b--;
			if(a > 0){
				choices = choices*b/a;
			}
		}

		bit <<= 1;
	}

	return output;
}

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

void swap_edges(rubiks_cube *cube, int edge0, int edge1){
	int temp;

	temp = cube->edge_permutation[edge0];
	cube->edge_permutation[edge0] = cube->edge_permutation[edge1];
	cube->edge_permutation[edge1] = temp;
}

void print_entry(unsigned int entry_id){
	int i;
	int edge_parity = 0;
	int corner_parity = 0;
	uint16_t combination;
	int middle_edges[4] = {1, 3, 9, 11};
	int edge_index = 0;
	int edge_inverse[12];

	memcpy(cube, &solved, sizeof(rubiks_cube));

	for(i = 0; i < 12; i++){
		edge_inverse[cube->edge_permutation[i]] = i;
	}
	combination = combination_inverse(12, 4, 165, entry_id%495);
	for(i = 0; i < 12; i++){
		if(combination&1){
			swap_edges(cube, middle_edges[edge_index], edge_inverse[i]);
			if(middle_edges[edge_index] != edge_inverse[i]){
				edge_parity = !edge_parity;
			}
			edge_index++;
		}
		combination >>= 1;
	}
	if(edge_parity){
		swap_edges(cube, 4, 5);
	}

	entry_id /= 495;
	for(i = 6; i >= 0; i--){
		if(entry_id >= three_powers[i]){
			cube->corner_orientation[i] = entry_id/three_powers[i];
			corner_parity += entry_id/three_powers[i];
			corner_parity %= 3;
			entry_id %= three_powers[i];
		}
	}

	cube->corner_orientation[7] = (3 - corner_parity)%3;

	solve_cube_G1(cube);
}

int main(int argc, char **argv){
	int i;

	cube = create_cube();
	load_G1_table();
	memcpy(&solved, cube, sizeof(rubiks_cube));

	printf("%s", header);

	for(i = 0; i < TABLE1_SIZE; i++){
		printf("\\noindent %d: ", i);
		//if(i){
			print_entry(i);
		//} else {
			//printf("[None]");
		//}
		printf("\n\n");
	}

	printf("%s", footer);
}

