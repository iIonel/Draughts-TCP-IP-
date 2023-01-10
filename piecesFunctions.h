void defaultInternationalBoard(int board[13][13]){
    for(int i = 0; i < 10; ++i) //10x10
        for(int j = 0; j < 10; ++j)
            board[i][j] = -1;
    for(int i = 0; i < 10; ++i){
        if(i % 2 == 1){
            board[0][i] = 0;
            board[2][i] = 0;
            board[6][i] = 1;
            board[8][i] = 1;
        }
        else{
            board[1][i] = 0;
            board[3][i] = 0;
            board[7][i] = 1;
            board[9][i] = 1;
        }
    }
}

void defaultCanadianBoard(int board[13][13]){
    for(int i = 0; i < 12; ++i) //12x12
        for(int j = 0; j < 12; ++j)
            board[i][j] = -1;
    for(int i = 0; i < 12; ++i){
        if(i % 2 == 1){
            board[0][i] = 0;
            board[2][i] = 0;
            board[4][i] = 0;
            board[10][i] = 1;
            board[8][i] = 1;
        }
        else{
            board[1][i] = 0;
            board[3][i] = 0;
            board[7][i] = 1;
            board[9][i] = 1;
            board[11][i] = 1;
        }
    }
}

void defaultBoard(int c,int board[13][13]){
    for(int i = 0; i < 12; ++i)
        for(int j = 0; j < 12; ++j)
            board[i][j] = -1; //no pieces default
    if(c == 1)
        defaultInternationalBoard(board);
    else
        defaultCanadianBoard(board);
}

int charToIntColumn(char c){
    return c - 97;
}

int inBoard(int a, int b, int c, int d, int e){
    if(a < 0 || b < 0 || c < 0 || d < 0 || a > e - 1 || b > e - 1 || c > e - 1 || d > e - 1)
        return 0;
    return 1;
}

int goodPositions(int board[13][13],int size, int c, int l, int cFinal, int lFinal, int colorRound, int s){

    if(inBoard(l,lFinal,c,cFinal,s) == 0){
        printf("\033[0;32m[SERVER] \033[0;31mWARN   \033[0;37mIn board??!?!\n");
        return 0;
    }

    if(board[lFinal][cFinal] != -1){
        printf("\033[0;32m[SERVER] \033[0;31mWARN   \033[0;37mNo space!!!\n");
        return 0; //invalid position for move
    }

    if(colorRound == 0){ //white
        if(lFinal == l + 1 && (cFinal == c + 1 || cFinal == c - 1)) return 1;
        if((lFinal == l - 2 && cFinal == c - 2 && board[l - 1][c - 1] == 1) || (lFinal == l - 2 && cFinal == c + 2 && board[l - 1][c + 1] == 1)) return 1;
        if((lFinal == l + 2 && cFinal == c - 2 && board[l + 1][c - 1] == 1) || (lFinal == l + 2 && cFinal == c + 2 && board[l + 1][c + 1] == 1)) return 1;
    } 
    else if(colorRound == 1){ //black
        if(lFinal == l - 1 && (cFinal == c + 1 || cFinal == c - 1)) return 1;
        if((lFinal == l - 2 && cFinal == c - 2 && board[l - 1][c - 1] == 0) || (lFinal == l - 2 && cFinal == c + 2 && board[l - 1][c + 1] == 0)) return 1;
        if((lFinal == l + 2 && cFinal == c - 2 && board[l + 1][c - 1] == 0) || (lFinal == l + 2 && cFinal == c + 2 && board[l + 1][c + 1] == 0)) return 1;
    }

    if(board[l][c] == colorRound + 2)
        if(abs(lFinal - l) == abs(cFinal - c)) return 1;

    return 0;
}  

int updatePieces(int c,int l,int cFinal,int lFinal,int colorRound, int size,int board[13][13]){
    //put pieces 
    if(colorRound == 0 && lFinal == size - 1)  board[lFinal][cFinal] = colorRound + 2;
    else if(colorRound == 1 && lFinal == 0) board[lFinal][cFinal] = colorRound + 2;
    else board[lFinal][cFinal] = board[l][c];

    int dPoints = 0;
    //delete pieces
    int op;
    if(colorRound == 1) op = 0;
    else op = 1;
  
        if((lFinal == l + 2 && cFinal == c - 2 && board[l + 1][c - 1] == op)){
            board[l + 1][c - 1] = -1;
            dPoints = 1;}
        
        else if((lFinal == l + 2 && cFinal == c + 2 && board[l + 1][c + 1] == op)){
             board[l + 1][c + 1] = -1;
             dPoints = 1;}
        

        if((lFinal == l - 2 && cFinal == c - 2 && board[l - 1][c - 1] == op)){
             board[l - 1][c - 1] = -1; 
             dPoints = 1;}
        else if((lFinal == l - 2 && cFinal == c + 2 && board[l - 1][c + 1] == op)){
             board[l - 1][c + 1] = -1;
             dPoints = 1;}
 

    if(board[l][c] == colorRound + 2){
        if(lFinal > l && cFinal < c){
            for(int i = 1; i <= lFinal - l; ++i)
            {
                if(board[l + i][c - i] == op || board[l + i][c - i] == op + 2){
                    board[l + i][c - i] = -1;
                    dPoints++;}
            }
        }else if(lFinal > l && cFinal > c){
            for(int i = 1; i <= lFinal - l; ++i)
            {
                if(board[l + i][c + i] == op || board[l + i][c + i] == op + 2){
                    board[l + i][c + i] = -1;
                    dPoints++;}
            }
        }else if(lFinal < l && cFinal < c){
            for(int i = 1; i <= l - lFinal; ++i)
            {
                if(board[l - i][c - i] == op || board[l - i][c - i] == op + 2){
                    board[l - i][c - i] = -1;
                    dPoints++;}
            }
        }else if(lFinal < l && cFinal > c){
            for(int i = 1; i <= l - lFinal; ++i)
            {
                if(board[l - i][c + i] == op || board[l - i][c + i] == op + 2){
                    board[l - i][c + i] = -1;
                    dPoints++;}
            }
        }
    }
    board[l][c] = -1;

    return dPoints;
}
