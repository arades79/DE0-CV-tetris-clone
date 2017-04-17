//please make a copy before editing anything
//ex: download tetrisvxx.c and tetrisvxx.h and rename them tetrisvxx_goalOfEdit.c tetrisvxx_goalOfEdit.h

#include "tetrisv20.h" //must be in same directory as this file or else must be entire path

#define CONTROLS_Y 155

int main(int argc, char const *argv[])
{

	//seed for RNG, we should collect this through input using a timer or something, but for now it's hard coded
	unsigned random;
	//flag for tracking input, read descrioption on
	short L_R_button_pressed = 0;
	bool rotate_button_pressed = FALSE;

	//these are all the relevent hardware pointers used by the main program
	volatile int *timer_status = (volatile int*)0xFF202000;
	volatile int *KEY_ptr = (volatile int*)0xFF200050;
	volatile int *SW_ptr = (volatile int*)0xFF200040;
	volatile int *LED_ptr = (volatile int*)0xFF200000;


	char score_str[13];
	char speed_str[4];
	char lock_str[2];
	char multiplier_str[5];

	short rows = check_rows();
	bool flicker = FALSE;
	unsigned int score;
	unsigned int new_score;
	unsigned int multiplier;


	//declaration of all tetrominos, they are static to ensure functions in the header can edit values
	static Tetromino I =
	{
		{ { 0, 0, 0, 0 },
		    { 1, 1, 1, 1 },
		    { 0, 0, 0, 0 },
		    { 0, 0, 0, 0 } },   //shape
		0,                      //pos_x
		0,                      //pos_y
		I_PIECE_COLOR,          //color
		I_SHADOW_COLOR,         //shadow
		0
	};

	static Tetromino L =
	{
		{ { 0, 0, 0, 0 },
		    { 0, 0, 1, 0 },
		    { 1, 1, 1, 0 },
		    { 0, 0, 0, 0 } },   //shape
		0,                      //pos_x
		0,                      //pos_y
		L_PIECE_COLOR,          //color
		L_SHADOW_COLOR,         //shadow
		0
	};

	static Tetromino O =
	{
		{ { 0, 0, 0, 0 },
		    { 0, 1, 1, 0 },
		    { 0, 1, 1, 0 },
		    { 0, 0, 0, 0 } },   //shape
		0,                      //pos_x
		0,                      //pos_y
		O_PIECE_COLOR,          //color
		O_SHADOW_COLOR,         //shadow
		0
	};

	static Tetromino S =
	{
		{ { 0, 0, 0, 0 },
		    { 0, 1, 1, 0 },
		    { 1, 1, 0, 0 },
		    { 0, 0, 0, 0 } },   //shape
		0,                      //pos_x
		0,                      //pos_y
		S_PIECE_COLOR,          //color
		S_SHADOW_COLOR,         //shadow
		0
	};

	static Tetromino T =
	{
		{ { 0, 0, 0, 0 },
		    { 1, 1, 1, 0 },
		    { 0, 1, 0, 0 },
		    { 0, 0, 0, 0 } },   //shape
		0,                      //pos_x
		0,                      //pos_y
		T_PIECE_COLOR,          //color
		T_SHADOW_COLOR,         //shadow
		0
	};

	static Tetromino J =
	{
		{ { 0, 0, 0, 0 },
		    { 0, 1, 0, 0 },
		    { 0, 1, 1, 1 },
		    { 0, 0, 0, 0 } },   //shape
		0,                      //pos_x
		0,                      //pos_y
		J_PIECE_COLOR,          //color
		J_SHADOW_COLOR,         //shadow
		0
	};

	static Tetromino Z =
	{
		{ { 0, 0, 0, 0 },
		    { 1, 1, 0, 0 },
		    { 0, 1, 1, 0 },
		    { 0, 0, 0, 0 } },   //shape
		0,                      //pos_x
		0,                      //pos_y
		Z_PIECE_COLOR,          //color
		Z_SHADOW_COLOR,         //shadow
		0
	};


	//this puts the color of the pieces into the shape of it, this makes tweaking color easier
	piece_setup(&I);
	piece_setup(&L);
	piece_setup(&O);
	piece_setup(&S);
	piece_setup(&T);
	piece_setup(&J);
	piece_setup(&Z);


	//array of pointers to tetrominos for the random number to help select
	//since we use pointers this should only be 28 bytes instead of 340 bytes that would be required to hold all pieces
	static Tetromino *piece_array[7] = { &I, &L, &O, &S, &T, &J, &Z };

	//this infinite loop ensures you can keep playing games without reset
	for (;; ) {

		clear_screen(0);
		clear_char();

		tetris_title();
		draw_string(25, 55, "Press any button to continue");
		draw_string(14, 42, "Lock Speed");
		draw_string(30, 42, "Drop Speed");
		draw_string(45, 42, "Left");
		draw_string(50, 42, "Drop");
		draw_string(55, 42, "Rot.");
		draw_string(60, 42, "Right");
		draw_string(34, 36, "Controls:");
		outline(54, 140, 260, 175);
		switch_draw(64, CONTROLS_Y);
		switch_draw(74, CONTROLS_Y);
		switch_draw(84, CONTROLS_Y);
		switch_draw(105, CONTROLS_Y);
		switch_draw(115, CONTROLS_Y);
		switch_draw(125, CONTROLS_Y);
		switch_draw(135, CONTROLS_Y);
		switch_draw(145, CONTROLS_Y);
		switch_draw(155, CONTROLS_Y);
		switch_draw(165, CONTROLS_Y);
		pushbutton_draw(185, CONTROLS_Y);
		pushbutton_draw(205, CONTROLS_Y);
		pushbutton_draw(225, CONTROLS_Y);
		pushbutton_draw(245, CONTROLS_Y);

		*timer_status = 0;

		set_timer(TITLE_FLICKER_SPEED);
		while (!*(KEY_ptr)) {
			random %= SHORT_MAX;
			random++;
			if (*timer_status == TIMER_DONE) {
				if (flicker) {
					draw_string(25, 55, "Press any button to continue");
					flicker = FALSE;
					*timer_status = 0;
					set_timer(TITLE_FLICKER_SPEED);
				} else if (!flicker) {
					draw_string(25, 55, "                                ");
					flicker = TRUE;
					*timer_status = 0;
					set_timer(TITLE_FLICKER_SPEED);
				}
			}

		}
		while (*(KEY_ptr)) ;

		//the PRNG algorithm is best if allowed to "warm up"
		random = quickrand(random);
		random = quickrand(random);
		random = quickrand(random);
		random = quickrand(random);
		random = quickrand(random);
		random = quickrand(random);

		clear_char();
		clear_screen(BACKGROUND_COLOR);

		box(190, 14, 250, 215);
		box(79, 14, 180, 215);
		outline(79, 14, 179, 214);
		outline(190, 14, 249, 214);
		draw_string(48, 5, "Next Piece:");
		draw_string(48, 20, "Score:");
		draw_string(50, 24, "0");
		draw_string(48, 38, "Lock Speed:");
		line(210, 133, 224, 133, OUTLINE_COLOR_2);
		draw_string(53, 34, "128");
		draw_string(48, 28, "Drop Speed:");
		line(214, 173, 220, 173, OUTLINE_COLOR_2);
		draw_string(54, 44, "8");
		draw_string(48, 48, "Multiplier:");
		draw_string(52, 52, "X1");

		//this preps the grid for a new game by clearing any blocks and making sure the frame is intact
		grid_setup();

		Game.game_over = FALSE;
		score = 0;
		multiplier = 1;

		//this infinite loop is the primary game loop, where all I/O and calculations happen
		for (;; ) {

			itoa(((*SW_ptr) % 0x80) + 1, speed_str);
			itoa(((*SW_ptr) >> 7) + 1, lock_str);
			draw_string(53, 32, "    ");
			draw_string(53, 32, speed_str);
			draw_string(54, 42, "  ");
			draw_string(54, 42, lock_str);

			//if the falling flag is up, the piece should fall
			if (Game.piece_falling) {

				//printf("I broke at falling if\n");

				*(LED_ptr) = *(SW_ptr);
				set_timer(MIN_DROP_SPEED - ((*(SW_ptr) % 0x80) * DROP_SPEED_INC));

				if (L_R_button_pressed <= 0) {
					L_R_button_pressed = 0;
				} else {
					L_R_button_pressed--;
				}

				do {

					//DEBUG
					//printf("I broke at falling do/while\n");

					/*this block handles the input from the DE0-CV keys, it goes
					   through the switch statement to handle which button was
					   pressed until the drop timer runs out, updating the
					   output after each button press */
					switch (*(KEY_ptr)) {

					case KEY_2: // middle-left button instantly drops piece as low as possible
						if (!rotate_button_pressed) {
							hard_drop((Game.active_piece_ptr));
							grid_draw();
							rotate_button_pressed = TRUE;
						}
						break;

					case KEY_1: // middle-right button rotates piece clockwise
						if (!rotate_button_pressed) {
							rotate(Game.active_piece_ptr);
							grid_draw();
							rotate_button_pressed = TRUE;
							//random++;
						}
						break;

					case KEY_0: //rightmost button moves piece right
						if (!L_R_button_pressed) {
							moveRight((Game.active_piece_ptr));
							grid_draw();
							L_R_button_pressed += 2;
							//random++;

						}
						break;

					case KEY_3: //leftmost button moves piece left
						if (!L_R_button_pressed) {
							moveLeft((Game.active_piece_ptr));
							grid_draw();
							L_R_button_pressed += 2;
						}
						break;

					default:
						/*this ensures that the button inputs are only counted
						   once per press, this is important at lower speeds
						   since the piece can move too quickly to get precision */
						L_R_button_pressed = 0;
						rotate_button_pressed = FALSE;
						break;
					}

				} while (*timer_status != TIMER_DONE);

				//DEBUG
				//printf("I broke at drop\n");

				//reset the timer, attempt to lower the piece, and update the output
				*timer_status = 0;
				drop(Game.active_piece_ptr);
				grid_draw();

				/*if the locking flag is up, the piece should lock, we allow input during
				   the locking phase in order to allow you to fit the piece into 'lips'
				   or other awkward locations, or get a piece to drop further down */
			} else if (Game.piece_locking) {

				//DEBUG
				//printf("I broke at locking if\n");

				*(LED_ptr) = *(SW_ptr);
				set_timer(MIN_LOCK_SPEED - ((*(SW_ptr) >> 7) * LOCK_SPEET_INC));

				L_R_button_pressed = FALSE;

				do {

					//DEBUG
					//printf("I broke at locking do/while\n");

					switch (*(KEY_ptr)) {

					case KEY_2: // middle-left button instantly drops piece as low as possible
						if (!rotate_button_pressed) {
							hard_drop((Game.active_piece_ptr));
							grid_draw();
							rotate_button_pressed = TRUE;
						}
						break;

					case KEY_1: // middle-right button rotates piece clockwise
						if (!rotate_button_pressed) {
							rotate(Game.active_piece_ptr);
							grid_draw();
							rotate_button_pressed = TRUE;
							//random++;
						}
						break;

					case KEY_0: //rightmost button moves piece right
						if (!L_R_button_pressed) {
							moveRight((Game.active_piece_ptr));
							grid_draw();
							L_R_button_pressed = TRUE;
							//random++;

						}
						break;

					case KEY_3: //leftmost button moves piece left
						if (!L_R_button_pressed) {
							moveLeft((Game.active_piece_ptr));
							grid_draw();
							L_R_button_pressed = TRUE;
						}
						break;

					default:
						/*this ensures that the button inputs are only counted
						   once per press, this is important at lower speeds
						   since the piece can move too quickly to get precision */
						L_R_button_pressed = FALSE;
						rotate_button_pressed = FALSE;
						break;
					}

				} while (*timer_status != TIMER_DONE);

				//DEBUG
				//printf("I broke at lock\n");


				lock(Game.active_piece_ptr);

				if (!(Game.piece_locking || Game.piece_falling)) {
					rows = check_rows();
					new_score = 0;

					if (rows) {
						new_score = ((((32 + (random % 33)) << (rows + multiplier)) * multiplier));
						multiplier++;
					} else if (multiplier > 1) {

						multiplier--;

					}

					draw_string(50, 24, "             ");
					score += new_score;
					itoa(score, score_str);
					itoa(multiplier, multiplier_str);
					draw_string(50, 24, score_str);
					draw_string(53, 52, multiplier_str);
				}

				grid_draw();

			} else if (Game.game_over) {
				clear_screen(0);
				break;
			}else {

				//DEBUG
				//printf("I broke at spawn\n");

				spawn(piece_array[random % 7]);

				random = quickrand(random);

				next_grid(piece_array[random % 7]);

				grid_draw();
			}

		}
	}
	return 0;
}

/*
   Get the value to check for locking(1-8) and drop speed(1-128) and convert it to a string and display it
 */
