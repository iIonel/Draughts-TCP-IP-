#include "piecesFunctions.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <wait.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctype.h>
#define PORT 2023

//player structure
struct player{
    int color;  // 0 for white and 1 for black pieces
    int score;
    int winner;
    char nickname[32];
    int move;
}player1,player2;

int winner = 0;
int board[13][13];
int l,lFinal;
int c,cFinal;
int size;
int client1,client2;
char selectGame;
time_t t;
struct tm* timeInfo;
extern int errno;
int sd;
int optval;
struct sockaddr_in server; 
struct sockaddr_in from;

char *whichColor(int color){
    if(color == 1)
        return "black";
    return "white";
}

int selectSizeBoard(int c){
    if(c == 1)
        return 10;
    return 12;
}

int randGame(){
    srand(time(NULL));
    int r = rand();
    return r % 2;
}

char *whatGame(int c){
    if(c == 1)
        return "I";
    return "C";
}

char *translateColumn(int c){
    return c + 'a' - 1;
}


int main(){
    FILE *f;
    //<---------> CONNECTION SECTION <--------->
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
        
		printf("\033[0;32m[SERVER] \033[0;31mWARN   \033[0;37mSocket Error! \n");
		return errno;
	}
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    bzero(&server, sizeof(server));
	bzero(&from, sizeof(from));

    server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);

    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
	{
		printf("\033[0;32m[SERVER] \033[0;31mWARN   \033[0;37mBind Error! \n");
		return errno;
	}

	if (listen(sd, 1) == -1)
	{
      
		printf("\033[0;32m[SERVER] \033[0;31mWARN   \033[0;37mListen Error! \n");
		return errno;
	}
    int pId;
    //<---------> GAME DESIGN <--------->
    while(1){
        int client1,client2;
        int len = sizeof(from);
		printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mWaiting at port: %d \n",PORT);
        client1 = accept(sd, (struct sockaddr *)&from, &len);
		client2 = accept(sd, (struct sockaddr *)&from, &len);

        if(client1 < 0 || client2 < 0)
        {
		    printf("\033[0;32m[SERVER] \033[0;31mWARN   \033[0;37mAccept Error! \n");
            continue;
        }

        pId = fork();
        if(pId == -1){
            close(client1);
            close(client2);
            continue;
        }
        if(pId > 0){ //father 
            close(client1);
            close(client2);
            while (waitpid(-1, NULL, WNOHANG))
				;
			continue;
        }
        if(pId == 0){ //child

            close(sd);
            bzero(player1.nickname, 100);
			bzero(player2.nickname, 100);
            
            //take nicknames
            read(client1,player1.nickname,32);
            printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;36m%s \033[0;37mjoin!\n",player1.nickname);
            read(client2,player2.nickname,32);
            
            printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;36m%s \033[0;37mjoin!\n",player2.nickname);
            printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mCreate lobby with \033[0;36m%s and \033[0;36m%s\033[0;37m...\n",player1.nickname,player2.nickname);

            //transfer nicknames
            write(client2,&player1.nickname,32);
            write(client1,&player2.nickname,32);

            //first - white, second - black
            player1.color = 0;
            player2.color = 1;
		    printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mPlayer \033[0;36m%s \033[0;37mis %s!\n",player1.nickname,whichColor(player1.color));
		    printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mPlayer \033[0;36m%s \033[0;37mis %s!\n",player2.nickname,whichColor(player2.color));

            write(client1,&player1.color,sizeof(int));
            write(client2,&player2.color,sizeof(int));

            //game random
            int selectGame = randGame();
            if(selectGame == 1)
                printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mPlayers: \033[0;36m%s \033[0;37mand \033[0;36m%s \033[0;37mplay International checkers !\n",player1.nickname,player2.nickname);
            else
                printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mPlayers: \033[0;36m%s \033[0;37mand \033[0;36m%s \033[0;37mplay Canadian checkers !\n",player1.nickname,player2.nickname);
            write(client1,&selectGame,sizeof(int));
            write(client2,&selectGame,sizeof(int));
            
            //send size of board at clients
            size = selectSizeBoard(selectGame);
            printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mPlayers: \033[0;36m%s \033[0;37mand \033[0;36m%s \033[0;37mplay %d x %d table !\n",player1.nickname,player2.nickname,size,size);
            write(client1,&size,sizeof(int));
            write(client2,&size,sizeof(int));

            defaultBoard(selectGame,board);
            player1.score = 0;
            player2.score = 0;

            //<---------> SERVER GAMEPLAY SECTION <---------> 
            int gameLoop = 1;
            player1.move = 1;
            player2.move = 0;
            player1.winner = 0;
            player2.winner = 0;
            while(gameLoop){
               //send all board to clients
                for(int i = 0; i < size; ++i)
                    for(int j = 0; j < size; ++j){
                        write(client1,&board[i][j],sizeof(int));
                        write(client2,&board[i][j],sizeof(int));
                    } 
                
                int responseFromServer;         
                int colorRound;
                
                if(player1.move) printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mPlayer \033[0;36m%s \033[0;37mround!\n",player1.nickname);
                else printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mPlayer \033[0;36m%s \033[0;37mround!\n",player2.nickname);
                write(client1,&player1.move,sizeof(int));
                write(client2,&player2.move,sizeof(int));

                int manyPos = 0;
                do{
                    if(player1.move){
                        read(client1, &c, sizeof(int));
                        read(client1, &l, sizeof(int));
                        read(client1, &cFinal, sizeof(int));
                        read(client1, &lFinal, sizeof(int));
                        printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mPlayer \033[0;36m%s \033[0;37mchoose : %c%d -> %c%d !\n",player1.nickname,translateColumn(c),l ,translateColumn(cFinal),lFinal);
                    }
                    else{
                        read(client2, &c, sizeof(int));
                        read(client2, &l, sizeof(int));
                        read(client2, &cFinal, sizeof(int));
                        read(client2, &lFinal, sizeof(int));
                        printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mPlayer \033[0;36m%s \033[0;37mchoose : [%c%d] -> [%c%d] !\n",player2.nickname,translateColumn(c),l ,translateColumn(cFinal),lFinal);
                    }
                    c-=1;
                    l-=1;
                    cFinal-=1;
                    lFinal-=1;
                    printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mPositions : [%d,%d] -> [%d,%d] !\n",c,l,cFinal,lFinal);

                    if(player1.move) colorRound = 0;
                    else colorRound = 1; 
                    responseFromServer = goodPositions(board,size,c,l,cFinal,lFinal,colorRound,size); //its ok?
                    if(!responseFromServer)
		                printf("\033[0;32m[SERVER] \033[0;31mWARN   \033[0;37mInvalid Positions!\n");
                    if(player1.move) write(client1,&responseFromServer,sizeof(int));
                    else write(client2,&responseFromServer,sizeof(int));

                    if(responseFromServer)
                        printf("\033[0;32m[SERVER]  \033[0;34mINFO  \033[0;37mGood Positions!\n");
                    manyPos++;
                    if(manyPos == 3 && responseFromServer == 0){
                        if(player1.move) player2.winner = 1;
                        else player1.winner = 1;
                        responseFromServer = 1;
                    }
                }while(responseFromServer == 0);
                printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mScore: %d - %d !\n",player1.score,player2.score);   
                if(manyPos != 3)
                if(player1.move)
                    printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mPlayer \033[0;36m%s \033[0;37mmoved: [%c,%d] -> [%c,%d] !\n",player1.nickname,translateColumn(c+1),l,translateColumn(cFinal+1),lFinal);    
                else
                    printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mPlayer \033[0;36m%s \033[0;37mmoved: [%c,%d] -> [%c,%d] !\n",player2.nickname,translateColumn(c+1),l,translateColumn(cFinal+1),lFinal);

                //update
                printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mBoard Updated!!!\n");
                int scoreRound = updatePieces(c,l,cFinal,lFinal,colorRound,size,board);
                if(player1.move) player1.score+=scoreRound;
                else player2.score+=scoreRound;
                
                printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mScore: %d - %d !\n",player1.score,player2.score);

                for(int i = 0; i < size; ++i)
                    for(int j = 0; j < size; ++j){
                        write(client1,&board[i][j],sizeof(int));
                        write(client2,&board[i][j],sizeof(int));
                }
                if(size == 10 && player1.score == 20)
                    player1.winner = 1;
                else if(size == 10 && player2.score == 20)
                    player2.winner = 1;
                else if(size == 12 && player1.score == 30)
                    player1.winner = 1;
                else if(size == 12 && player2.score == 30)
                    player2.winner = 1;
                //choose round
                player1.move = !player1.move;
                player2.move = !player2.move;

                if(player1.winner || player2.winner){
                    winner = 1;
                    gameLoop = 0;
                }
                write(client1,&winner,sizeof(int));
                write(client2,&winner,sizeof(int));

            }
            write(client1,&player1.winner,sizeof(int));
            write(client2,&player2.winner,sizeof(int));
            if(player1.winner){
                printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mPlayer \033[0;36m%s \033[0;37mwin!\n",player1.nickname);
                printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mPlayer \033[0;36m%s \033[0;37mlose!\n",player2.nickname);
            }
            else{
                printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mPlayer \033[0;36m%s \033[0;37mwin!\n",player2.nickname);
                printf("\033[0;32m[SERVER] \033[0;34mINFO   \033[0;37mPlayer \033[0;36m%s \033[0;37mlose!\n",player1.nickname);
            }
            f=fopen("/home/ionel/Desktop/DraughtsS/scores.txt","a");
            fprintf(f,"%s vs %s ",player1.nickname,player2.nickname);
            if(size == 10){
                if(player1.winner)
                    fprintf(f,"(International checkers: 10x10) ------- Score: %d - %d ------- %s win!!! \n",player1.score,player2.score,player1.nickname);
                else
                    fprintf(f,"(International checkers: 10x10) ------- Score: %d - %d ------- %s win!!! \n",player1.score,player2.score,player2.nickname);
            }else{
                if(player1.winner)
                    fprintf(f,"(Canadian checkers: 12x12) ------- Score: %d - %d ------- %s win!!! \n",player1.score,player2.score,player1.nickname);
                else
                    fprintf(f,"(Canadian checkers: 12x12) ------- Score: %d - %d ------- %s win!!! \n",player1.score,player2.score,player2.nickname);
            }
            fclose(f);
            close(client1);
            close(client2);
            exit(0);
        }

    }
}  
