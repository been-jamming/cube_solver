#define NUM_MOVES (DOWN3 + 1)

enum rubiks_move{
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	FRONT2,
	BACK2,
	LEFT2,
	RIGHT2,
	UP2,
	DOWN2,
	FRONT3,
	BACK3,
	LEFT3,
	RIGHT3,
	UP3,
	DOWN3
};

typedef enum rubiks_move rubiks_move;

struct rubiks_cube{
	//Given an edge id, outputs its position
	int edge_permutation[12];
	//Given a corner id, outputs its position
	int corner_permutation[8];
	//Given an edge id, outputs its orientation
	int edge_orientation[12];
	//Given a corner id, outputs its orientation
	int corner_orientation[8];
	//Determines whether a piece is included in a cube render
	unsigned int edge_mask;
	unsigned int corner_mask;
};

typedef struct rubiks_cube rubiks_cube;

void make_move(rubiks_cube *cube, rubiks_move move);
void undo_move(rubiks_cube *cube, rubiks_move move);
int cubes_equal(rubiks_cube *cube0, rubiks_cube *cube1);
int edges_equal(rubiks_cube *cube0, rubiks_cube *cube1);
int corners_equal(rubiks_cube *cube0, rubiks_cube *cube1);
rubiks_cube *create_cube();
