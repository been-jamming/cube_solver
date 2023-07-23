#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cube.h"

const char *header = 
"\\documentclass[9pt]{article}\n"
"\\usepackage{multicol}\n"
"\\usepackage[left=2cm, right=0.5cm, bottom=0.5cm, top=0.5cm]{geometry}"
"\\begin{document}{\\begin{multicols}{6}\\small\n";

const char *footer =
"\\end{multicols}}\\end{document}\n";

const unsigned int TABLE0_SIZE = 1<<11;
unsigned char *edge_orient_table;
rubiks_cube *cube;
rubiks_cube solved;

void load_edge_table(void){
	FILE *fp;

	edge_orient_table = malloc(sizeof(unsigned char)*TABLE0_SIZE);
	fp = fopen("edge_orient_table.dat", "rb");
	fread(edge_orient_table, sizeof(unsigned char), TABLE0_SIZE, fp);
	fclose(fp);
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

void print_entry(unsigned int entry_id){
	int i;
	unsigned char odd = 0;

	memcpy(cube, &solved, sizeof(rubiks_cube));
	for(i = 0; i < 11; i++){
		if(entry_id&(1<<i)){
			cube->edge_orientation[i] = 1;
			odd = !odd;
		}
	}

	if(odd){
		cube->edge_orientation[11] = 1;
	}

	solve_cube_orientation(cube);
}

int main(int argc, char **argv){
	int i;

	cube = create_cube();
	load_edge_table();
	memcpy(&solved, cube, sizeof(rubiks_cube));

	printf("%s", header);

	for(i = 0; i < 1<<11; i++){
		printf("\\noindent %d: ", i);
		if(i){
			print_entry(i);
		} else {
			printf("[None]");
		}
		printf("\n\n");
	}

	printf("%s", footer);
}

