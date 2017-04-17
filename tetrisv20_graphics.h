#define PIXEL_BUFFER 0x08000000
#define SHORT_MAX 65536

#define MIN_DROP_SPEED 0x0800
#define DROP_SPEED_INC 0x0010
#define MIN_LOCK_SPEED 0x0400
#define LOCK_SPEET_INC 0x0080
#define TITLE_FLICKER_SPEED 0x0400
#define TIMER_CTRL_START 0x4
#define TIMER_DONE 1

#define KEY_0 1
#define KEY_1 2
#define KEY_2 4
#define KEY_3 8

#define GAME_COLOR 0
#define FRAME_COLOR 0b0000100000100001
#define OUTLINE_COLOR_1 0b1011110111110111
#define OUTLINE_COLOR_2 0xFFFF
#define BACKGROUND_COLOR 0x3ACE
#define TITLE_WHITE 0xFFFF
#define TITLE_GRAY 0x41E8

#define I_PIECE_COLOR 0b0111100010000100
#define L_PIECE_COLOR 0b0001111011100011
#define O_PIECE_COLOR 0b0001100111110111
#define S_PIECE_COLOR 0b0111100111100100
#define T_PIECE_COLOR 0b0000011011110111
#define J_PIECE_COLOR 0b0111100001110111
#define Z_PIECE_COLOR 0b0111101011110111

#define I_SHADOW_COLOR 0b0010000000000000
#define L_SHADOW_COLOR 0b0000000100000000
#define O_SHADOW_COLOR 0b0000000000000100
#define S_SHADOW_COLOR 0b0010000010000000
#define T_SHADOW_COLOR 0b0000000010000100
#define J_SHADOW_COLOR 0b0010000000000100
#define Z_SHADOW_COLOR 0b0010000010000100

#define COLOR_GRADIENT 0b0000100000100001

typedef enum {FALSE, TRUE}bool;

//typedef means that Tetromino is a valid type just like int or char
//you can also use Tetromino* to create a pointer to a Tetromino
typedef struct {
	unsigned short shape[4][4];
	short pos_x;
	short pos_y;
	short color;
	short shadow;
	short pos_s;
}Tetromino;

//game struct keeps track of most flags that functions need access to, defined globally
static struct {
	char active_piece_ID;
	Tetromino* active_piece_ptr;
	bool piece_locking;
	bool piece_falling;
	bool game_over;
	unsigned short grid[25][12];
} Game;

//All for graphics below

void set_timer(int cycles) {
	volatile int *timer_high = (volatile int*)0xFF20200c;
	volatile int *timer_ctrl = (volatile int*)0xFF202004;

        *(timer_high)= (short)cycles;
        *(timer_ctrl) = TIMER_CTRL_START;
}

static int abs(int x) {
        int y = (x >> 31);
        return (x ^ y) - y;
}

static int sgn(int x) {
        return (x>0) - (x<0);
}

//Write a pixel to a defined x and y position to a defined color
static void draw_pixel(int x, int y, short color)
{
	volatile short* pixel_buffer = (volatile short*)(PIXEL_BUFFER + (y << 10) + (x << 1));
	*(pixel_buffer) = color;
}

static void line(int x1, int y1, int x2, int y2, short color) {
        int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

        dx=x2-x1; /* the horizontal distance of the line */
        dy=y2-y1; /* the vertical distance of the line */
        dxabs=abs(dx);
        dyabs=abs(dy);
        sdx=sgn(dx);
        sdy=sgn(dy);
        x=dyabs>>1;
        y=dxabs>>1;
        px=x1;
        py=y1;

        if (dxabs>=dyabs) /* the line is more horizontal than vertical */
        {
                for(i=0; i<dxabs; i++)
                {
                        y+=dyabs;
                        if (y>=dxabs)
                        {
                                y-=dxabs;
                                py+=sdy;
                        }
                        px+=sdx;
                        draw_pixel(px,py,color);
                }
        }
        else /* the line is more vertical than horizontal */
        {
                for(i=0; i<dyabs; i++)
                {
                        x+=dxabs;
                        if (x>=dyabs)
                        {
                                x-=dyabs;
                                px+=sdx;
                        }
                        py+=sdy;
                        draw_pixel(px,py,color);
                }
        }
}

