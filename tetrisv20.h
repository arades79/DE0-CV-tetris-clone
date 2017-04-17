//PLEASE MAKE A COPY BEFORE EDITING SO NOTHING BECOMES BROKEN

#ifndef TETRIS_HEADER
#define TETRIS_HEADER

#include "tetrisv20_graphics.h"

//DEBUG
//#include <stdio.h>

void itoa(unsigned int input, char string[])
{
	int i = 0;

	do {
		string[i] = (char)(input % 10) + '0';
		input /= 10;
		i++;
	} while (input);
	string[i] = '\0';

	//DEBUG
	//printf("%s\n", string);

	char c;

	i--;

	for (int j = 0; j <= i; j++, i--) {
		c = string[j];
		string[j] = string[i];
		string[i] = c;

	}

}




// sets grid to be a defined color as well as have a border for collision detection purposes
void grid_setup()
{
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < 12; j++) {
			Game.grid[i][j] = GAME_COLOR+FRAME_COLOR;
		}
	}
	for (int i = 0; i < 24; i++) {
		for (int j = 1; j < 11; j++) {
			Game.grid[i][j] = GAME_COLOR;
		}
	}
}

//puts the color of a piece as the relevent elements of the piece, makes changing shapes easier
void piece_setup(Tetromino *piece)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if(piece->shape[i][j] != 0)
			piece->shape[i][j] = piece->color;
		}
	}
}

//puts a piece on the grid, or takes it out of the grid if erase_mode is true
void draw_piece(Tetromino *piece, bool erase_mode)
{
	short x1 = piece->pos_x, x2 = x1 + 4;
	short y1 = piece->pos_y, y2 = y1 + 4;

	if (erase_mode) {
		for (int y = y1; y < y2; y++) {
			for (int x = x1; x < x2; x++) {
				if(!((Game.grid[y][x] == Game.active_piece_ptr->shape[y-y1][x-x1] || Game.grid[y][x] == GAME_COLOR) && piece->shadow == 0)) {
				Game.grid[y][x] -= piece->shape[y - y1][x - x1];
			}
			}
		}
	} else {
		for (int y = y1; y < y2; y++) {
			for (int x = x1; x < x2; x++) {
				if (Game.grid[y][x] == (piece->shadow + GAME_COLOR) && piece->shadow > 0 && piece->shape[y-y1][x-x1] > 0) {
					Game.grid[y][x] -= piece->shadow;
				}
				Game.grid[y][x] += piece->shape[y - y1][x - x1];

			}
		}
	}
}

//tries lowers a piece one block, if it can't, it begins locking, if it can it draws the result
void drop(Tetromino *piece)
{

        //first we erase the piece where it was
	draw_piece(piece, TRUE);
	piece->pos_y++;
        //then we place the piece where we want it to go
	draw_piece(piece, FALSE);

	short x1 = piece->pos_x, x2 = x1 + 4;
	short y1 = piece->pos_y, y2 = y1 + 4;

        //collision detection loop sees if by lowering the piece we hit another piece
	for (int y = y1; y < y2; y++) {
		for (int x = x1; x < x2; x++) {
			if ((Game.grid[y][x]) > (piece->color + GAME_COLOR) && piece->shape[y - y1][x - x1] > 0 ) {

                                //if the piece caused a collision, we erase it, and move it back up
				draw_piece(piece, TRUE);
				piece->pos_y--;
				draw_piece(piece, FALSE);
                                //we change the flags to tell the main loop that we can't drop anymore
				Game.piece_falling = FALSE;
				Game.piece_locking = TRUE;
				return;

			}




		}
	}

	//if no collision occured we make sure the flags reflect that more drops are possible
	Game.piece_locking = FALSE;
	Game.piece_falling = TRUE;


}

//drops the piece as low as it can by watching the falling flag
void hard_drop(Tetromino *piece)
{
	while (Game.piece_falling) {
		drop(piece);
	}
}


//
static void draw_shadow(Tetromino *piece, bool erase) {
	Tetromino temp;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp.shape[j][i] = (piece->shape[j][i]);
		}
	}

	temp.pos_x = piece->pos_x;
	temp.pos_y = piece->pos_y;
	temp.color = piece->shadow;
	temp.shadow = 0;
	piece_setup(&temp);

	if(erase && Game.piece_falling) {
		temp.pos_y = piece->pos_s;
		draw_piece(&temp,TRUE);
	} else {

	draw_piece(piece,TRUE);
	draw_piece(&temp,FALSE);
	hard_drop(&temp);

	Game.piece_falling = TRUE;
	Game.piece_locking = FALSE;
	piece->pos_s = temp.pos_y;

	draw_piece(piece,FALSE);
}
}

//prepares a piece to be dropped by drawing a new one at the top
void spawn(Tetromino *piece)
{

	short x1 = 4, x2 = x1 + 4;
	short y1 = 0, y2 = y1 + 4;

	for (int y = y1; y < y2; y++) {
		for (int x = x1; x < x2; x++) {
			if (Game.grid[y][x] > GAME_COLOR ) {

                                //DEBUG
				//printf("game over at %d,%d\n", x, y );

                                Game.piece_falling = FALSE;
				Game.piece_locking = FALSE;
				Game.active_piece_ptr = FALSE;
				Game.game_over = TRUE;

                                return;
			}
		}
	}

	piece->pos_x = 4;
	piece->pos_y = 0;
	Game.active_piece_ptr = piece;
	Game.piece_falling = TRUE;

	draw_piece(piece, FALSE);
	draw_shadow(piece,FALSE);
}

