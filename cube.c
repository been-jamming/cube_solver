//Implementation of Korf's optimal solver

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "cube.h"

static int edge_permutations[6][12] = {
	{4, 1, 2, 3, 8, 5, 6, 0, 7, 9, 10, 11},
	{0, 1, 6, 3, 4, 2, 10, 7, 8, 9, 5, 11},
	{0, 5, 2, 3, 1, 9, 6, 7, 8, 4, 10, 11},
	{0, 1, 2, 7, 4, 5, 3, 11, 8, 9, 10, 6},
	{3, 0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11},
	{0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 8}
};

static int inverse_edge_permutations[6][12] = {
	{7, 1, 2, 3, 0, 5, 6, 8, 4, 9, 10, 11},
	{0, 1, 5, 3, 4, 10, 2, 7, 8, 9, 6, 11},
	{0, 4, 2, 3, 9, 1, 6, 7, 8, 5, 10, 11},
	{0, 1, 2, 6, 4, 5, 11, 3, 8, 9, 10, 7},
	{1, 2, 3, 0, 4, 5, 6, 7, 8, 9, 10, 11},
	{0, 1, 2, 3, 4, 5, 6, 7, 11, 8, 9, 10}
};

static int corner_permutations[6][8] = {
	{4, 1, 2, 0, 7, 5, 6, 3},
	{0, 2, 6, 3, 4, 1, 5, 7},
	{1, 5, 2, 3, 0, 4, 6, 7},
	{0, 1, 3, 7, 4, 5, 2, 6},
	{3, 0, 1, 2, 4, 5, 6, 7},
	{0, 1, 2, 3, 5, 6, 7, 4}
};

static int inverse_corner_permutations[6][12] = {
	{3, 1, 2, 7, 0, 5, 6, 4},
	{0, 5, 1, 3, 4, 6, 2, 7},
	{4, 0, 2, 3, 5, 1, 6, 7},
	{0, 1, 6, 2, 4, 5, 7, 3},
	{1, 2, 3, 0, 4, 5, 6, 7},
	{0, 1, 2, 3, 7, 4, 5, 6}
};

//Only Front/Back change the edge orientations
static int edge_orientations[2][12] = {
	{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1}
};

//Front/Back don't change the corner orientations
static int corner_orientations[4][8] = {
	{2, 1, 0, 0, 1, 2, 0, 0},
	{0, 0, 2, 1, 0, 0, 1, 2},
	{1, 2, 1, 2, 0, 0, 0, 0},
	{0, 0, 0, 0, 2, 1, 2, 1}
};

void undo_move(rubiks_cube *cube, rubiks_move move){
	int new_edge_permutation[12];
	int new_corner_permutation[8];
	int i;

	for(i = 0; i < 12; i++){
		new_edge_permutation[i] = edge_permutations[move][cube->edge_permutation[i]];
	}

	for(i = 0; i < 8; i++){
		new_corner_permutation[i] = corner_permutations[move][cube->corner_permutation[i]];
	}

	memcpy(cube->edge_permutation, new_edge_permutation, sizeof(cube->edge_permutation));
	memcpy(cube->corner_permutation, new_corner_permutation, sizeof(cube->corner_permutation));

	if(move == UP || move == DOWN){
		for(i = 0; i < 12; i++){
			cube->edge_orientation[i] ^= edge_orientations[move - UP][cube->edge_permutation[i]];
		}
	}

	if(move != FRONT && move != BACK){
		for(i = 0; i < 8; i++){
			cube->corner_orientation[i] = (cube->corner_orientation[i] + corner_orientations[move - 2][cube->corner_permutation[i]])%3;
		}
	}
}

void make_move(rubiks_cube *cube, rubiks_move move){
	int new_edge_permutation[12];
	int new_corner_permutation[8];
	int i;

	if(move != FRONT && move != BACK){
		for(i = 0; i < 8; i++){
			cube->corner_orientation[i] = (3 + cube->corner_orientation[i] - corner_orientations[move - 2][cube->corner_permutation[i]])%3;
		}
	}

	if(move == UP || move == DOWN){
		for(i = 0; i < 12; i++){
			cube->edge_orientation[i] ^= edge_orientations[move - UP][cube->edge_permutation[i]];
		}
	}

	for(i = 0; i < 12; i++){
		new_edge_permutation[i] = inverse_edge_permutations[move][cube->edge_permutation[i]];
	}

	for(i = 0; i < 8; i++){
		new_corner_permutation[i] = inverse_corner_permutations[move][cube->corner_permutation[i]];
	}

	memcpy(cube->edge_permutation, new_edge_permutation, sizeof(cube->edge_permutation));
	memcpy(cube->corner_permutation, new_corner_permutation, sizeof(cube->corner_permutation));
}

