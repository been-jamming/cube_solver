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
			} else if(color0>>1 == WHITE>>1){
				sticker_colors[edge_sticker_positions_left[cube->edge_permutation[i]]] = color1;
			} else if(color1>>1 == WHITE>>1){
				sticker_colors[edge_sticker_positions_left[cube->edge_permutation[i]]] = color0;
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
			} else if(color0>>1 == WHITE>>1){
				sticker_colors[edge_sticker_positions_left[cube->edge_permutation[i]]] = color0;
			} else if(color1>>1 == WHITE>>1){
				sticker_colors[edge_sticker_positions_left[cube->edge_permutation[i]]] = color1;
			}
		}
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

	for(i = 0; i < 12; i++){
		if(edge_sticker_positions_front[i] == -1){
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
			} else if(color0>>1 == WHITE>>1){
				sticker_colors[edge_sticker_positions_front[cube->edge_permutation[i]]] = color1;
			} else if(color1>>1 == WHITE>>1){
				sticker_colors[edge_sticker_positions_front[cube->edge_permutation[i]]] = color0;
			}
		} else {
			if(color1>>1 == BLUE>>1){
				sticker_colors[edge_sticker_positions_front[cube->edge_permutation[i]]] = color0;
			} else if(color0>>1 == BLUE>>1){
				sticker_colors[edge_sticker_positions_front[cube->edge_permutation[i]]] = color1;
			} else if(color1>>1 == RED>>1){
				sticker_colors[edge_sticker_positions_front[cube->edge_permutation[i]]] = color1;
			} else if(color0>>1 == RED>>1){
				sticker_colors[edge_sticker_positions_front[cube->edge_permutation[i]]] = color0;
			} else if(color0>>1 == WHITE>>1){
				sticker_colors[edge_sticker_positions_front[cube->edge_permutation[i]]] = color0;
			} else if(color1>>1 == WHITE>>1){
				sticker_colors[edge_sticker_positions_front[cube->edge_permutation[i]]] = color1;
			}
		}
	}

	printf("\\drawright");
	for(i = 0; i < 9; i++){
		printf("{%s}", color_strings[sticker_colors[i]]);
	}
	printf("\n");
}

int main(int argc, char **argv){
	rubiks_cube *cube;

	cube = create_cube();
	//make_move(cube, FRONT);
	//make_move(cube, LEFT);
	//undo_move(cube, FRONT);
	undo_move(cube, UP);
	printf("%s", document_start);
	printf("%s", image_start);
	gen_top_face(cube);
	gen_left_face(cube);
	gen_front_face(cube);
	printf("%s", image_end);
	printf("%s", document_end);
	free(cube);

	return 0;
}

