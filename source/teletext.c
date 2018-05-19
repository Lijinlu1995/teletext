#include "teletext.h"

void state_init(state *s)
{
   s->fgcolour = white;
   s->bgcolour = black;
   s->dismode  = alphanumeric;
   s->holdgraphics = release;
   s->height   = single;
}

FILE *openFile(char *filename, char *mode)
{
   FILE *file = fopen(filename, mode);
   if (file == NULL) {
      ON_ERROR("ERROR: unable to open file:\n");
   }
   return file;
}

void readCodesFromFile(char *filename, state matrix[H][W])
{
   myUint8 a[H][W];
   FILE *file = openFile(filename, "rb");
   int i, j;
   size_t itemcnt = fread(a, sizeof(myUint8), TEXTCNT, file);
   if (itemcnt != TEXTCNT) {
      ON_ERROR("Incorrect filesize.\n");
   }
   fclose(file);
   for (i = 0; i < H; i++) {
      for (j = 0; j < W; j++) {
         matrix[i][j].code = addBits(a[i][j]);
      }
   }
}

myUint8 addBits(myUint8 i)
{
   if (i < MINBITS) {
      i += MINBITS;
   }
   return i;
}

void setMatrixState(state matrix[H][W], state *currstate)
{
   int i, j;
   for (i = 0; i < H; i++) {
      state_init(currstate);
      for (j = 0; j < W; j++) {
         setMode(matrix[i][j].code, currstate);
         if (i > 0 && currstate->height == doubletop
         &&matrix[i-1][j].height == doubletop) {
         currstate->height = doublebottom;
      }
      setState(&matrix[i][j], currstate);
    }
  }
}

void setState(state *s, state *currState)
{
   s->fgcolour = currState->fgcolour;
   s->bgcolour = currState->bgcolour;
   s->dismode  = currState->dismode;
   s->height   = currState->height;
   s->holdgraphics = currState->holdgraphics;
}

void setMode(myUint8 code, state *s)
{
   switch (code) {
    /* alphanumeric mode code.*/
    case redalpha:
      s->dismode  = alphanumeric; s->fgcolour = red;     break;
    case greenalpha:
      s->dismode  = alphanumeric; s->fgcolour = green;   break;
    case yellowalpha:
      s->dismode  = alphanumeric; s->fgcolour = yellow;  break;
    case bluealpha:
      s->dismode  = alphanumeric; s->fgcolour = blue;    break;
    case magentaalpha:
      s->dismode  = alphanumeric; s->fgcolour = magenta; break;
    case cyanalpha:
      s->dismode  = alphanumeric; s->fgcolour = cyan;    break;
    case whitealpha:
      s->dismode  = alphanumeric; s->fgcolour = white;   break;
    /* graphical mode codes.*/
    case redgraphics:
      s->dismode  = contiguous; s->fgcolour = red;     break;
    case greengraphics:
      s->dismode  = contiguous; s->fgcolour = green;   break;
    case yellowgraphics:
      s->dismode  = contiguous; s->fgcolour = yellow;  break;
    case bluegraphics:
      s->dismode  = contiguous; s->fgcolour = blue;    break;
    case magentagraphics:
      s->dismode  = contiguous; s->fgcolour = magenta; break;
    case cyangraphics:
      s->dismode  = contiguous; s->fgcolour = cyan;    break;
    case whitegraphics:
      s->dismode  = contiguous; s->fgcolour = white;   break;
    /* contiguous 0r separate mode. */
    case contgraphics:
      s->dismode  = contiguous; break;
    case sepgraphics:
      s->dismode  = separate;   break;
    /* Single and double height.*/
    case singleheight:
      s->height = single;       break;
    case doubleheight:
      s->height = doubletop;    break;
    /* Background colour */
    case blckbackground:
      s->bgcolour = black;      break;
    case newbackground:
      s->bgcolour = s->fgcolour;break;
    /* Hold or release graphics. */
    case holdgraphics:
      s->holdgraphics = hold;   break;
    case relgraphics:
      s->holdgraphics = release;break;
    default: break;
   }
}

