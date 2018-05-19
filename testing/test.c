#include "../source/teletext.h"
#include <assert.h>

#define MODE7 "../test.m7"

void test_openFile(void);
void test_state_init(void);
void test_setMode(void);
void test_setBgcolour(void);
void test_setHeightMode(void);
void test_setHodeMode(void);
void test_setMatrixState(void);
void test_readFont(void);
void test_readCodesFromFile(void);

int main(void)
{
   printf("Beginning teletext Test ...\n");
   test_openFile();
   test_state_init();
   test_setMode();
   test_setBgcolour();
   test_setHeightMode();
   test_setHodeMode();
   test_setMatrixState();
   test_readFont();
   test_readCodesFromFile();
   printf("Finished teletext Test ...\n");

   return 0;
}

void test_openFile(void)
{
   FILE *file = openFile(MODE7, "rb");
   assert(file != NULL);
   printf("test open File: OK!\n");
}

void test_state_init(void)
{
   state s;
   state_init(&s);
   assert(s.fgcolour == white);
   assert(s.bgcolour == black);
   assert(s.dismode == alphanumeric);
   assert(s.height == single);
   assert(s.holdgraphics == release);
   printf("test state init: OK!\n");
}

void test_setMode(void)
{
   state s;
   setMode(redalpha, &s);
   assert(s.dismode == alphanumeric && s.fgcolour == red);
   setMode(greenalpha, &s);
   assert(s.dismode == alphanumeric && s.fgcolour == green);
   setMode(yellowalpha, &s);
   assert(s.dismode == alphanumeric && s.fgcolour == yellow);
   setMode(bluealpha, &s);
   assert(s.dismode == alphanumeric && s.fgcolour == blue);
   setMode(magentaalpha, &s);
   assert(s.dismode == alphanumeric && s.fgcolour == magenta);
   setMode(cyanalpha, &s);
   assert(s.dismode == alphanumeric && s.fgcolour == cyan);
   setMode(whitealpha, &s);
   assert(s.dismode == alphanumeric && s.fgcolour == white);
   setMode(redgraphics, &s);
   assert(s.dismode == contiguous && s.fgcolour == red);
   setMode(greengraphics, &s);
   assert(s.dismode == contiguous && s.fgcolour == green);
   setMode(yellowgraphics, &s);
   assert(s.dismode == contiguous && s.fgcolour == yellow);
   setMode(bluegraphics, &s);
   assert(s.dismode == contiguous && s.fgcolour == blue);
   setMode(magentagraphics, &s);
   assert(s.dismode == contiguous && s.fgcolour == magenta);
   setMode(cyangraphics, &s);
   assert(s.dismode == contiguous && s.fgcolour == cyan);
   setMode(whitegraphics, &s);
   assert(s.dismode == contiguous && s.fgcolour == white);
   setMode(contgraphics, &s);
   assert(s.dismode == contiguous);
   setMode(sepgraphics, &s);
   assert(s.dismode == separate);
   setMode(singleheight, &s);
   assert(s.height == single);
   setMode(doubleheight, &s);
   assert(s.height == doubletop);
   setMode(blckbackground, &s);
   assert(s.bgcolour == black);
   setMode(newbackground, &s);
   assert(s.bgcolour == s.fgcolour);
   setMode(holdgraphics, &s);
   assert(s.holdgraphics == hold);
   setMode(relgraphics, &s);
   assert(s.holdgraphics == release);
   printf("test set mode: OK!\n");
}

void test_setBgcolour(void)
{
   state s;
   setMode(redalpha, &s);
   setMode(blckbackground, &s);
   assert(s.bgcolour == black);
   setMode(newbackground, &s);
   assert(s.bgcolour == red);
   setMode(greenalpha, &s);
   setMode(newbackground, &s);
   assert(s.bgcolour == green);
   setMode(yellowalpha, &s);
   setMode(newbackground, &s);
   assert(s.bgcolour == yellow);
   setMode(bluealpha, &s);
   setMode(newbackground, &s);
   assert(s.bgcolour == blue);
   setMode(magentaalpha, &s);
   setMode(newbackground, &s);
   assert(s.bgcolour == magenta);
   setMode(cyanalpha, &s);
   setMode(newbackground, &s);
   assert(s.bgcolour == cyan);
   setMode(whitealpha, &s);
   setMode(newbackground, &s);
   assert(s.bgcolour == white);
   printf("test set background colour: OK!\n");
}

void test_setHeightMode(void)
{
   state s;
   setMode(singleheight, &s);
   assert(s.height == single);
   setMode(doubleheight, &s);
   assert(s.height == doubletop);
   printf("test set height mode: OK!\n");
}