int cubes_equal(rubiks_cube *cube0, rubiks_cube *cube1){
	return !memcmp(cube0->edge_permutation, cube1->edge_permutation, sizeof(cube0->edge_permutation)) &&
		!memcmp(cube0->corner_permutation, cube1->corner_permutation, sizeof(cube0->corner_permutation)) &&
		!memcmp(cube0->edge_orientation, cube1->edge_orientation, sizeof(cube0->edge_orientation)) &&
		!memcmp(cube0->corner_orientation, cube1->corner_orientation, sizeof(cube0->corner_orientation));
}

int edges_equal(rubiks_cube *cube0, rubiks_cube *cube1){
	return !memcmp(cube0->edge_permutation, cube1->edge_permutation, sizeof(cube0->edge_permutation)) &&
		!memcmp(cube0->edge_orientation, cube1->edge_orientation, sizeof(cube0->edge_orientation));
}

int corners_equal(rubiks_cube *cube0, rubiks_cube *cube1){
	return !memcmp(cube0->corner_permutation, cube1->corner_permutation, sizeof(cube0->corner_permutation)) &&
		!memcmp(cube0->corner_orientation, cube1->corner_orientation, sizeof(cube0->corner_orientation));
}

rubiks_cube *create_cube(){
	rubiks_cube *output;
	int i;

	output = malloc(sizeof(rubiks_cube));
	if(!output){
		return NULL;
	}
	for(i = 0; i < 12; i++){
		output->edge_permutation[i] = i;
		output->edge_orientation[i] = 0;
	}
	for(i = 0; i < 8; i++){
		output->corner_permutation[i] = i;
		output->corner_orientation[i] = 0;
	}

	return output;
}

int main2(int argc, char **argv){
	rubiks_cube *cube0;
	rubiks_cube *cube1;
	int i;

	cube0 = create_cube();
	cube1 = create_cube();

	make_move(cube1, UP);
	undo_move(cube1, UP);
	if(cubes_equal(cube0, cube1)){
		printf("SOLVED\n");
	} else {
		printf("OH NO\n");
	}
	
	make_move(cube1, UP);
	make_move(cube1, RIGHT); make_move(cube1, RIGHT);
	make_move(cube1, FRONT);
	make_move(cube1, BACK);
	make_move(cube1, RIGHT);
	make_move(cube1, BACK); make_move(cube1, BACK);
	make_move(cube1, RIGHT);
	make_move(cube1, UP); make_move(cube1, UP);
	make_move(cube1, LEFT);
	make_move(cube1, BACK); make_move(cube1, BACK);
	make_move(cube1, RIGHT);
	undo_move(cube1, UP);
	undo_move(cube1, DOWN);
	make_move(cube1, RIGHT); make_move(cube1, RIGHT);
	make_move(cube1, FRONT);
	undo_move(cube1, RIGHT);
	make_move(cube1, LEFT);
	make_move(cube1, BACK); make_move(cube1, BACK);
	make_move(cube1, UP); make_move(cube1, UP);
	make_move(cube1, FRONT); make_move(cube1, FRONT);

	if(!memcmp(cube0->edge_permutation, cube1->edge_permutation, sizeof(cube0->edge_permutation))){
		printf("Edge permutation is the same.\n");
	}
	if(!memcmp(cube0->corner_permutation, cube1->corner_permutation, sizeof(cube0->corner_permutation))){
		printf("Corner permutation is the same.\n");
	}
	if(!memcmp(cube0->corner_orientation, cube1->corner_orientation, sizeof(cube0->corner_orientation))){
		printf("Corners are oriented correctly.\n");
	}

	for(i = 0; i < 12; i++){
		if(!cube1->edge_orientation[i]){
			break;
		}
	}
	if(i == 12){
		printf("Edges are all misoriented.\n");
	}

	free(cube0);
	free(cube1);

	return 0;
}
