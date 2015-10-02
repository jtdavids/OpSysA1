#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


void writeToLog(char *message);
char* PROCNANNYLOGS = "../tmp/logfile.txt";
//Procnanny
int main(int argc, char* argv[])
{
	printf("test");
    char const* const filename = argv[1];
    FILE* config = fopen(filename, "r"); 
    char line[256];
    char processnames[129][256];
   
    int killtime;

    int pnames_counter = 0;
    while (fgets(line, sizeof(line), config)) {
    	if (pnames_counter == 0){
    		killtime = atoi(line);
    		pnames_counter++;
    	} else{
        	strcpy(processnames[pnames_counter],line);
        	//printf("%s\n",processnames[pnames_counter]);
        	//printf("test");
			//printf("%s\n",line);
        	pnames_counter++;
    	}
    	printf("%d\n", pnames_counter);
    }
    fclose(config);

   

   	//clear previous data
   	FILE *logfile;
    logfile = fopen(PROCNANNYLOGS, "w");
    fclose(logfile);
   

	FILE *file;
	int checksuccess;
	int found_pid = 0;
	char pid[256];
	char pnames[262] = "pgrep ";
	int pidtokill;
	int processkilled_count = 0;
	int child;
	char msg[256];
	int i = 0;
	for (i = 1; i<pnames_counter; i++){
		strcpy(pnames, "pgrep ");
		strtok(processnames[i],"\n");
		strcat(pnames, processnames[i]);
		printf("%s\n",pnames);

		file = popen( pnames , "r");
		if (file == NULL){
			//popen error
		}
		while (fgets(pid, 256, file) != NULL){
			printf("%s\n",pid);
			found_pid = 1;
			if((child = fork()) < 0){

			} else if (child == 0){
				//child
				pidtokill = atoi(pid);
				strcpy(msg, "Info: Initializing monitoring of process '");
				strtok(processnames[i],"\n");
				strcat(msg, processnames[i]);
				strcat(msg,"' (PID ");
				strtok(pid,"\n");
				strcat(msg, pid);
				strcat(msg, ").");
				writeToLog(msg);

				sleep(killtime);
				printf("sleep %s\n",pid);
				int result = kill(pidtokill,1);
				char kill_time[6];
				sprintf(kill_time, "%d", killtime);

				if (result==0){
					//successful exit
					strcpy(msg, "Action: PID "); 
					strtok(pid,"\n");
					strcat(msg, pid);
					strcat(msg, " (");
					strcat(msg, processnames[i]);
					strtok(processnames[i],"\n");
					strcat(msg, ") killed after exceeding ");
					strcat(msg, kill_time);
					strcat(msg, " seconds.");
					writeToLog(msg);
					exit(0);
				} else {
					printf("%d killed early\n", pidtokill);
					//process exited earlier, no need to log output.
					exit(-1);
				}

			}
			processkilled_count++;
		}
		if (found_pid == 0){
			printf("%s file null\n",pnames);
			//process is not running
			strcpy(msg, "Info: No '");
			strtok(processnames[i],"\n");
			strcat(msg, processnames[i]);
			strcat(msg,"' proccesses found.");
			writeToLog(msg);
			continue;
		}
		found_pid = 0;

		checksuccess = pclose(file);
		if (checksuccess == -1) {
		    //report error and break?
		    break;
		}
	}
	int k;
	int exit_status;
	printf("processes: %d\n", processkilled_count);
	for (k = 0; k < processkilled_count; k++){
		wait(&exit_status);
	}
	return 0;
}

void writeToLog(char *message){
	FILE *logfile;
    logfile = fopen(PROCNANNYLOGS, "a");
    time_t date;
    fprintf(logfile, "[%s] %s\n",ctime(&date),message);
    fclose(logfile);
}








// TODO *****

//1) ----------Read in config file

//2) generate log file

//3) Find which processes are running/need to be monitored

//4) fork children to monitor process

//5) Send/recieve message between children/parent when process is to be killed

//6) Kill processes

//7) Send processes killed info to logfile

//8) exit


