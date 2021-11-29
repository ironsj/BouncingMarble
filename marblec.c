#include <curses.h>
#include <term.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

//indicates that the process is supposed to keep running
bool running = true;

//signal handler for parent process (tells to stop running on ctrl-c)
void sigHandler(int number) {
	switch(number){
		case SIGINT:
			running = false;
			break;
		default:
			break;
	
	}
	
}

//signal handler for child process (tells to stop running when parent send SIGUSR1)
void sigHandler2(int number) {
	switch(number){
		case SIGUSR1:
			running = false;
			break;
		default:
			break;
	
	}
	
}

//function given to move cursor around screen from starter code
void move_cursor(int row, int col) {
	// get control characters for cursor positioning (cup)
    	char *cap = tigetstr("cup");
    	char *cmd = tparm(cap, row, col); // Insert row/column to the control chars
    	putp(cmd);                        // Send the control chars to the terminal
    	fflush(stdout);                   // flush the buffer for immediate effect
}

int main(int argc, char *argv[]){
	setupterm(NULL, fileno(stdout), NULL);
	int rows = tigetnum("lines");
    	int cols = tigetnum("cols");
    	putp(tigetstr("clear"));  // Clear screen
    	putp(tigetstr("smcup"));  // Enter cursor addressing mode
    	
    	srand(time(NULL));
    	int numRows = 1; //initially sets the number of rows to 1
    	int numCols = 2; //initially sets the number of cols to 2
    	if(argc >= 3){ //takes command line arguments for rows and cols if user includes them
    		numRows = atoi(argv[1]);
    		numCols = atoi(argv[2]);
    	}
    	
    	int numCells = numRows * numCols; //the number of total cells
    	int entered = 0; //initializes the number of times a cell has been entered to 0
    	int currCell = (rand() % (numRows * numCols)); //selects a random cell to start in
    	int cellRows = rows / numRows; //the number of rows per cell
    	int cellCols = cols / numCols; //the number of cols per cell
    	int completeRows = currCell / numCols; //variable that helps find random position in the cell
    	int completeCols = currCell - (completeRows * numCols); //variable that helps find random position in the cell
    	int xPos = (rand() % cellCols) + (completeCols * cellCols); //selects random x position in cell
    	int yPos = (rand() % cellRows) + (cellRows * completeRows); //selects random y position in cell
    	move_cursor(yPos, xPos); //moves cursor to random x and y position
    	
    	
    	int xDir; //variable for x direction
    	int yDir; //variable for y direction
    	if(argc == 3 || argc == 1){ //sets random x direction and y direction when user doesn't provide argument
	    	xDir = (rand() % 5) - 2;
	    	yDir = (rand() % 5) - 2;
	    	if(xDir == 0 && yDir == 0){ //if both x and y direction are 0, sets x to different direction that is not 0
	    		xDir = (rand() % 2) + 1;
	    	}
	}
	else{ //sets x and y direction to user specification if applicable
		xDir = atoi(argv[3]);
		yDir = atoi(argv[4]);
	}
    	
    	
    	
    	//initialize pipes for parent and child processes
    	int many_pvc[numCells * 2][2];
    	for (int k = 0; k < numCells * 2; k++){
  		pipe(many_pvc[k]);
  	}
  	
  	//array of child processes	
  	pid_t pids[numCells];
  	for (int i = 0; i < (numRows * numCols); ++i) {
  		if ((pids[i] = fork()) < 0) {
    			perror("fork");
    			abort();
  		} 
  		else if (pids[i] == 0) { //child processes do work within this branch
  			signal(SIGUSR1, sigHandler2); //initialize SIGUSR1 signal
			signal(SIGINT, SIG_IGN); //ignore ctrl-c
			close(many_pvc[i][0]); //close reading end for first pipe
			close(many_pvc[i+numCells][1]); //close writing end for second pipe
			int north = cellRows * (i / numCols); //north boundary of cell
			int south = north + cellRows; //south boundary of cell
			int west = (i % numCols) * cellCols; //west boundary of cell
			int east = west + cellCols; //east boundary of cell
			int marbleX; //variable for x position in cell
			int marbleY; //variable for y position in cell
			int x; //variable for x direction in cell
			int y; //variable for y direction in cell
			
			int buff[4]; //holds the integers that are read from the pipe
    			while(running){
    				read(many_pvc[i+numCells][0], buff, sizeof(int) * 4); //reads from pipe when sent from parent process
    				if(buff[0] == -1){ //parent process sends -1 when ctrl-c is pressed
    					move_cursor(north + 4, west + 1);
    					printf("%d terminated.", getpid()); //termination message
    					
    				}
    				else{
	    				entered++; //increment the number of times the cell has been entered
	    				//start of code that creates visual of ACTIVE cell walls
	    				move_cursor(north + 1, west +1);
					printf("\u250F");
					move_cursor(south - 1, west + 1);
					printf("\u2517");
					move_cursor(north + 1, east - 1);
					printf("\u2513");
					move_cursor(south - 1, east - 1);
					printf("\u251B");
	    				for(int i = west + 2; i < east - 1; i++){
						move_cursor(north + 1, i);
						printf("\u2501");
						move_cursor(south - 1, i);
						printf("\u2501");
					}
					for(int i = north + 2; i < south - 1; i++){
						move_cursor(i, west + 1);
						printf("\u2503");
						move_cursor(i, east - 1);
						printf("\u2503");
					}
					move_cursor(north + 1, east - (cellCols / 4));
	    				printf("%d", entered);
					fflush(stdout);
					//end of code that creates visual of ACTIVE cell walls
					
					marbleX = buff[0]; //x position of marble from parent
					marbleY = buff[1]; //y position of marble from parent
					x = buff[2]; //x direction of marble from parent
					y = buff[3]; //y direction of marble from parent
					move_cursor(rows, 0);
					printf("Child %d is in charge of marble", getpid()); //indicates which child process is in controll
					move_cursor(marbleY, marbleX);
					
					
					while(marbleX <= east && marbleX >= west && marbleY >= north && marbleY <= south){ //while marble is within cell
	    					marbleY += y; //update y position
	    					marbleX += x; //update x position
	    					//send marble opposite direction if wall is hit
	    					if(marbleX >= (cellCols * numCols) || marbleX <= 0){
	    						x *= -1;
	    						marbleX += x;
	    					}
	    					if(marbleY <= 0 || marbleY >= (cellRows * numRows)){
	    						y *= -1;
	    						marbleY += y;
	    					}
	    					
	    					//move cursor to updates marbleX and marbleY
	    					move_cursor(marbleY, marbleX);
						usleep(250000);
		    				
					}
					
					
					if(marbleX <= west){ //child passes west boundary
						//START OF CODE TO MAKE  NONE ACTIVE CELL WALLS
	    					move_cursor(north + 1, west +1);
						printf("\u250C");
						move_cursor(south - 1, west + 1);
						printf("\u2514");
						move_cursor(north + 1, east - 1);
						printf("\u2510");
						move_cursor(south - 1, east - 1);
						printf("\u2518");				
						for(int i = west + 2; i < east - 1; i++){
							move_cursor(north + 1, i);
							printf("-");
							move_cursor(south - 1, i);
							printf("-");
						}
						for(int i = north + 2; i < south - 1; i++){
							move_cursor(i, west + 1);
							printf("|");
							//usleep(2500);
							move_cursor(i, east - 1);
							printf("|");
							//usleep(2500);
						}
						move_cursor(north + 1, east - (cellCols / 4));
				    		printf("%d", entered);
						fflush(stdout);				
	    					//END OF CODE TO MAKE  NONE ACTIVE CELL WALLS
	    					
	    					int buff2[5]; //holds information to write to parent
	    					if(marbleY <= north){
	    						buff2[0] = i - numCols - 1; //sends new cell to north west
	    					}
	    					else if(marbleY >= south){ //sends new cell to to south
	    						buff2[0] = i + numCols - 1;
	    					}
	    					else{ //sends new cell to the west
	    						buff2[0] = i - 1;
	    					}
	    					buff2[1] = marbleX; //sends back current x position
	    					buff2[2] = marbleY; //send back current y position
	    					buff2[3] = x; //sends back x direction
	    					buff2[4] = y; //sends back y direction
	    					//write to information of cell to parent process
	    					write(many_pvc[i][1], buff2, sizeof(int) * 5);
	    				
					}
	    				else if(marbleX >= east){ //code is the same as the west branch but updated for when the east boundary is passed
	    					move_cursor(north + 1, west +1);
						printf("\u250C");
						move_cursor(south - 1, west + 1);
						printf("\u2514");
						move_cursor(north + 1, east - 1);
						printf("\u2510");
						move_cursor(south - 1, east - 1);
						printf("\u2518");
						for(int i = west + 2; i < east - 1; i++){
							move_cursor(north + 1, i);
							printf("-");
							move_cursor(south - 1, i);
							printf("-");
						}
						for(int i = north + 2; i < south - 1; i++){
							move_cursor(i, west + 1);
							printf("|");
							move_cursor(i, east - 1);
							printf("|");
						}
						move_cursor(north + 1, east - (cellCols / 4));
				    		printf("%d", entered);
						fflush(stdout);    				
	    					
	    					int buff2[5];
	    					if(marbleY <= north){
	    						buff2[0] = i - numCols + 1;
	    					}
	    					else if(marbleY >= south){
	    						buff2[0] = i + numCols + 1;
	    					}
	    					else{
	    						buff2[0] = i + 1;
	    					}
	    					buff2[1] = marbleX;
	    					buff2[2] = marbleY;
	    					buff2[3] = x;
	    					buff2[4] = y;
	    					write(many_pvc[i][1], buff2, sizeof(int) * 5);
	    				
	    				}
	    				else if(marbleY <= north){ //code is the same as the west branch but updated for when the north boundary is passed
	    					move_cursor(north + 1, west +1);
						printf("\u250C");
						move_cursor(south - 1, west + 1);
						printf("\u2514");
						move_cursor(north + 1, east - 1);
						printf("\u2510");
						move_cursor(south - 1, east - 1);
						printf("\u2518");    				
						for(int i = west + 2; i < east - 1; i++){
							move_cursor(north + 1, i);
							printf("-");
							move_cursor(south - 1, i);
							printf("-");
						}
						for(int i = north + 2; i < south - 1; i++){
							move_cursor(i, west + 1);
							printf("|");
							move_cursor(i, east - 1);
							printf("|");
						}
						move_cursor(north + 1, east - (cellCols / 4));
				    		printf("%d", entered);
						fflush(stdout);    				
	    					
	    					int buff2[5];
	    					buff2[0] = i - numCols;
	    					buff2[1] = marbleX;
	    					buff2[2] = marbleY;
	    					buff2[3] = x;
	    					buff2[4] = y;
	    					write(many_pvc[i][1], buff2, sizeof(int) * 5);
	    				
	    				}
	    				else if(marbleY >= south){ //code is the same as the west branch but updated for when the south boundary is passed
	    					move_cursor(north + 1, west +1);
						printf("\u250C");
						move_cursor(south - 1, west + 1);
						printf("\u2514");
						move_cursor(north + 1, east - 1);
						printf("\u2510");
						move_cursor(south - 1, east - 1);
						printf("\u2518");    				
						for(int i = west + 2; i < east - 1; i++){
							move_cursor(north + 1, i);
							printf("-");
							move_cursor(south - 1, i);
							printf("-");
						}
						for(int i = north + 2; i < south - 1; i++){
							move_cursor(i, west + 1);
							printf("|");
							move_cursor(i, east - 1);
							printf("|");
						}
						move_cursor(north + 1, east - (cellCols / 4));
				    		printf("%d", entered);
						fflush(stdout);    				
	    					int buff2[5];
	    					buff2[0] = i + numCols;
	    					buff2[1] = marbleX;
	    					buff2[2] = marbleY;
	    					buff2[3] = x;
	    					buff2[4] = y;
	    					write(many_pvc[i][1], buff2, sizeof(int) * 5);
	    				}
		    				
	    				
	    			
	    			}
    			}
    			fflush(stdout); //allows termination message to be in each child process cell
    			return 0; //allows child processes to terminate
  		}
  			
	}
	
	/*
	PARENT PROCESS CODE BELOW
	*/
	
	signal(SIGINT, sigHandler); //sends ctrl-c to signal handler for parent process
	
	//START OF CODE TO CREATE VISUAL CELLS FOR EACH CHILD PROCESS
	for(int i = 0; i < numCells; i++){
		int north = cellRows * (i / numCols);
		int south = north + cellRows;
		int west = (i % numCols) * cellCols;
		int east = west + cellCols;
		move_cursor(north + 1, west +1);
		printf("\u250C");
		move_cursor(south - 1, west + 1);
		printf("\u2514");
		move_cursor(north + 1, east - 1);
		printf("\u2510");
		move_cursor(south - 1, east - 1);
		printf("\u2518");
		for(int i = west + 2; i < east - 1; i++){
			move_cursor(north + 1, i);
			printf("-");
			move_cursor(south - 1, i);
			printf("-");
		}
		for(int i = north + 2; i < south - 1; i++){
			move_cursor(i, west + 1);
			printf("|");
			move_cursor(i, east - 1);
			printf("|");
		}
		move_cursor(north + 1, east - (cellCols / 4));
    		printf("%d", entered);
		fflush(stdout);
	}
	//END OF CODE THAT CREATES VISUAL CELLS FOR EACH CHILD PROCESS
	
	int buff[4]; //holds information that is written to the appropriate child process
	buff[0] = xPos; //current x position of marble
	buff[1] = yPos; //curent y position of marble
	buff[2] = xDir; //current x direction of marble
	buff[3] = yDir; //current y direction of marble
	for(int i = 0; i < numCells; i++){
		close(many_pvc[i][1]); //closes pipes for writing
		close(many_pvc[i+numCells][0]); //closes pipes for reading
	}
	
	
	while(running){
		write(many_pvc[currCell+numCells][1], buff, sizeof(int) * 4); //writes to appropriate child
		
		int buff2[5]; //holds information returned from child process
		read(many_pvc[currCell][0], buff2, sizeof(int) * 5); //read from child process sending back information
		currCell = buff2[0]; //the new current cell the marble is in
		buff[0] = buff2[1]; //the new x position
		buff[1] = buff2[2]; //the new y position
		buff[2] = buff2[3]; //the new x direction
		buff[3] = buff2[4]; //the new y direction
	}
	
	
	for(int i = 0; i < numCells; i++){
		kill(pids[i], SIGUSR1); //send SIGUSR1 to children after ctrl-c
		buff[0] = -1;
		write(many_pvc[i + numCells][1], buff, sizeof(int) * 4); //write -1 to child processes to make them print termination message
		
		usleep(25000);
		
	}
	
	
	//closes remaining pipess
	for(int i = 0; i < numCells; i++){
		close(many_pvc[i][0]);
		close(many_pvc[i+numCells][1]);
	} 
	
	//waits for children to terminate
	int status;
	for(int i=0;i<5;i++){
		wait(&status);
	}
   	
   	
   	sleep(10);
    	reset_shell_mode();

    	// Move the cursor below the horizontal center
    	move_cursor(rows/2+5, 0);
    	putp(tigetstr("rmcup"));  // Exit cursor addressing mode	
    	
    	



	return 0;
}