void loadFonts(myUint16 font[CHARNUM][FNTH], char *fname)
{
   FILE *file = openFile(FONT, "rb");
   size_t itms;
   if (!file) {
      fprintf(stderr, "Can't open Font file %s\n", fname);
       exit(1);
   }
   itms = fread(font, sizeof(myUint16), CHARNUM * FNTH, file);
   if (itms != CHARNUM * FNTH)  {
      fprintf(stderr, "Can't read all Font file %s (%d) \n", 
              fname, (int)itms);
       exit(1);
   }
   fclose(file);
}

void setMatrixPixels(state matrix[H][W], 
                     myUint16 font[CHARNUM][FNTH])
{
   int i, j;
   unsigned int k;
   myUint16 fontcont[CHARNUM][FNTH];
   myUint16 fontsep[CHARNUM][FNTH];
   myUint8 cont[SIXELH] = { CONT, CONT, CONT, CONT, CONT, CONT };
   myUint8 sep[SIXELH] = { 0, SEP, SEP, SEP, SEP, 0 };
   memcpy(fontcont, font, sizeof(myUint16) * FNTH * CHARNUM);
   memcpy(fontsep, font, sizeof(myUint16) * FNTH * CHARNUM );
   /*creat two new font array to edit base on the m7fixed.fnt*/
   for (k = 0; k < CHARNUM; k++) {
      editChar(fontcont[k], k + BASECODE, cont);
      editChar(fontsep[k], k + BASECODE, sep);
   }
   for (i = 0; i < H; i++) {
      for (j = 0; j < W; j++) {
         if (matrix[i][j].dismode == alphanumeric) {
            setPixels(font,  &matrix[i][j]);
         }else if (matrix[i][j].dismode == contiguous) {
            setPixels(fontcont,  &matrix[i][j]);
         }else{
            setPixels(fontsep,  &matrix[i][j]);
         }
      }
    }
}

void setPixels(myUint16 font[CHARNUM][FNTH], state *s)
{
   unsigned int i, j, site;
   location locate;
   myUint8 index = MAX(s->code, BASECODE) - BASECODE;
   myUint16 myfont[FNTH] = {0};
   static location fontlocation[LOCATIONNUM] = {
    /*single, doublebottom, doubletop*/
    {0,0,FNTW,FNTH}, {0,FNTH/2,FNTW,FNTH/2},{0,0,FNTW,FNTH/2}
   };
   locate = fontlocation[s->height];
   site = FNTH / locate.h;
   setHoldGraphics(font[index], s, myfont);
   for (j = 0; j < locate.h; j++){
      for (i = 0; i < site; i++) {
         s->ch[(j * site) + i] = myfont[j + locate.y];
      } 
    }
}

void setHoldGraphics(myUint16 *nfont, state *s, myUint16 *myfont)
{
   static myUint16 temp[FNTH] = {0};
   static heightMode  prevheight  = single;
   static displayMode prevdismode = alphanumeric;
   
   /*reset the hold graphics when dismode or height change*/
   if (prevdismode != s->dismode || prevheight != s->height) {
      memset(temp, 0, sizeof(myUint16) * FNTH);
   }
   /*set the myfont be last graphics or newfont*/
   if (s->code < BASECODE && s->holdgraphics == hold) {
      memcpy(myfont, temp, sizeof(myUint16) * FNTH);
   }
   else {
      memcpy(myfont, nfont, sizeof(myUint16) * FNTH);
   }

   /*for code is not control code store in temp*/
   if (s->code > BASECODE && s->dismode != alphanumeric) {
      memcpy(temp, nfont, sizeof(myUint16) * FNTH);
   }
   prevdismode = s->dismode;
   prevheight  = s->height;
}


