#include <stdlib.h>
#include <stdio.h>
#include "cube.h"

enum cube_color{
	GRAY,
	BLUE,
	GREEN,
	WHITE,
	YELLOW,
	ORANGE,
	RED
};

static const char *color_names[7] = {"gray", "blue", "green", "white", "yellow", "orange", "red"};

static const char *document_start = "\\documentclass{article}\n\\usepackage{tikz}\n\\input{template.tex}\n\\begin{document}\n";
static const char *image_start = "\\begin{tikzpicture}[scale=2.0]\n";
static const char *image_end = "\\end{tikzpicture}\n";
static const char *document_end = "\\end{document}\n";

enum cube_color edge_colors[12][2] = {
	{BLUE, WHITE}, {BLUE, ORANGE}, {BLUE, YELLOW}, {BLUE, RED},
	{WHITE, ORANGE}, {YELLOW, ORANGE}, {YELLOW, RED}, {WHITE, RED},
	{GREEN, WHITE}, {GREEN, ORANGE}, {GREEN, YELLOW}, {GREEN, RED}
};
enum cube_color corner_colors[8][3] = {
	{WHITE, BLUE, ORANGE}, {ORANGE, BLUE, YELLOW}, {YELLOW, BLUE, RED}, {RED, BLUE, WHITE},
	{ORANGE, GREEN, WHITE}, {YELLOW, GREEN, ORANGE}, {RED, GREEN, YELLOW}, {WHITE, GREEN, RED}
};

struct cube_orientation{
	enum cube_color right_face;
	enum cube_color top_face;
	enum cube_color left_face;
};

static int cube_orientation_to_id(struct cube_orientation orientation){
	int corner_id;
	int i;

	for(corner_id = 0; corner_id < 8; i++){
		if(orientation.right_face != corner_colors[corner_id][0] &&
		   orientation.right_face != corner_colors[corner_id][1] &&
		   orientation.right_face != corner_colors[corner_id][2]){
			continue;
		}
		if(orientation.top_face != corner_colors[corner_id][0] &&
		   orientation.top_face != corner_colors[corner_id][1] &&
		   orientation.top_face != corner_colors[corner_id][2]){
			continue;
		}
		if(orientation.left_face != corner_colors[corner_id][0] &&
		   orientation.left_face != corner_colors[corner_id][1] &&
		   orientation.left_face != corner_colors[corner_id][2]){
			continue;
		}
		break;
	}

	if(corner_id == 8){
		return -1;
	}

	for(i = 0; i < 3; i++){
		if(orientation.right_face == corner_colors[corner_id][i]){
			return 3*corner_id + i;
		}
	}

	return -1;
}

static int top_corner_indicies[24][4] = {
	{0, 1, 2, 3}, {0, 4, 5, 1}, {0, 3, 7, 4},
	{1, 2, 3, 0}, {1, 5, 6, 2}, {1, 0, 4, 5},
	{2, 3, 0, 1}, {2, 6, 7, 3}, {2, 1, 5, 6},
	{3, 0, 1, 2}, {3, 7, 4, 0}, {3, 2, 6, 7},
	{4, 7, 6, 5}, {4, 0, 3, 7}, {4, 5, 1, 0},
	{5, 4, 7, 6}, {5, 1, 0, 4}, {5, 6, 2, 1},
	{6, 5, 4, 7}, {6, 2, 1, 5}, {6, 7, 3, 2},
	{7, 6, 5, 4}, {7, 3, 2, 6}, {7, 4, 0, 3}
};

static int top_edge_indicies[24][4] = {
	{0, 1, 2, 3}, {1, 4, 9, 5}, {4, 0, 7, 8},
	{1, 2, 3, 0}, {2, 5, 10, 6}, {5, 1, 4, 9},
	{2, 3, 0, 1}, {3, 6, 11, 7}, {6, 2, 5, 10},
	{3, 0, 1, 2}, {0, 7, 8, 4}, {7, 3, 6, 11},
	{9, 8, 11, 10}, {8, 4, 0, 7}, {4, 9, 5, 1},
	{10, 9, 8, 11}, {9, 5, 1, 4}, {5, 10, 6, 2},
	{11, 10, 9, 8}, {10, 6, 2, 5}, {6, 11, 7, 3},
	{8, 11, 10, 9}, {11, 7, 3, 6}, {7, 8, 4, 0}
};

