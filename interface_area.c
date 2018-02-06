//compile with -std=gnu11
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define CHAINS 8

char **strsplit(const char* pdb_line, const char* delim, int * token_count) {
    char *s = strdup(pdb_line);      			//Create a copy of the string

    size_t allocated = 1, used =0;
    char **tokens = calloc(allocated, sizeof(char*));
    char *token, *strtok_ctx;
    for (token = strtok_r(s, delim, &strtok_ctx);	//Split input string
            token != NULL;
            token = strtok_r(NULL, delim, &strtok_ctx)) {
        if (used == allocated) {			//Increase allocated space if needed
            allocated *= 2;
            tokens = realloc(tokens, allocated * sizeof(char*));
        }
        tokens[used++] = strdup(token);
    }

    if (used == 0) {					//Free Memory
        free(tokens);
        tokens = NULL;
    } else {
        tokens = realloc(tokens, used * sizeof(char*));
    }
    * token_count = used;
    free(s);
    return tokens;
}

int main(void) {
    FILE * pdb;
    FILE** aa = malloc(sizeof(FILE*) * CHAINS);
    char *line = NULL;
    size_t len;
    ssize_t read;
    char **tokens;
    int token_count;

    pdb = fopen("5un8.pdb", "r");								//Check if file is valid
    if (pdb == NULL)
        exit(EXIT_FAILURE);

    struct stat st = {0};
    if (stat("/pops", &st) == -1) {
        mkdir("/pops", 0700);
    }

    char buffer[15], chain[2];
    int i;
    for(i=0; i<CHAINS; i++){
        snprintf(buffer, sizeof(char) * 15, "pops/%c.pdb", i+65);
        aa[i] = fopen (buffer,"w");
        fclose (aa[i]);
    }

    while ((read = getline(&line, &len, pdb)) != -1) {						                        //Read one line at a time
    	tokens = strsplit(line, " \t\n", &token_count);
    	if(!strcmp(tokens[0],"ATOM")){
	    for(i=0; i<CHAINS; i++){
		snprintf(chain, sizeof(char) * 2, "%c", i+65);
	        if(!strcmp(tokens[4],chain)){
		    snprintf(buffer, sizeof(char) * 15, "pops/%c.pdb", i+65);
		    aa[i] = fopen (buffer,"a");
		    if(token_count==12)
                        fprintf (aa[i], "%s  %5s  %-3s %3s %s%4s    %8s%8s%8s  %4s %5s           %s\n",	//Write to file
                            tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], tokens[5], tokens[6],
                            tokens[7], tokens[8], tokens[9], tokens[10], tokens[11]);
            	    else
                	fprintf( aa[i], "%s  %5s  %-3s %3s %s%4s    %8s%8s%8s  %10s           %s\n",	//Write to file
                            tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], tokens[5], tokens[6],
                            tokens[7], tokens[8], tokens[9], tokens[10]);
		    fclose (aa[i]);
		    break;
                }
            }
        }
        if (tokens != NULL)
            free(tokens);
    }
    if (line != NULL) free(line);
    fclose(pdb);

    return EXIT_SUCCESS;
}
