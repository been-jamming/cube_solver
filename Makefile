default:
	gcc edge_book_gen.c cube.c -o cube_book_gen
	./cube_book_gen > test.tex
	tectonic test.tex