static int top_corner_color_positions[4] = {8, 2, 0, 6};
static int left_corner_color_positions[4] = {2, 8, 6, 0};
static int right_corner_color_positions[4] = {0, 2, 8, 6};

static int top_edge_color_positions[4] = {7, 3, 1, 5};
static int left_edge_color_positions[4] = {1, 5, 7, 3};
static int right_edge_color_positions[4] = {3, 1, 5, 7};

static enum cube_color face_pair(enum cube_color color){
	return ((color - 1)&0x6) + 1;
}

static enum cube_color opposite_face(enum cube_color color){
	switch(color){
		case BLUE:
			return GREEN;
		case GREEN:
			return BLUE;
		case WHITE:
			return YELLOW;
		case YELLOW:
			return WHITE;
		case ORANGE:
			return RED;
		case RED:
			return ORANGE;
		default:
			return GRAY;
	}
}

static enum cube_color edge_color(int edge_id, int edge_orientation, enum cube_color face, enum cube_color adjacent){
	enum cube_color color0;
	enum cube_color color1;

	if(face_pair(adjacent) == face_pair(BLUE)){
		return edge_color(edge_id, !edge_orientation, adjacent, face);
	}

	if(face_pair(face) == face_pair(BLUE)){
		if(face_pair(edge_colors[edge_id][0]) == face_pair(BLUE)){
			return (face_pair(adjacent) == face_pair(edge_colors[edge_id][1])) == edge_orientation ? edge_colors[edge_id][1] : edge_colors[edge_id][0];
		} else if(face_pair(edge_colors[edge_id][1]) == face_pair(BLUE)){
			return (face_pair(adjacent) == face_pair(edge_colors[edge_id][0])) == edge_orientation ? edge_colors[edge_id][0] : edge_colors[edge_id][1];
		} else if(face_pair(edge_colors[edge_id][0]) == face_pair(adjacent)){
			return edge_orientation ? edge_colors[edge_id][0] : edge_colors[edge_id][1];
		} else if(face_pair(edge_colors[edge_id][1]) == face_pair(adjacent)){
			return edge_orientation ? edge_colors[edge_id][1] : edge_colors[edge_id][0];
		}
	} else {
		if(face_pair(edge_colors[edge_id][0]) == face_pair(face)){
			return edge_orientation ? edge_colors[edge_id][1] : edge_colors[edge_id][0];
		} else if(face_pair(edge_colors[edge_id][1]) == face_pair(face)){
			return edge_orientation ? edge_colors[edge_id][0] : edge_colors[edge_id][1];
		} else if(face_pair(edge_colors[edge_id][0]) == face_pair(adjacent)){
			return edge_orientation ? edge_colors[edge_id][0] : edge_colors[edge_id][1];
		} else if(face_pair(edge_colors[edge_id][1]) == face_pair(adjacent)){
			return edge_orientation ? edge_colors[edge_id][1] : edge_colors[edge_id][0];
		}
	}

	fprintf(stderr, "Could not compute edge color!\n");
	return -1;
}

static enum cube_color corner_color(int corner_id, int corner_orientation, enum cube_color face, enum cube_color adjacent0, enum cube_color adjacent1){
	int orientation_index;

	for(orientation_index = 0; face_pair(corner_colors[corner_id][orientation_index]) != face_pair(WHITE) && orientation_index < 3; orientation_index++);

	if(orientation_index == 3){
		fprintf(stderr, "Could not compute corner color!\n");
		return -1;
	}

	if(face_pair(adjacent0) == face_pair(WHITE)){
		orientation_index += 1;
	}

	if(face_pair(adjacent1) == face_pair(WHITE)){
		orientation_index += 2;
	}

	return corner_colors[corner_id][(orientation_index + corner_orientation)%3];
}

