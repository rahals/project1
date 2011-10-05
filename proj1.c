#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/resource.h>


typedef int (gvpipe_t) [2];      
const int READ = 0;
const int WRIT = 1;
int i,val;
int z = 0;
int count = 0;
int size = 10;

void sChild(int id, gvpipe_t fd[], int N);
int compare (const void * a, const void * b);



int main (void) 
{
    gvpipe_t *pip;
    pid_t who;
    int numFiles,k;
    char input[100];     
    
	printf("new2");
    	fputs("How many sorting files?  Please enter in a number in a power of 2\n", stdout);
	fflush(stdout);
	if (fgets(input, sizeof input, stdin) != NULL) 
	{
	      char *newLine = strchr(input, '\n');
	      if (newLine != NULL)
		  *newLine = '\0';
    	}

    numFiles = atoi (input);
    pip = (gvpipe_t *) calloc (numFiles, sizeof(gvpipe_t));
    printf("Creating pipes!\n");
    for (k = 0; k < numFiles; k++)
	{
     		pipe (pip[k]);
	}

    printf("Creating children!\n");
    for (k = 0; k < numFiles; k++) 
	{
        	who = fork ();
		int stat;
        	wait (&stat);
        	if (who == 0) 
		{
			sChild(k, pip, numFiles);
			exit (0);           
        	}
    	}
  
      
	
	  int values[numFiles];
	  int pipes[numFiles];
	  int counts[numFiles];
	  int winner = -1; 
	  
	  //Everything starts out as 0!
	  for (z = 0; z < numFiles; z++) {
	      values[z] = 0;
	      pipes[z] = 1;
	      counts[z] = 0;
	  }

	int pipeNumber = 0;
	  int t = 0;
	  count = 0;
	  while (count < size * numFiles) {
	    
	    count++;	    
        /********************************************************************************************************/
	      /*This should read in each time*/
	      for (t = 0; t < numFiles; t++) {
		if (pipes[t] == 1 && counts[t] < size) {
		  read(pip[t][READ], &values[t], sizeof(int));
		  //printf("Read from pipe %d: value %d\n", z, values[t]);
		  pipes[t] = 0;
		  counts[t]++;
		}
	      }//end of for loop
      
	      pipeNumber = 0;
	      for (z = 0; z < numFiles-1; z++) {
		printf("Pipping out...");
		if (z == 0) {
		  winner = values[z];
		  pipeNumber = z;
		 }
		  if (winner > values[z+1]) {
		    winner = values[z+1];
		    pipeNumber = z+1;
		  } else if (winner == values[z+1]) {
		    if (counts[z] < counts[z+1]) {
		      winner = values[z];
		      pipeNumber = z;
		    } else {
		      winner = values[z+1];
		      pipeNumber = z+1;
		    }
		  }
		  
		  if (counts[z] > size) {
		      winner = z + 1;
		      pipeNumber = z + 1;
		  }
		  if (counts[z+1] > size) {
		      winner = z;
		      pipeNumber = z;
		  }
		}//end of forloop
		
		printf("  %d\n", winner); //This is the lowest number
		pipes[pipeNumber] = 1; //this sets the pipe to be read again
	    }
      
       /********************************************************************************************************/
	   


     /* now wait for all children to stop */
    for (k = 0; k < numFiles; k++) {
         int stat;
         wait (&stat);
     }
   
    free (pip);
    return 0;	
}


/**********************************************************
 * 		Child process
 **********************************************************/
void sChild(int id, gvpipe_t fd[], int N) {

	char *fileName;
	char input[100];
	
	fputs("Please enter a file name to sort\n", stdout);
	fflush(stdout);
	if (fgets(input, sizeof input, stdin) != NULL) {
	      char *newLine = strchr(input, '\n');
	      if (newLine != NULL)
		  *newLine = '\0';
	}

	fileName = input;
	int size = 80;
	char line[80];
	FILE *fr;
	fr = fopen(fileName, "rt");
	int temp;
	int arrayOfInt[80];
	 i=0;
	
	while(fgets(line,80, fr) !=NULL){
	  
	  sscanf(line, "%d", &temp);
	  arrayOfInt[i] = temp;
	  i++;
	}
	size=i;
	
	int x = 0;	
	printf("Sorting!\n");
    
	qsort (arrayOfInt, size, sizeof(int), compare);
	
	while(x < i){
	    printf("%d\n", arrayOfInt[x]);	
	      write (fd[id][WRIT], &arrayOfInt[x], sizeof(int));
	    x++;
	}
	
	fclose(fr);
	close(fd[id][WRIT]);
 	close(fd[id][READ]);
}


//used to compair the 2 ints passed by the pipe
int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