//tries to move the piece right with a routine very similar to the drop routine above
bool moveRight(Tetromino *piece)
{
	if (Game.piece_falling) {
		draw_shadow(piece,TRUE);
	}
	draw_piece(piece, TRUE);
	piece->pos_x++;
	short x1 = piece->pos_x, x2 = x1 + 4;
	short y1 = piece->pos_y, y2 = y1 + 4;
	draw_piece(piece, FALSE);

	for (int y = y1; y < y2; y++) {
		for (int x = x1; x < x2; x++) {
			if (Game.grid[y][x] > (piece->color + GAME_COLOR) && piece->shape[y - y1][x - x1] > 0) {

				draw_piece(piece, TRUE);
				piece->pos_x--;
				draw_piece(piece, FALSE);

				if (Game.piece_falling) {
					draw_shadow(piece, FALSE);
				}

				return FALSE;

			}
		}
	}

	if (Game.piece_falling) {
		draw_shadow(piece,FALSE);
	}

	return TRUE;
}

//tries to move the piece left with a routine very similar to the drop routine above
bool moveLeft(Tetromino *piece)
{
	if(Game.piece_falling) {
		draw_shadow(piece, TRUE);
	}
	draw_piece(piece, TRUE);
	piece->pos_x--;
	short x1 = piece->pos_x, x2 = x1 + 4;
	short y1 = piece->pos_y, y2 = y1 + 4;
	draw_piece(piece, FALSE);

	for (int y = y1; y < y2; y++) {
		for (int x = x1; x < x2; x++) {
			if (Game.grid[y][x] > (piece->color + GAME_COLOR) && piece->shape[y - y1][x - x1] > 0) {

				draw_piece(piece, TRUE);
				piece->pos_x++;
				draw_piece(piece, FALSE);
				if (Game.piece_falling) {
					draw_shadow(piece, FALSE);
				}
				return FALSE;

			}
		}
	}
		if (Game.piece_falling) {
			draw_shadow(piece, FALSE);
		}
	return TRUE;
}

//makes sure the piece is actually at the bottom by trying to drop it first, then locks the piece
void lock(Tetromino *piece)
{
        drop(piece);

        if (Game.piece_locking) {
		piece->pos_s = piece->pos_y;
                Game.active_piece_ID = 0;
        	Game.piece_locking = FALSE;
        } else {
		draw_shadow(piece,FALSE);
	}

}

//tries to rotate the piece clockwise once using a temporary matrix so the change can be undone
void rotate(Tetromino *piece)
{
	//defining a temporary Tetromino so we can keep the previous state of the piece later on
	Tetromino temp;

	//the temp will inherit the values of the piece passed into the function
	temp.pos_x = piece->pos_x, temp.pos_y = piece->pos_y, temp.color = piece->color;
	 temp.pos_s = piece->pos_s; temp.shadow = piece->shadow;

	short x1 = piece->pos_x, x2 = x1 + 4;
	short y1 = piece->pos_y, y2 = y1 + 4;

	if (Game.piece_falling) {
		draw_shadow(piece, TRUE);
	}

	draw_piece(piece, TRUE);

        //these loops set the shape of the temp Tetromino to the piece shape rotated clockwise
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp.shape[j][i] = piece->shape[3 - i][j];
		}
	}

		draw_piece(&temp, FALSE);


        //check collision and undo if a collision occured
	for (int y = y1; y < y2; y++) {

		for (int x = x1; x < x2; x++) {

			if (Game.grid[y][x] > (temp.color + GAME_COLOR) && temp.shape[y - y1][x - x1] > 0) {

				draw_piece(&temp, TRUE);
				draw_piece(piece, FALSE);
				if (Game.piece_falling) {
					draw_shadow(piece, FALSE);
				}
				return;
			}
		}
	}

        //if no collision happened we make the rotated temp shape the new piece shape
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			piece->shape[i][j] = temp.shape[i][j];
		}
	}

	if (Game.piece_falling) {
		draw_shadow(piece, FALSE);
	}
}



//clears a row that has been completed, and drops all other rows above it
static void row_clear(short row)
{
	for (int i = 1; i < 11; i++) {
		Game.grid[row][i] = 0;
	}
	for (int j = row; j > 0; j--) {
		for (int i = 1; i < 11; i++) {
			Game.grid[j][i] = Game.grid[j - 1][i];
		}
	}
}


//checks for any completed rows and clear them
short check_rows(void)
{
	short row_full;
	short row_clears = 0;

	for (int j = 23; j > 1; j--) {
		row_full = 1;
		for (int i = 0; i < 12; i++) {
			if (Game.grid[j][i] == 0) {

                                //DEBUG
				//printf("row %d not full\n", j );

				row_full = 0;
				break;
			}
		}

                //DEBUG
		//printf("row_product of row %d is %d\n", j, row_product );

		if (row_full) {

                        //DEBUG
			//printf("attempting to clear row %d\n", j );

			row_clear(j);
			row_clears++;
			j = 24;

		}
	}

	return row_clears;
}

//very quick and efficient PRNG algorithm
int quickrand(int x)
{
	x += (x * x) | 5;
	return x;
}






//end of .h file
#endif
