#include <stdlib.h>
#include <stdio.h>
#include "cube.h"
#include "cube_book_gen.h"

const char *color_strings[7] = {
	"blue",
	"green",
	"red",
	"orange",
	"white",
	"yellow",
	"black"
};

const char *document_start =
	"\\documentclass{article}\n"
	"\\usepackage{tikz}\n"
	"\\title{How to Solve a Rubik's Cube Like a Computer}\n"
	"\\date{}\n"
	"\\author{}\n"
	"\\input{template.tex}\n"
	"\\begin{document}\n"
	"\\maketitle\n";

const char *document_end = "\\end{document}\n";

const char *image_start = "\\begin{tikzpicture}[scale=3.0]\n";

const char *image_end = "\\end{tikzpicture}\n";

int edge_colors[12][2] = {
	{WHITE, BLUE}, {WHITE, RED}, {WHITE, GREEN}, {WHITE, ORANGE},
	{BLUE, RED}, {GREEN, RED}, {GREEN, ORANGE}, {BLUE, ORANGE},
	{YELLOW, BLUE}, {YELLOW, RED}, {YELLOW, GREEN}, {YELLOW, ORANGE}
};

int edge_sticker_positions_top[12] = {
	7, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1, -1
};

int edge_sticker_positions_left[12] = {
	-1, 1, -1, -1, 5, 3, -1, -1, -1, 7, -1, -1
};

int edge_sticker_positions_front[12] = {
	1, -1, -1, -1, 3, -1, -1, 5, 7, -1, -1, -1
};

//Corner colors in clockwise order
int corner_colors[8][3] = {
	{BLUE, RED, WHITE}, {GREEN, WHITE, RED},
	{GREEN, ORANGE, WHITE}, {BLUE, WHITE, ORANGE},
	{BLUE, YELLOW, RED}, {GREEN, RED, YELLOW},
	{GREEN, YELLOW, ORANGE}, {BLUE, ORANGE, YELLOW}
};

int corner_sticker_positions_top[8] = {
	8, 2, 0, 6, -1, -1, -1, -1
};

int corner_orientation_offsets_top[8] = {
	2, 4, 2, 4, -1, -1, -1, -1
};

int corner_sticker_positions_left[8] = {
	2, 0, -1, -1, 8, 6, -1, -1
};

int corner_orientation_offsets_left[8] = {
	4, 2, -1, -1, 2, 4, -1, -1
};

int corner_sticker_positions_front[8] = {
	0, -1, -1, 2, 6, -1, -1, 8
};

int corner_orientation_offsets_front[8] = {
	3, -1, -1, 3, 3, -1, -1, 3
};

void gen_top_face(rubiks_cube *cube){
	int i;
	int color0;
	int color1;
	int orig_color0;
	int orig_color1;
	int sticker_colors[9];

	for(i = 0; i < 9; i++){
		sticker_colors[i] = BLACK;
	}

	sticker_colors[4] = WHITE;
	for(i = 0; i < 12; i++){
		if(edge_sticker_positions_top[cube->edge_permutation[i]] == -1){
			continue;
		}
		orig_color0 = edge_colors[cube->edge_permutation[i]][0];
		orig_color1 = edge_colors[cube->edge_permutation[i]][1];
		color0 = edge_colors[i][0];
		color1 = edge_colors[i][1];

		//If the edge is oriented
		if(cube->edge_orientation[i] == 0){
			if(color1>>1 == orig_color1>>1){
				sticker_colors[edge_sticker_positions_top[cube->edge_permutation[i]]] = color0;
			} else {
				sticker_colors[edge_sticker_positions_top[cube->edge_permutation[i]]] = color1;
			}
		} else {
			if(color1>>1 == orig_color1>>1){
				sticker_colors[edge_sticker_positions_top[cube->edge_permutation[i]]] = color1;
			} else {
				sticker_colors[edge_sticker_positions_top[cube->edge_permutation[i]]] = color0;
			}
		}
	}

	for(i = 0; i < 8; i++){
		if(corner_sticker_positions_top[cube->corner_permutation[i]] == -1){
			continue;
		}
		sticker_colors[corner_sticker_positions_top[cube->corner_permutation[i]]] = corner_colors[i][(corner_orientation_offsets_top[cube->corner_permutation[i]] - cube->corner_orientation[i])%3];
	}

	printf("\\drawtop");
	for(i = 0; i < 9; i++){
		printf("{%s}", color_strings[sticker_colors[i]]);
	}
	printf("\n");
}

