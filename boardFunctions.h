#include <wchar.h>
#define draughtsB 0x26C0
#define draughtsW 0x26C2
#define draughtsKB 0x26C1
#define draughtsKW 0x26C3

wchar_t resultPieces(int code){
    if(code == -1)
        return ' '; // space, no piece
    if(code == 0)
        return draughtsW; //white piece
    if(code == 1)
        return draughtsB; //black piece
    if(code == 2)
        return draughtsKW; //white king piece
    if(code == 3)
        return draughtsKB; //black king piece 
}


void printBoard(int size,int board[13][13]){
   if(size == 10){ //print format for international 
         printf("          a   b   c   d   e   f   g   h   i   j    \n");
         for(int i = 0; i < 10; ++i){
            printf("        -----------------------------------------   \n");
            printf("     %d  │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc | %lc |  %d \n", (i + 1)%10, resultPieces(board[i][0]), resultPieces(board[i][1]), resultPieces(board[i][2]), resultPieces(board[i][3]), resultPieces(board[i][4]), resultPieces(board[i][5]), resultPieces(board[i][6]), resultPieces(board[i][7]), resultPieces(board[i][8]), resultPieces(board[i][9]), (i + 1)%10);
         }
         printf("        -----------------------------------------   \n");
         printf("          a   b   c   d   e   f   g   h   i   j    \n\n");
   }
   else if(size == 12){ // print format for canadian
         printf("          a   b   c   d   e   f   g   h   i   j   k   l    \n");
         for(int i = 0; i < 12; ++i){
            printf("        -------------------------------------------------   \n");
            printf("     %d  │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc │ %lc | %lc | %lc | %lc |  %d \n", (i + 1)%10, resultPieces(board[i][0]), resultPieces(board[i][1]), resultPieces(board[i][2]), resultPieces(board[i][3]), resultPieces(board[i][4]), resultPieces(board[i][5]), resultPieces(board[i][6]), resultPieces(board[i][7]), resultPieces(board[i][8]), resultPieces(board[i][9]), resultPieces(board[i][10]), resultPieces(board[i][11]), (i + 1)%10);
         }
         printf("        -------------------------------------------------   \n");
         printf("          a   b   c   d   e   f   g   h   i   j   k   l    \n\n");
   }
   printf("\n\n");
}
