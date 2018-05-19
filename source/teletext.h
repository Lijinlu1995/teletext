#include <stdio.h>
 #include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <time.h>

#define ON_ERROR(STR) fprintf(stderr, STR); exit(EXIT_FAILURE)
#define MAX(a,b) (a>b? a:b)
#define H 25
#define W 40
#define FNTW 16
#define FNTH 18
#define TEXTCNT 1000 /*sum of teletext numbers*/

/* value of min bits if less should be add 128 */
#define MINBITS 128 
#define WINW (W * FNTW)
#define WINH (H * FNTH)
/*individual sixels define*/
#define SIXELH    (FNTH / 3)
#define SIXELCNT  6
#define SIXELCOLS 2
#define SIXELROWS 3
/* Blast through text begin and end points. */
#define BLASTBEGIN 0xC0 
#define BLASTEND   0xDF
#define CONT 0xFF /* 8 bits for 11111111 */
#define SEP  0x7E /* 8 bits for 01111110 */
#define BASECODE 0xA0 
/* number of colours including black */
#define CLRNUM 8 
#define CHARNUM 96 /* number of characters in font file */
/* singleheight, double including top and bottom.*/
#define LOCATIONNUM 3 
#define WINTITLE "Teletext Viewer"
#define FONT "../m7fixed.fnt"
 
typedef unsigned char  myUint8;
typedef unsigned short myUint16;

enum colour {
   black, red, green, yellow, blue, magenta, cyan, white
};
typedef enum colour colour;

enum displayMode { alphanumeric, contiguous, separate };
typedef enum displayMode  displayMode;

enum holdMode { release, hold };
typedef enum holdMode holdMode;

enum heightMode { single, doublebottom, doubletop };
typedef enum heightMode heightMode;

struct state {
  myUint8     code;
  myUint16    ch[FNTH];
  colour      fgcolour;
  colour      bgcolour;
  displayMode dismode;
  heightMode  height;
  holdMode    holdgraphics;
} ;
typedef struct state state;

/*define the display location for double top or bottom */
struct location {
  myUint8 x, y, w, h;
};
typedef struct location location;

struct display {
   SDL_bool ended;
   SDL_Window *win;
   SDL_Renderer *rend;
};
typedef struct display display;

struct RGBtriple {
  myUint8 r, g, b;
};
typedef struct RGBtriple RGBtriple;

enum controlCode {
  redalpha        = 0x81,
  greenalpha      = 0x82,
  yellowalpha     = 0x83,
  bluealpha       = 0x84,
  magentaalpha    = 0x85,
  cyanalpha       = 0x86,
  whitealpha      = 0x87,
  singleheight    = 0x8c,
  doubleheight    = 0x8d,

  redgraphics     = 0x91,
  greengraphics   = 0x92,
  yellowgraphics  = 0x93,
  bluegraphics    = 0x94,
  magentagraphics = 0x95,
  cyangraphics    = 0x96,
  whitegraphics   = 0x97,
  contgraphics    = 0x99,
  sepgraphics     = 0x9a,
  blckbackground  = 0x9c,
  newbackground   = 0x9d,
  holdgraphics    = 0x9e,
  relgraphics     = 0x9f
};
typedef enum controlCode controlCode;

/*individual sixels classify*/
enum sixel { off, on };
typedef enum sixel sixel;

enum sixelcol { left, right };
typedef enum sixelcol sixelcol;

enum sixelrow { top, mid, bottom };
typedef enum sixelrow sixelrow;

/*sixl position to define witch should be on*/
enum sixelpos { 
  topleft     = 0, 
  topright    = 1, 
  midleft     = 2, 
  midright    = 3,
  bottomleft  = 4, 
  bottomright = 6
};

/* initial state*/
void state_init(state *s);
/* read code from binary file*/
FILE *openFile(char *filename, char *mode);
myUint8 addBits(myUint8 i);
void readCodesFromFile(char *filename, state matrix[H][W]);
/*edit contiguous and separate font array base on the m7fixed.fnt*/
void editChar(myUint16 *point, unsigned int i, myUint8 *array);
void writeSixels(myUint16* point, sixel sixels[SIXELROWS][SIXELCOLS], 
  myUint8 *array);
/* Updates the current state then copy to the  state. */
void setMatrixState(state matrix[H][W], state *s);
void setState(state *s, state *currstate);
void setMode(myUint8 code, state *s);

void loadFonts(myUint16 font[CHARNUM][FNTH], char *fname);
void setPixels(myUint16 font[CHARNUM][FNTH], state *s);
void setHoldGraphics(myUint16 *fonttile, state *s, myUint16 *myfont);
void setMatrixPixels(state matrix[H][W], myUint16 font[CHARNUM][FNTH]);
/*SDL part to display*/
void initSDL(display *d);
void SDL_DrawString(display *d, state matrix[H][W]);
void SDL_DrawChar(display *d, state *s, int x, int y);
void SDL_SetDrawColour(display *d, colour c);
void SDL_Events(display *d);
