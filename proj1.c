#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>


typedef int (gvpipe_t) [2];      
const int READ = 0;
const int WRIT = 1;
int i,val;
int z = 0;
int count = 0;
int size = 10;
void sChild(int id, gvpipe_t fd[], int N);
int compare (const void * a, const void * b);


/**********************************************************************
* main takes in a number that represents the number of files to sort.
* takes in the file names ending in .txt and sorts them using pipes. 
**********************************************************************/
int main (void) 
{
    gvpipe_t *pip;
    pid_t who;
    int numFiles,k;
    char input[100];     
    	
	printf("************************************************************\n");
    	fputs("How many sorting files?  \nPlease enter in a number in a power of 2\n> ", stdout);

	fflush(stdout);
	if (fgets(input, sizeof input, stdin) != NULL) 
	{
	      char *scanIn = strchr(input, '\n');
	      if (scanIn != NULL)
		  *scanIn = '\0';
    	}

    numFiles = atoi (input);
    pip = (gvpipe_t *) calloc (numFiles, sizeof(gvpipe_t));
    
		printf("*****************\n");
    		printf("Creating pipes!\n");
		printf("*****************\n");

    for (k = 0; k < numFiles; k++)
	{
     		pipe (pip[k]);
	}

		printf("**************************\n");
    		printf("Creating children by fork!\n");
		printf("**************************\n");
    
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
	int high = -1; 
	  
	for (z = 0; z < numFiles; z++) 
	{
		values[z] = 0;
		pipes[z] = 1;
	      	counts[z] = 0;
	}

	int pipeNumber = 0;
	int t = 0;
	count = 0;
	while (count < size * numFiles) 
	{
		count++;	    
	      	for (t = 0; t < numFiles; t++) 
		{
			if (pipes[t] == 1 && counts[t] < size) 
			{
		  		read(pip[t][READ], &values[t], sizeof(int));
		  		pipes[t] = 0;
		  		counts[t]++;
			}
	      	}
      
	       pipeNumber = 0;
	       for (z = 0; z < numFiles-1; z++) 
		{
			if (z == 0) {
		  	high = values[z];
		  	pipeNumber = z;
		 }
	       if (high > values[z+1]) 
		{
			high = values[z+1];
		    	pipeNumber = z+1;
   	        } 
		else if (high == values[z+1]) 
		{
			if (counts[z] < counts[z+1]) 
			{
		      		high = values[z];
		      		pipeNumber = z;
		        } 
			else {
		      		high = values[z+1];
		      		pipeNumber = z+1;
		    	}
		  }
		  
		  if (counts[z] > size) {
		      	high = z + 1;
		      	pipeNumber = z + 1;
		  }
		  if (counts[z+1] > size) {
		      	high = z;
		      	pipeNumber = z;
		  }
		}
		
		printf("Piping out...  %d\n", high); 
		pipes[pipeNumber] = 1; 
	    }

     /* now wait for all children to stop */
    for (k = 0; k < numFiles; k++) {
         	int stat;
         	wait (&stat);
     }
   
    free (pip);
    return 0;	
}



//		Child process
void sChild(int id, gvpipe_t fd[], int N) {

	char *fileName;
	char input[100];
	
	fputs("Please enter a file name to sort\n> ", stdout);
	fflush(stdout);
	if (fgets(input, sizeof input, stdin) != NULL) 
	{
	      char *scanIn = strchr(input, '\n');
	      if (scanIn != NULL)
		  	*scanIn = '\0';
	}

	fileName = input;
	int size = 80;
	char line[80];
	FILE *fr;
	fr = fopen(fileName, "rt");
	int temp;
	int arrayOfInt[80];
	 i=0;
	printf("printing out file\n");
	while(fgets(line,80, fr) !=NULL){
	  
	  	sscanf(line, "%d", &temp);
	  	arrayOfInt[i] = temp;
	  	printf("unsorted : %i\n",temp);
	  	i++;
	}
	size=i;
	
	int x = 0;	
	printf("Sorting!\n");
    
	qsort (arrayOfInt, size, sizeof(int), compare);
	
	while(x < i)
	{
	    	printf("sorted : %d\n", arrayOfInt[x]);	
	      	write (fd[id][WRIT], &arrayOfInt[x], sizeof(int));
	    	x++;
	}
	
	fclose(fr);
	close(fd[id][WRIT]);
 	close(fd[id][READ]);
}



//compaiing 2 integers
int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

