//compile with -std=gnu11
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
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
    FILE** asa = malloc(sizeof(FILE*) * (CHAINS+1));
    FILE *final, *final_sp;
    char **line = calloc((CHAINS+1), sizeof(char*));
    char * line_sp = NULL;
    size_t len, len_res, len_sp;
    ssize_t *read = malloc(sizeof(size_t) * (CHAINS+2));
    char **tokens, **tokens_res, **tokens_sp;
    int token_count, token_count_res, token_count_sp;
    int i,n=0;
    char buffer[15], chain[2];
    size_t allocated = 1, used =0;
    float x,y,z,x1,y1,z1,dist;

    asa[0] = fopen("5un8.pdb", "r");	    							//Check if file is valid
    if (asa[0] == NULL)
        exit(EXIT_FAILURE);
    for(i=0;i<CHAINS;i++){
        snprintf(buffer, sizeof(char) * 15, "asa_out/%c.asa", i+65);
        asa[i+1] = fopen(buffer, "r");								//Check if file is valid
        if (asa[i+1] == NULL)
            exit(EXIT_FAILURE);
    }

    final = fopen ("final.pdb","w");
    fclose (final);
    final_sp = fopen ("final_sp.pdb","w");
    fclose (final_sp);

    
    for(i=0; i<CHAINS; i++){
        snprintf(chain, sizeof(char) * 2, "%c", i+65);
	    while ((read[i+1] = getline(&line[i+1], &len_res, asa[i+1])) != -1){
	       tokens_res = strsplit(line[i+1], " \t\n", &token_count_res);
	    	if(token_count_res==7){
                while ((read[0] = getline(&line[0], &len, asa[0])) != -1) {                                 //Read one line at a time
                    tokens = strsplit(line[0], " \t\n", &token_count);
                    if(!strcmp(tokens[0],"ATOM")){
        		        if(!strcmp(tokens[1],tokens_res[0])){
                            x=atof(tokens[6]);
                            y=atof(tokens[7]);
                            z=atof(tokens[8]);
                            break;
                        }
                    }
                }
                line[0]=NULL;
                fseek(asa[0],0, SEEK_SET);
                while ((read[CHAINS+1] = getline(&line_sp, &len_sp, asa[0])) != -1) {
                    tokens_sp = strsplit(line_sp, " \t\n", &token_count_sp);
                    if(!strcmp(tokens_sp[0],"ATOM")){
                        x1=atof(tokens[6]);
                        y1=atof(tokens[7]);
                        z1=atof(tokens[8]);
                        dist = sqrt(pow(x-x1,2) + pow(y-y1,2) + pow(z-z1,2));
                        if(dist<=6.0 && strcmp(tokens_sp[4],chain)){
                            // printf("%s\n","hey" );
                            final = fopen ("final.pdb","a");
                            final_sp = fopen ("final_sp.pdb","a");
                            // printf("%s\t\t%s\n",chain, tokens_sp[4] );
                            // if(token_count_sp==12)
                            //     fprintf (final, "%s  %5s  %-3s %3s %s%4s    %8s%8s%8s  %4s %5s           %s\n", //Write to file
                            //             tokens_sp[0], tokens_sp[1], tokens_sp[2], tokens_sp[3], tokens_sp[4], tokens_sp[5], tokens_sp[6],
                            //             tokens_sp[7], tokens_sp[8], tokens_sp[9], tokens_sp[10], tokens_sp[11]);
                            // else
                            //     fprintf (final, "%s  %5s  %-3s %3s %s%4s    %8s%8s%8s  %10s           %s\n",    //Write to file
                            //             tokens_sp[0], tokens_sp[1], tokens_sp[2], tokens_sp[3], tokens_sp[4], tokens_sp[5], tokens_sp[6],
                            //             tokens_sp[7], tokens_sp[8], tokens_sp[9], tokens_sp[10]);
                            fclose (final);
                            fclose(final_sp);
                        }
                    }
                }
                fseek(asa[0],0, SEEK_SET);
                line_sp = NULL;
                printf("%d\n", i);
            }
	    }
    }

    return EXIT_SUCCESS;
}
