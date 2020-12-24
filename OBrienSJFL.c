/**
* Implements SJF algorithm on a given data set
* @author Noah O'Brien
* @version 1.0
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Process{

	int processNum;
	int tua;
	float alpha;
	int *t;

} Process;

// Global variables for amount of processes and ticks
int numPro;
int ticks;

// Initialize functions
int* smallestArray(Process *processes, int tickNum);
void swap(int *x, int *y);

// Combines folder and file name given from command line
char* readCommandLine(char* folder, char* file){

	// Allocates full input array
	int folderLen = strlen(folder);
	int length = strlen(folder) + strlen(file);
	char *input = malloc(sizeof(char*) * length);

	// Appends '/' to seperate folder and file
	folder[folderLen] = '/';

	// Appends the folder and file into one string
	strcpy(input, folder);

	return input;

}

// Reads the entirety of the given file
Process* readFile(FILE *file){

	// Checks to see if the file is open
	if(!file){

		printf("The file is not open. Please correct the file location.");
		exit(1);

	}

	// Reads the number of ticks and processes
	int i, j;
	fscanf(file, "%d", &ticks);
	fscanf(file, "%d", &numPro);

	// Allocates the correct amount of processes
	Process* info = (Process*) malloc(sizeof(Process) * numPro);

	for(i = 0; i < numPro; i++){

		// Allocates the correct amount of ticks per process
		info[i].t = malloc(sizeof(int) * ticks);

		// Reads the process number, TUA and alpha of that process
		fscanf(file, "%d", &info[i].processNum);
		fscanf(file, "%d", &info[i].tua);
		fscanf(file, "%f", &info[i].alpha);

		for(j = 0; j < ticks; j++){

			// For each tick, reads the t info of that process
			fscanf(file, "%d", &info[i].t[j]);

		}

	}

	return info;

	fclose(file);

}

// Sorts the burst value of all arrays from smallest to largest
int* smallestArray(Process *processes, int tickNum){

	int *values = malloc(sizeof(int) * numPro);
	int *final = malloc(sizeof(int) * numPro);
	int i, j, min;

	// Stores burst values into array to be sorted
	for(i = 0; i < numPro; i++){

		values[i] = processes[i].t[tickNum];

		//printf("%d", values[i]);

	}

	// Sorts burst value array
	for(i = 0; i < numPro; i++){

		min = i;
		for(j = i + 1; j < numPro; j++){

			if(values[j] < values[min]){

				min = j;

			}

		}

		swap(&values[min], &values[i]);

	}

	// Finds the index number of smallest number in processes, in order
	for(i = 0; i < numPro; i++){

		for(j = 0; j < numPro; j++){

			if(values[i] == processes[j].t[tickNum]){

				if(values[i] == values[i - 1]){

					j++;

				}else{

				final[i] = j;

				}

			}

		}

	}

	free(values);
	return final;

}

// Computes SJF algorithm on given file
void SJF(Process *processes){

	int prevTime, turn, wait, i, j, time = 0;
	turn = 0;
	wait = 0;
	int *min = malloc(sizeof(int) * numPro);

	printf("==Shortest-Job-First==\n");

	for(i = 0; i < ticks; i++){

		printf("Simulating tick #%d of processes at @ time %d:\n", i, time);

		min = smallestArray(processes, i);
		prevTime = time;

		for(j = 0; j < numPro; j++){

			printf(" Process %d took %d\n", min[j], processes[min[j]].t[i]);
			time += processes[min[j]].t[i];

		}

		wait += processes[min[0]].t[i];
		turn += ((processes[min[0]].t[i]) + (time - prevTime));

	}

	printf("Turnaround time: %d\n", turn);
	printf("Waiting time: %d\n", wait);

	free(min);
	min = NULL;

}

// Computes SJF Live algorithm on given file
void SJFLive(Process *processes){

	int error, find, x, prevTime, turn, wait, i, j, time = 0;
	turn = 0;
	wait = 0;
	find = 0;
	error = 0;
	int (*est)[ticks] = malloc(sizeof(int[numPro][ticks]));
	int *min = malloc(sizeof(int) * numPro);

	printf("\n==Shortest-Job-First Live==\n");

	for(i = 0; i < ticks; i++){

		printf("Simulating tick #%d of processes at @ time %d:\n", i, time);

		min = smallestArray(processes, i);
		prevTime = time;

		for(j = 0; j < numPro; j++){

			if(i == 0){

				est[min[j]][i] = processes[min[j]].tua;

			}else{

				est[min[j]][i] = ((processes[min[j]].alpha * processes[min[j]].t[i - 1]) + ((1 - processes[min[j]].alpha) *  est[min[j]][i - 1]));

			}

			if(processes[min[j]].t[i] > est[min[j]][i]){

				error += processes[min[j]].t[i] - est[min[j]][i];

			}else{

				error += est[min[j]][i] - processes[min[j]].t[i];

			}

			printf(" Process %d was estimated for %d and took %d\n", min[j], est[min[j]][i], processes[min[j]].t[i]);
			time += processes[min[j]].t[i];

		}

		for(x = 0; x < numPro + 1; x++){

			if(est[min[x]] < est[min[x] + 1]){

				find = est[min[x]][i];

			}

			if(est[min[x] + 1][i] <= est[min[x]][i]){

				find = est[min[x] + 1][i];

			}

		}

		wait += find;
		turn += (find + (time - prevTime));

	}

	printf("Turnaround time: %d\n", turn);
	printf("Waiting time: %d\n", wait);
	printf("Estimation Error: %d\n", error);

	free(min);
	free(*est);
	est = NULL;
	min = NULL;

}

// Swap function for selection sort
void swap(int *x, int *y){

	int temp = *x;
	*x = *y;
	*y = temp;

}

int main(int argc, char* argv[]) {

	// Reads local data file from command line
	char* input = readCommandLine(argv[1], argv[2]);

	// Opens file under specified location and frees used input array
	FILE *file = fopen(input, "r");
	free(input);
	input = NULL;

	// Reads the file and stores information into the struct
	Process *info = readFile(file);

	// Computes SJF algorithm on given processes
	SJF(info);

	// Computes SJF Live algorithm on given processes
	SJFLive(info);

	return 0;

}
