#include "boardFunctions.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#define maxPoints 500

int board[13][13];
extern int ernno;
int port;
struct sockaddr_in server; //connect to server
int sd; //socket descriptor
char namePlayer[32];
int sizeBoard;

char *whichColor(int color){
    if(color == 1)
        return "black";
    return "white";
}

int *tableSize(char c){
    if(c == 'I')
        return 10;
    return 12;
}

char *reversewhichColor(int color){
    if(color == 1)
        return "white";
    return "black";
}

char *translateColumn(int c){
    return c + 'a' - 1;
}


void selectGamePrint(int c){
    //International(I), Canadian(C)
    printf("\n");
    if(c == 1)
        printf("\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mYou play International checkers");
    else if(c == 0)
        printf("\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mYou play Canadian checkers");
    printf("\n");
}

void finalMessage(int champ){
    if(champ == 1)
        printf("\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mNice! You Win!\n");
    else
        printf("\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mNext Time!\n");
}

int main(int argc, char *argv[]){
    //<---------> CONNECTION SECTION <--------->
    if(argc != 3){ 
        printf("\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37m%s <address> <port> \n",argv[0]);
        return -1;
    }
    port = atoi(argv[2]);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      printf("\033[0;32m[CLIENT] \033[0;31mWARN   \033[0;37mSocket Error! \n");
      return errno;
    }

    server.sin_family = AF_INET;  
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port); 

    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        printf("\033[0;32m[CLIENT] \033[0;31mWARN   \033[0;37mSocket Connect Error! \n");
        return errno;
    }

    printf("\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mWELCOME! \n");

    //<---------> SELECT NAME SECTION <--------->
    bzero(namePlayer,32);
    printf("\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mSet your nickname: ");
    fflush(stdout);
    read(0,namePlayer,32);
    namePlayer[strlen(namePlayer) - 1]= '\0';

    write(sd,&namePlayer,32);
    read(sd,namePlayer,32);
    printf("\n\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mYour enemy nickname: \033[0;36m%s \n",namePlayer);
    fflush(stdout);

    //<---------> CHOOSE PIECES <--------->
    int colorBuffer; // color pieces
    read(sd,&colorBuffer,sizeof(int));
    printf("\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mYou will play with \033[0;36m%s \033[0;37mpieces \n", whichColor(colorBuffer));
    printf("\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mYour enemy will play with \033[0;36m%s \033[0;37mpieces \n", reversewhichColor(colorBuffer));
    

    //<---------> SELECT GAME SECTION <--------->
    setlocale( LC_ALL, "en_US.UTF-8" );
    int selectGame;
    read(sd,&selectGame,sizeof(int));
    selectGamePrint(selectGame);

    //size board
    read(sd,&sizeBoard,sizeof(int));
    printf("\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mThis game have %d x %d pieces \n",sizeBoard,sizeBoard); 

    //<---------> GAMEPLAY SECTION <---------> 
    int gameLoop = 1; 
    int round;
    int win;
    
    while(gameLoop){
        //player current
        //updated board
        for(int i = 0; i < sizeBoard; ++i)
            for(int j = 0; j < sizeBoard; ++j)
                read(sd,&board[i][j],sizeof(int));
        printBoard(sizeBoard,board);

        read(sd,&round,sizeof(int));
        int responseFromServer = 0;
        if(round == 1){
                printf("\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mYour round! You have 4 chances!\n");
                //set positions / moves
                do{
                printf("\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mThis is your table!\n");
                    int l,lFinal;
                    int c,cFinal;
                    //set your positions
                    // choose piece position
                    printf("\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mFirst Position!!!\n");
                    printf("\n\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mFor column choose between 1 and %d (a = 1, b = 2 etc...): ",sizeBoard);
                    scanf("%d",&c);
                    printf("\n\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mFor line choose between 1 and %d: ",sizeBoard);
                    scanf("%d",&l);
                    printf("\n");  

                    // choose destionation position 
                    printf("\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mSecond Position!!!\n");
                    printf("\n\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mFor column choose between 1 and %d (a = 1, b = 2 etc...): ",sizeBoard);
                    scanf("%d",&cFinal);
                    printf("\n\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mFor line choose between 1 and %d: ",sizeBoard);
                    scanf("%d",&lFinal);
                    printf("\n");
                    
                    printf("\n\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mYou have selected: %c%d -> %c%d\n",translateColumn(c),l,translateColumn(cFinal),lFinal);    
                    write(sd, &c, sizeof(int));
                    write(sd, &l, sizeof(int));
                    write(sd, &cFinal, sizeof(int));
                    write(sd, &lFinal, sizeof(int));

                    //good positions?
                    read(sd,&responseFromServer,sizeof(int));
                    if(responseFromServer == 1)
                        printf("\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mGood Move!!!\n");
                    else 
                        printf("\033[0;32m[CLIENT] \033[0;31mWARN   \033[0;37mTry Again!!!Incorect positions\n");
                }while(responseFromServer == 0);

        
        }else printf("\033[0;32m[CLIENT] \033[0;34mINFO   \033[0;37mWait for your round!!!\n");
        
        //update
        for(int i = 0; i < sizeBoard; ++i)
            for(int j = 0; j < sizeBoard; ++j)
                read(sd,&board[i][j],sizeof(int));
        printBoard(sizeBoard,board);
                
        read(sd,&win,sizeof(int));
        if(win == 1)
            gameLoop = 0;
    }
    int winner;
    read(sd,&winner,sizeof(int));
    finalMessage(winner);
    close(sd);       
}