void test_setHodeMode(void)
{
   state s;
   setMode(holdgraphics, &s);
   assert(s.holdgraphics == hold);
   setMode(relgraphics, &s);
   assert(s.holdgraphics == release);
   printf("test set hode mode: OK!\n");
}

void test_setMatrixState(void)
{
   state currstate;
   state testmatrix[H][W];

   readCodesFromFile(MODE7, testmatrix);
   setMatrixState(testmatrix, &currstate);
   
   /*test the set of front colour*/
   assert(testmatrix[5][5].fgcolour == red);
   assert(testmatrix[6][5].fgcolour == green);
   assert(testmatrix[7][5].fgcolour == yellow);
   assert(testmatrix[8][5].fgcolour == magenta);
   assert(testmatrix[9][5].fgcolour == white );
   /*test the set of back colour*/
   assert(testmatrix[13][5].bgcolour == cyan);
   assert(testmatrix[15][13].bgcolour == blue);
   assert(testmatrix[15][19].bgcolour == yellow);
   /*test the set of dismode*/
   assert(testmatrix[0][5].dismode == alphanumeric);
   assert(testmatrix[1][5].dismode == alphanumeric);
   assert(testmatrix[2][5].dismode == alphanumeric);
   assert(testmatrix[3][5].dismode == alphanumeric);
   assert(testmatrix[4][5].dismode == alphanumeric);
   assert(testmatrix[5][5].dismode == alphanumeric);
   assert(testmatrix[H-1][5].dismode == contiguous);
   assert(testmatrix[H-2][5].dismode == contiguous);
   assert(testmatrix[H-5][15].dismode == separate);
   assert(testmatrix[H-5][5].dismode == contiguous);
   /*test the set of height*/
   assert(testmatrix[1][15].height == single);
   assert(testmatrix[5][15].height == single);
   assert(testmatrix[6][15].height == single);
   assert(testmatrix[7][15].height == single);
   assert(testmatrix[2][15].height == doubletop);
   assert(testmatrix[3][15].height == doublebottom);
   assert(testmatrix[15][31].height == single);
   assert(testmatrix[16][31].height == doubletop);
   assert(testmatrix[15][36].height == doubletop);
   assert(testmatrix[16][36].height == doublebottom);
   /*test the set of hold*/
   assert(testmatrix[2][15].holdgraphics == release);
   assert(testmatrix[3][15].holdgraphics == release);
   assert(testmatrix[15][31].holdgraphics == release);
   assert(testmatrix[16][31].holdgraphics == release);
   assert(testmatrix[15][36].holdgraphics == release);
   assert(testmatrix[20][36].holdgraphics == hold);
   assert(testmatrix[20][35].holdgraphics == hold);
   assert(testmatrix[20][34].holdgraphics == hold);
   assert(testmatrix[20][33].holdgraphics == hold);
   /*m[18][31] is hold control after that control codes are
   instead displayed as copy of most recently graphices
   until meet the release control code*/
   assert(testmatrix[18][31].holdgraphics == hold);
   assert(testmatrix[18][32].holdgraphics == hold);
   assert(testmatrix[18][33].holdgraphics == hold);
   assert(testmatrix[18][34].holdgraphics == hold);
   assert(testmatrix[18][35].holdgraphics == hold);
   assert(testmatrix[18][36].holdgraphics == hold);
   assert(testmatrix[18][37].holdgraphics == release);
   assert(testmatrix[18][38].holdgraphics == release);
   assert(testmatrix[18][39].holdgraphics == release);
   printf("test set Matrix State: OK!\n");
}

void test_readFont(void)
{
   myUint16 font[CHARNUM][FNTH];
   myUint16 testdata[CHARNUM][FNTH] = {
   #include "m7fixed_data.h"
   };
   int i, j;
   loadFonts(font, FONT);
   for (i = 0; i < CHARNUM; i++) {
      for (j = 0; j < FNTH; j++) {
         assert(memcmp(&font[i][j], &testdata[i][j],
                sizeof(myUint16)) == 0);
      }
   }
   printf("test read font file: OK!\n");
}

void test_readCodesFromFile(void)
{
   state matrix[H][W];
   myUint8  test_matrix[H][W]={
   #include "test_m7data.h"
   };
   int i,j;

   readCodesFromFile(MODE7, matrix);
   for(i = 0; i < H; i++){
      for(j = 0; j < W; j++){
         assert(matrix[i][j].code == test_matrix[i][j]);
      }
   }
   printf("test read mode7 file: OK!\n");
}