void editChar(myUint16 *point, unsigned int i,  myUint8 *array)
{
   int j;
   /*init sixels be off*/
   sixel sixels[SIXELROWS][SIXELCOLS] = {{off}};

   if ((i >> topleft) & 1) {
     sixels[top][left] = on;
   }
   if ((i >> topright) & 1) {
     sixels[top][right] = on;
   }
   if ((i >> midleft) & 1) {
     sixels[mid][left] = on;
   }
   if ((i >> midright) & 1) {
     sixels[mid][right] = on;
   }
   if ((i >> bottomleft) & 1) {
     sixels[bottom][left] = on;
   }
   if ((i >> bottomright) & 1) {
     sixels[bottom][right] = on;
   }
   if ( i < BLASTBEGIN || i > BLASTEND) {
      for (j = 0; j < FNTH; j++) {
         point[j] = 0;
      }
      writeSixels(point, sixels, array);
   }

}

void writeSixels(myUint16* point, sixel sixels[SIXELROWS][SIXELCOLS], 
  myUint8 *array)
{
  sixelcol i;
  sixelrow j;
  int k;

  for (j = 0; j < SIXELROWS; j++) {
    for (i = 0; i < SIXELCOLS; i++) {
      if (sixels[j][i] == on) {
        for (k = 0; k < SIXELH; k++) {
          /*characters for 8 bits*/
          point[k +(SIXELH * j)] |= (array[k] << ((1 - i) * 8));
        }
      }
    }
  }
}

void initSDL(display *d)
{
   if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      fprintf(stderr, "\nUnable to initialize SDL: %s\n", 
              SDL_GetError());
      SDL_Quit();
      exit(1);
   }
   d->ended = 0;
   d->win= SDL_CreateWindow("Teletext Viewer",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          WINW, WINH,
                          SDL_WINDOW_SHOWN);
   if(d->win == NULL){
      fprintf(stderr, "\nUnable to initialize SDL Window: %s\n", 
      SDL_GetError());
      SDL_Quit();
      exit(1);
   }
   d->rend = SDL_CreateRenderer(d->win, -1, 
                   SDL_RENDERER_ACCELERATED | 
                   SDL_RENDERER_TARGETTEXTURE);
   if(d->rend == NULL){
      fprintf(stderr, "\nUnable to initialize SDL Renderer: %s\n", 
              SDL_GetError());
      SDL_Quit();
      exit(1);
   }
   SDL_SetRenderDrawBlendMode(d->rend,SDL_BLENDMODE_BLEND);
}

void SDL_DrawString(display *d, state matrix[H][W])
{
   int i, j;
   for (i = 0; i < H; i++) {
      for (j = 0; j < W; j++) {
         SDL_DrawChar(d, &matrix[i][j], j * FNTW, i * FNTH);
      }
   }
}

void SDL_DrawChar(display *d, state *s, int x, int y)
{
   int i, j;
   for (j = 0; j < FNTH; j++){
      for (i = 0; i < FNTW; i++){
         if (s->ch[j] >> (FNTW - 1 - i) & 1) {
            SDL_SetDrawColour(d, s->fgcolour);
         }
         else {
            SDL_SetDrawColour(d, s->bgcolour);
         }
         SDL_RenderDrawPoint(d->rend, x + i, y + j);
      }
   }
}

void SDL_SetDrawColour(display *d, colour c)
{
   /* RGB of colours including black */
   static RGBtriple rgb[CLRNUM] = {{0,0,0}, {255,0,0}, 
     {0,255,0}, {255,255,0},{0,0,255},{255,0,255},
     {0,255,255}, {255,255,255}
   };
   SDL_SetRenderDrawColor(d->rend,rgb[c].r,rgb[c].g,rgb[c].b,
                          SDL_ALPHA_OPAQUE);
}

/* Gobble all events & ignore most */
void SDL_Events(display *d)
{
   SDL_Event event;
   while(SDL_PollEvent(&event)) 
   {      
       switch (event.type){
          case SDL_QUIT:
          case SDL_MOUSEBUTTONDOWN:
          case SDL_KEYDOWN:
             d->ended = 1;
       }
    }
}