void small_block_draw(short x, short y, short color)
{
	short x2 = x + 3, y2 = y + 3;
	for (int j = y; j < y2; j++) {
		for (int i = x; i < x2; i++) {
			draw_pixel(i, j, color);
		}
	}
}

void tetris_title()
{
	for(int x=115; x<200; x++) {
		for(int y=47; y<75; y++) {
        draw_pixel(x, y,TITLE_WHITE);
    }
	}
	for(int x=143; x<171; x++) {
		for(int y=75; y<103; y++) {
        draw_pixel(x, y,TITLE_WHITE);
    }
	}
	line(113,46,200,46,TITLE_GRAY);
	line(200,46,200,74,TITLE_GRAY);
	line(201,75,171,75,TITLE_GRAY);
	line(171,74,171,102,TITLE_GRAY);
	line(172,103,142,103,TITLE_GRAY);
	line(142,103,142,76,TITLE_GRAY);
	line(143,75,114,75,TITLE_GRAY);
	line(114,75,114,47,TITLE_GRAY);

	small_block_draw(116,51,TITLE_GRAY);
	small_block_draw(120,51,TITLE_GRAY);
	small_block_draw(124,51,TITLE_GRAY);
	small_block_draw(120,55,TITLE_GRAY);			//T
	small_block_draw(120,59,TITLE_GRAY);
	small_block_draw(120,63,TITLE_GRAY);
	small_block_draw(120,67,TITLE_GRAY);

	small_block_draw(132,51,TITLE_GRAY);
	small_block_draw(132,55,TITLE_GRAY);
	small_block_draw(132,59,TITLE_GRAY);
	small_block_draw(132,63,TITLE_GRAY);
	small_block_draw(132,67,TITLE_GRAY);
	small_block_draw(136,51,TITLE_GRAY);		//E
	small_block_draw(136,59,TITLE_GRAY);
	small_block_draw(136,67,TITLE_GRAY);
	small_block_draw(140,51,TITLE_GRAY);
	small_block_draw(140,67,TITLE_GRAY);

	small_block_draw(148,51,TITLE_GRAY);
	small_block_draw(152,51,TITLE_GRAY);
	small_block_draw(152,55,TITLE_GRAY);
	small_block_draw(152,59,TITLE_GRAY);			//T
	small_block_draw(152,63,TITLE_GRAY);
	small_block_draw(152,67,TITLE_GRAY);
	small_block_draw(156,51,TITLE_GRAY);

	small_block_draw(164,51,TITLE_GRAY);
	small_block_draw(164,55,TITLE_GRAY);
	small_block_draw(164,59,TITLE_GRAY);
	small_block_draw(164,63,TITLE_GRAY);
	small_block_draw(164,67,TITLE_GRAY);
	small_block_draw(168,51,TITLE_GRAY);		//R
	small_block_draw(168,59,TITLE_GRAY);
	small_block_draw(172,51,TITLE_GRAY);
	small_block_draw(172,55,TITLE_GRAY);
	small_block_draw(172,63,TITLE_GRAY);
	small_block_draw(172,67,TITLE_GRAY);

	small_block_draw(180,51,TITLE_GRAY);
	small_block_draw(180,55,TITLE_GRAY);
	small_block_draw(180,59,TITLE_GRAY);
	small_block_draw(180,63,TITLE_GRAY);			//I
	small_block_draw(180,67,TITLE_GRAY);

	small_block_draw(188,51,TITLE_GRAY);
	small_block_draw(188,55,TITLE_GRAY);
	small_block_draw(188,59,TITLE_GRAY);
	small_block_draw(188,67,TITLE_GRAY);
	small_block_draw(192,51,TITLE_GRAY);
	small_block_draw(192,59,TITLE_GRAY);		//S
	small_block_draw(192,67,TITLE_GRAY);
	small_block_draw(196,51,TITLE_GRAY);
	small_block_draw(196,59,TITLE_GRAY);
	small_block_draw(196,63,TITLE_GRAY);
	small_block_draw(196,67,TITLE_GRAY);

}

