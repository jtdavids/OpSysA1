#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "memwatch.h"


void writeToLog(char *message);
void killProcNanny();
char log_name[13] = "PROCNANNYLOGS";
char *output_log;
char *home_dir;
//Procnanny
int main(int argc, char* argv[])
{
    char *filename = argv[1];
    FILE* config = fopen(filename, "r"); 
    char line[256];
    char processnames[129][256];
    output_log = getenv(log_name);
    home_dir = getenv("HOME");
    if (output_log[0] == '~') {
    	memmove(output_log, output_log+1, strlen(output_log));
    }
    strcat(home_dir, output_log);
    output_log = home_dir;
   
    int killtime;  

    killProcNanny();

    int pnames_counter = 0;
    while (fgets(line, sizeof(line), config)) {
    	if (pnames_counter == 0){
    		killtime = atoi(line);
    		pnames_counter++;
    	} else{
        	strcpy(processnames[pnames_counter],line);
        	pnames_counter++;
    	}
    }
    fclose(config);

   

   	//clear previous data
   	FILE *logfile;
    logfile = fopen(output_log, "w");
    fflush(logfile);
    fclose(logfile);
   

	FILE *file;
	int checksuccess;
	int found_pid = 0;
	char pid[256];
	char pnames[262] = "pgrep ";
	int pidtokill;
	int processkilled_count = 0;
	int child;
	char msg[512];
	int i = 0;
	int k;
	int exit_status = 0;
	char processes_killed[3];
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
					exit(0);
				}

			}
			processkilled_count++;
		}
		if (found_pid == 0){
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
	for (k = 0; k < processkilled_count; k++){
		wait(&exit_status);
		printf("process: %d\n",k);
		printf("exit status: %d\n",exit_status);

	}
	sprintf(processes_killed, "%d", processkilled_count);
	strcpy(msg, "Info: Exiting. ");
	strtok(processes_killed,"\n");
	strcat(msg, processes_killed);
	strcat(msg," proccess(es) killed.");
	writeToLog(msg);
	return 0;
}

void writeToLog(char *message){
	FILE *logfile;
    logfile = fopen(output_log, "a");
    time_t date;
    fprintf(logfile, "[%s] %s\n",ctime(&date),message);
    fflush(logfile);
    fclose(logfile);
}

void killProcNanny(){
	FILE *file;
	int checksuccess;
	char proc_pid[256];
	int pidkill;
	char procnan[15] = "pgrep procnanny";
	pid_t parent_pid = getpid();
	file = popen( procnan , "r");
	if (file == NULL){
		//popen error
	}
	while (fgets(proc_pid, 256, file) != NULL){
		pidkill = atoi(proc_pid);
		if(pidkill != parent_pid){
			kill(pidkill,1);
		}
	}
			
}








