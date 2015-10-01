#include <stdio.h>

char PIDS[128][10];
void getProcesses(char *processes);

//Procnanny
int main(int argc, char* argv[])
{
    char const* const filename = argv[1];
    FILE* config = fopen(filename, "r"); 
    char line[256];
    char processnames[129][256];

    int i = 0;
    while (fgets(line, sizeof(line), config)) {
        *processnames[i] = line;
        i++;
    }
    fclose(config);

   	int killtime = (int) processnames[0];

   	getProcesses(*processnames);


}

void getProcesses(char *processes){
	FILE *file;
	int checksuccess;
	char pid[256];
	char pnames[270] = "pgrep -u user ";
	int i;
	for (i = 1; i<129; i++){
		strcat(pnames, processes[i]);

		file = popen( pnames , "r");
		if (file == NULL){

		}
		while (fgets(pid, 256, file) != NULL){
		 	*PIDS[0] = pid;   
		}

		checksuccess = pclose(file);
		if (checksuccess == -1) {
		    //report error and break?
		    break;
		}
	}
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