void gen_left_face(rubiks_cube *cube){
	int i;
	int color0;
	int color1;
	int orig_color0;
	int orig_color1;
	int sticker_colors[9];

	for(i = 0; i < 9; i++){
		sticker_colors[i] = BLACK;
	}

	sticker_colors[4] = RED;
	for(i = 0; i < 12; i++){
		if(edge_sticker_positions_left[cube->edge_permutation[i]] == -1){
			continue;
		}
		orig_color0 = edge_colors[cube->edge_permutation[i]][0];
		orig_color1 = edge_colors[cube->edge_permutation[i]][1];
		color0 = edge_colors[i][0];
		color1 = edge_colors[i][1];

		if(cube->edge_orientation[i] == 0){
			if(color1>>1 == RED>>1){
				sticker_colors[edge_sticker_positions_left[cube->edge_permutation[i]]] = color1;
			} else if(color0>>1 == RED>>1){
				sticker_colors[edge_sticker_positions_left[cube->edge_permutation[i]]] = color0;
			} else if(color1>>1 == BLUE>>1){
				sticker_colors[edge_sticker_positions_left[cube->edge_permutation[i]]] = color0;
			} else if(color0>>1 == BLUE>>1){
				sticker_colors[edge_sticker_positions_left[cube->edge_permutation[i]]] = color1;
			}
		} else {
			if(color1>>1 == RED>>1){
				sticker_colors[edge_sticker_positions_left[cube->edge_permutation[i]]] = color0;
			} else if(color0>>1 == RED>>1){
				sticker_colors[edge_sticker_positions_left[cube->edge_permutation[i]]] = color1;
			} else if(color1>>1 == BLUE>>1){
				sticker_colors[edge_sticker_positions_left[cube->edge_permutation[i]]] = color1;
			} else if(color0>>1 == BLUE>>1){
				sticker_colors[edge_sticker_positions_left[cube->edge_permutation[i]]] = color0;
			}
		}
	}

	for(i = 0; i < 8; i++){
		if(corner_sticker_positions_left[cube->corner_permutation[i]] == -1){
			continue;
		}
		sticker_colors[corner_sticker_positions_left[cube->corner_permutation[i]]] = corner_colors[i][(corner_orientation_offsets_left[cube->corner_permutation[i]] - cube->corner_orientation[i])%3];
	}

	printf("\\drawleft");
	for(i = 0; i < 9; i++){
		printf("{%s}", color_strings[sticker_colors[i]]);
	}
	printf("\n");
}

void gen_front_face(rubiks_cube *cube){
	int i;
	int color0;
	int color1;
	int orig_color0;
	int orig_color1;
	int sticker_colors[9];

	for(i = 0; i < 9; i++){
		sticker_colors[i] = BLACK;
	}

	sticker_colors[4] = BLUE;
	for(i = 0; i < 12; i++){
		if(edge_sticker_positions_front[cube->edge_permutation[i]] == -1){
			continue;
		}
		orig_color0 = edge_colors[cube->edge_permutation[i]][0];
		orig_color1 = edge_colors[cube->edge_permutation[i]][1];
		color0 = edge_colors[i][0];
		color1 = edge_colors[i][1];

		if(cube->edge_orientation[i] == 0){
			if(color1>>1 == BLUE>>1){
				sticker_colors[edge_sticker_positions_front[cube->edge_permutation[i]]] = color1;
			} else if(color0>>1 == BLUE>>1){
				sticker_colors[edge_sticker_positions_front[cube->edge_permutation[i]]] = color0;
			} else if(color1>>1 == RED>>1){
				sticker_colors[edge_sticker_positions_front[cube->edge_permutation[i]]] = color0;
			} else if(color0>>1 == RED>>1){
				sticker_colors[edge_sticker_positions_front[cube->edge_permutation[i]]] = color1;
			}
		} else {
			fprintf(stderr, "WTF %d %d\n", color0, color1);
			if(color1>>1 == BLUE>>1){
				fprintf(stderr, "HIIII %d\n", i);
				sticker_colors[edge_sticker_positions_front[cube->edge_permutation[i]]] = color0;
			} else if(color0>>1 == BLUE>>1){
				sticker_colors[edge_sticker_positions_front[cube->edge_permutation[i]]] = color1;
			} else if(color1>>1 == RED>>1){
				fprintf(stderr, "Hi1\n");
				sticker_colors[edge_sticker_positions_front[cube->edge_permutation[i]]] = color1;
			} else if(color0>>1 == RED>>1){
				fprintf(stderr, "Hi2\n");
				sticker_colors[edge_sticker_positions_front[cube->edge_permutation[i]]] = color0;
			}
		}
	}

	for(i = 0; i < 8; i++){
		if(corner_sticker_positions_front[cube->corner_permutation[i]] == -1){
			continue;
		}
		sticker_colors[corner_sticker_positions_front[cube->corner_permutation[i]]] = corner_colors[i][(corner_orientation_offsets_front[cube->corner_permutation[i]] - cube->corner_orientation[i])%3];
	}

	printf("\\drawright");
	for(i = 0; i < 9; i++){
		printf("{%s}", color_strings[sticker_colors[i]]);
	}
	printf("\n");
}

void display_cube(rubiks_cube *cube){
	printf("%s", image_start);
	gen_top_face(cube);
	gen_left_face(cube);
	gen_front_face(cube);
	printf("%s", image_end);
}

int main(int argc, char **argv){
	rubiks_cube *cube;

	cube = create_cube();
	printf("%s", document_start);
	display_cube(cube);
	make_move(cube, FRONT);
	display_cube(cube);
	make_move(cube, LEFT);
	display_cube(cube);
	undo_move(cube, FRONT);
	display_cube(cube);
	undo_move(cube, LEFT);
	display_cube(cube);
	free(cube);
	printf("%s", document_end);

	return 0;
}

