#include "teletext.h"

int main(int argc, char **argv)
{
   state currstate;
   state matrix[H][W];
   display d;
   myUint16 font[CHARNUM][FNTH];
 
   if(argc != 2){
      ON_ERROR("Only 1 argument is required.\n");
   }
   readCodesFromFile(argv[1], matrix);
   loadFonts(font, FONT);
   setMatrixState(matrix, &currstate);
   setMatrixPixels(matrix, font);
  
   initSDL(&d);
   SDL_DrawString(&d, matrix);
   SDL_RenderPresent(d.rend);
   SDL_RenderClear(d.rend);

   do {
     SDL_Events(&d);
   }while (d.ended == 0);
   /* Clear up graphics subsystems */
   SDL_Quit();
   return 0;
}