void switch_draw(int x_start, int y_start)
{
	draw_pixel(x_start,y_start,TITLE_WHITE);
	draw_pixel(x_start+1,y_start,TITLE_WHITE);
	draw_pixel(x_start+2,y_start,TITLE_WHITE);
	draw_pixel(x_start+3,y_start,TITLE_WHITE);
	draw_pixel(x_start+4,y_start,TITLE_WHITE);
	draw_pixel(x_start+5,y_start,TITLE_WHITE);
	draw_pixel(x_start,y_start+7,TITLE_WHITE);
	draw_pixel(x_start+1,y_start+7,TITLE_WHITE);
	draw_pixel(x_start+2,y_start+7,TITLE_WHITE);
	draw_pixel(x_start+3,y_start+7,TITLE_WHITE);
	draw_pixel(x_start+4,y_start+7,TITLE_WHITE);
	draw_pixel(x_start+5,y_start+7,TITLE_WHITE);
	draw_pixel(x_start,y_start+1,TITLE_WHITE);
	draw_pixel(x_start,y_start+2,TITLE_WHITE);
	draw_pixel(x_start,y_start+3,TITLE_WHITE);
	draw_pixel(x_start,y_start+4,TITLE_WHITE);
	draw_pixel(x_start,y_start+5,TITLE_WHITE);
	draw_pixel(x_start,y_start+6,TITLE_WHITE);
	draw_pixel(x_start+5,y_start+1,TITLE_WHITE);
	draw_pixel(x_start+5,y_start+2,TITLE_WHITE);
	draw_pixel(x_start+5,y_start+3,TITLE_WHITE);
	draw_pixel(x_start+5,y_start+4,TITLE_WHITE);
	draw_pixel(x_start+5,y_start+5,TITLE_WHITE);
	draw_pixel(x_start+5,y_start+6,TITLE_WHITE);
	draw_pixel(x_start+2,y_start+4,TITLE_WHITE);
	draw_pixel(x_start+2,y_start+5,TITLE_WHITE);
	draw_pixel(x_start+3,y_start+4,TITLE_WHITE);
	draw_pixel(x_start+3,y_start+5,TITLE_WHITE);
}

void pushbutton_draw(int x_start, int y_start)
{
	draw_pixel(x_start,y_start,TITLE_WHITE);
	draw_pixel(x_start+1,y_start,TITLE_WHITE);
	draw_pixel(x_start+2,y_start,TITLE_WHITE);
	draw_pixel(x_start+3,y_start,TITLE_WHITE);
	draw_pixel(x_start+4,y_start,TITLE_WHITE);
	draw_pixel(x_start+5,y_start,TITLE_WHITE);
	draw_pixel(x_start,y_start+5,TITLE_WHITE);
	draw_pixel(x_start+1,y_start+5,TITLE_WHITE);
	draw_pixel(x_start+2,y_start+5,TITLE_WHITE);
	draw_pixel(x_start+3,y_start+5,TITLE_WHITE);
	draw_pixel(x_start+4,y_start+5,TITLE_WHITE);
	draw_pixel(x_start+5,y_start+5,TITLE_WHITE);
	draw_pixel(x_start,y_start+1,TITLE_WHITE);
	draw_pixel(x_start,y_start+2,TITLE_WHITE);
	draw_pixel(x_start,y_start+3,TITLE_WHITE);
	draw_pixel(x_start,y_start+4,TITLE_WHITE);
	draw_pixel(x_start+5,y_start+1,TITLE_WHITE);
	draw_pixel(x_start+5,y_start+2,TITLE_WHITE);
	draw_pixel(x_start+5,y_start+3,TITLE_WHITE);
	draw_pixel(x_start+5,y_start+4,TITLE_WHITE);
	draw_pixel(x_start+2,y_start+2,TITLE_WHITE);
	draw_pixel(x_start+2,y_start+3,TITLE_WHITE);
	draw_pixel(x_start+3,y_start+2,TITLE_WHITE);
	draw_pixel(x_start+3,y_start+3,TITLE_WHITE);
}