static void draw_face(rubiks_cube *cube, struct cube_orientation orientation, const char *draw_str, int edge_locations[4], int corner_locations[4]){
	int edge;
	int corner;
	int i;
	int sticker_location;
	int piece_pos;
	enum cube_color adjacent0;
	enum cube_color adjacent1;
	enum cube_color sticker0;
	enum cube_color sticker1;
	enum cube_color sticker2;
	enum cube_color sticker_colors[9];

	for(i = 0; i < 9; i++){
		sticker_colors[i] = GRAY;
	}

	sticker_colors[4] = orientation.top_face;

	for(edge = 0; edge < 12; edge++){
		if(!(cube->edge_mask&(1<<edge))){
			continue;
		}
		sticker0 = edge_colors[cube->edge_permutation[edge]][0];
		sticker1 = edge_colors[cube->edge_permutation[edge]][1];
		if(sticker0 == orientation.top_face ||
		   sticker1 == orientation.top_face){
			if(sticker0 == orientation.right_face ||
			   sticker1 == orientation.right_face){
				piece_pos = 0;
			} else if(sticker0 == orientation.left_face ||
				  sticker1 == orientation.left_face){
				piece_pos = 1;
			} else if(sticker0 == opposite_face(orientation.right_face) ||
				  sticker1 == opposite_face(orientation.right_face)){
				piece_pos = 2;
			} else {
				piece_pos = 3;
			}
			if(piece_pos%2){
				adjacent0 = orientation.left_face;
			} else {
				adjacent0 = orientation.right_face;
			}
			sticker_location = edge_locations[piece_pos];
			sticker_colors[sticker_location] = edge_color(edge, cube->edge_orientation[edge], orientation.top_face, adjacent0);
		}
	}

	for(corner = 0; corner < 8; corner++){
		if(!(cube->corner_mask&(1<<corner))){
			continue;
		}
		sticker0 = corner_colors[cube->corner_permutation[corner]][0];
		sticker1 = corner_colors[cube->corner_permutation[corner]][1];
		sticker2 = corner_colors[cube->corner_permutation[corner]][2];
		if(sticker0 == orientation.top_face ||
		   sticker1 == orientation.top_face ||
		   sticker2 == orientation.top_face){
			if(sticker0 == orientation.right_face ||
			   sticker1 == orientation.right_face ||
			   sticker2 == orientation.right_face){
				if(sticker0 == orientation.left_face ||
				   sticker1 == orientation.left_face ||
				   sticker2 == orientation.left_face){
					piece_pos = 0;
				} else {
					piece_pos = 3;
				}
			} else {
				if(sticker0 == orientation.left_face ||
				   sticker1 == orientation.left_face ||
				   sticker2 == orientation.left_face){
					piece_pos = 1;
				} else {
					piece_pos = 2;
				}
			}
			if(piece_pos%2){
				adjacent0 = orientation.left_face;
				adjacent1 = orientation.right_face;
			} else {
				adjacent0 = orientation.right_face;
				adjacent1 = orientation.left_face;
			}
			sticker_location = corner_locations[piece_pos];
			sticker_colors[sticker_location] = corner_color(corner, cube->corner_orientation[corner], orientation.top_face, adjacent0, adjacent1);
		}
	}

	printf("%s", draw_str);
	for(i = 0; i < 9; i++){
		printf("{%s}", color_names[sticker_colors[i]]);
	}
	printf("\n");
}

void draw_cube(rubiks_cube *cube, struct cube_orientation orientation){
	printf("%s", image_start);
	draw_face(cube, orientation, "\\drawtop", top_edge_color_positions, top_corner_color_positions);
	draw_face(cube, (struct cube_orientation) {.top_face = orientation.left_face, .left_face = orientation.right_face, .right_face = orientation.top_face}, "\\drawleft", left_edge_color_positions, left_corner_color_positions);
	draw_face(cube, (struct cube_orientation) {.top_face = orientation.right_face, .left_face = orientation.top_face, .right_face = orientation.left_face}, "\\drawright", right_edge_color_positions, right_corner_color_positions);
	printf("%s", image_end);
}

int main(int argc, char **argv){
	rubiks_cube *cube;

	cube = create_cube();
	make_move(cube, FRONT);
	make_move(cube, LEFT);
	undo_move(cube, FRONT);
	undo_move(cube, LEFT);
	make_move(cube, FRONT);
	make_move(cube, LEFT);
	undo_move(cube, FRONT);
	undo_move(cube, LEFT);
	make_move(cube, FRONT);
	make_move(cube, LEFT);
	undo_move(cube, FRONT);
	undo_move(cube, LEFT);
	//cube->edge_mask = 0;
	//cube->corner_mask = 1<<4;
	printf("%s", document_start);
	draw_cube(cube, (struct cube_orientation) {.top_face = RED, .left_face = WHITE, .right_face = GREEN});
	printf("%s", document_end);

	free(cube);
	return 0;
}