//just fills screen with a color
void clear_screen(short color)
{
	for (int x = 0; x < 320; x++) {
		for (int y = 0; y < 240; y++) {
			draw_pixel(x, y, color);
		}
	}
}

//Creates a 10x10 block at the given coordinates
static void block_draw(short x, short y, short color)
{
	short x2 = x + 9, y2 = y + 9;
	short color_bias=0;
	for (int j = y; j < y2; j++) {
		for (int i = x; i < x2; i++) {
			draw_pixel(i, j, color+color_bias);
		}
		if (color) {
			color_bias+=COLOR_GRADIENT;
		}
	}
}

static void outline_component(int x_start, int y_start, int x_end, int y_end, short stage, short color) {

	line(x_start,y_start-stage,x_end,y_start-stage,color);   //top line
	line(x_end+stage,y_start,x_end+stage,y_end,color);   //right line
	line(x_end,y_end+stage,x_start,y_end+stage,color);   //bottom line
	line(x_start-stage,y_end,x_start-stage,y_start,color);   //left line
	line(x_end,y_start-stage,x_end+stage,y_start,color);   //upper right diagonal line
	line(x_end+stage,y_end,x_end,y_end+stage,color);   //lower right diagonal line
	line(x_start,y_end+stage,x_start-stage,y_end,color);   //lower left diagonal line
	line(x_start-stage,y_start,x_start,y_start-stage,color);   //upper left diagonal line
}

void outline(int x_start, int y_start, int x_end, int y_end)
{

	//inner line
	outline_component(x_start,y_start,x_end,y_end,1,OUTLINE_COLOR_1);
	outline_component(x_start,y_start,x_end,y_end,2,OUTLINE_COLOR_2);
	outline_component(x_start,y_start,x_end,y_end,3,OUTLINE_COLOR_1);

	draw_pixel(x_start,y_start,OUTLINE_COLOR_1);
	draw_pixel(x_end,y_start,OUTLINE_COLOR_1);
	draw_pixel(x_end,y_end,OUTLINE_COLOR_1);
	draw_pixel(x_start,y_end,OUTLINE_COLOR_1);
	draw_pixel(x_start-2,y_start-2,OUTLINE_COLOR_1);
	draw_pixel(x_end+2,y_start-2,OUTLINE_COLOR_1);
	draw_pixel(x_end+2,y_end+2,OUTLINE_COLOR_1);
	draw_pixel(x_start-2,y_end+2,OUTLINE_COLOR_1);
}

//Create a 10x20 size grid on the screen made up by 10x10 blocks.
void grid_draw()
{
	for (int i = 1; i < 11; i++) {
		for (int j = 4; j < 24; j++) {
			block_draw(70 + (i * 10), (j * 10) - 25, Game.grid[j][i]);
		}
	}
}

void next_grid(Tetromino *piece)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			block_draw((200 + i * 10), (30 + j * 10), piece->shape[j][i]);
		}
	}
}

//Draws a specific charcter at a defined x and y location
static void draw_char(short x, short y, char input)
{
	char*(character_buffer) = (char*)(0x09000000) + (y << 7) + x;
	*(character_buffer) = input;
}

//Draws a string that starts at a defined x and y location within the size 80x60
void  draw_string(short x, short y, char* input)
{
	int counter = 0;
	while (*(input + counter) != '\0') {
		draw_char(x + counter, y, *(input + counter));
		counter++;
	}
}

//Clears the char buffer
void clear_char() {
	int x, y;
	for(x = 0; x <= 79; x++){
		for (y = 0; y <= 59; y++) {
				draw_char(x, y, 0);
		}
	}
}

//Draws a box of a desired shape at any location
void box(short x_start, short y_start, short x_end, short y_end)
{
	int i, j;
	for (i = x_start; i < x_end; i++) {
		for (j = y_start; j < y_end; j++) {
			draw_pixel(i, j, 0x0);
		}
	}
}
