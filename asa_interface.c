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
    FILE** asa = malloc(sizeof(FILE*) * (CHAINS+1));
    FILE** aa = malloc(sizeof(FILE*) * (CHAINS+1));
    char **line = calloc((CHAINS+1), sizeof(char*));
    size_t len, len_res;
    ssize_t *read = malloc(sizeof(size_t) * (CHAINS+1));
    char **tokens, **tokens_res;
    int token_count, token_count_res;
    int i;
    char buffer[15], chain[2];
    float *area = calloc((CHAINS), sizeof(float));

    asa[0] = fopen("asa/pdb.asa", "r");	    							//Check if file is valid
    if (asa[0] == NULL)
        exit(EXIT_FAILURE);
    for(i=0;i<CHAINS;i++){
        snprintf(buffer, sizeof(char) * 15, "asa/%c.asa", i+65);
        asa[i+1] = fopen(buffer, "r");								//Check if file is valid
        if (asa[i+1] == NULL)
            exit(EXIT_FAILURE);
    }

    struct stat st = {0};
    if (stat("./asa_out", &st) == -1) {
        mkdir("./asa_out", 0700);
    }

    for(i=0; i<CHAINS; i++){
        snprintf(buffer, sizeof(char) * 15, "asa_out/%c.asa", i+65);
        aa[i] = fopen (buffer,"w");
        fclose (aa[i]);
    }
    aa[CHAINS]=fopen("asa_out/concatenate.asa", "w");
    fclose (aa[CHAINS]);

    while ((read[0] = getline(&line[0], &len, asa[0])) != -1) {			                        //Read one line at a time
    	tokens = strsplit(line[0], " \t\n", &token_count);
    	if(token_count==11){
    	    for(i=0; i<CHAINS; i++){
    	        snprintf(chain, sizeof(char) * 2, "%c", i+65);
    	        if(!strcmp(tokens[3],chain)){
        		    while ((read[i+1] = getline(&line[i+1], &len_res, asa[i+1])) != -1){
        			    tokens_res = strsplit(line[i+1], " \t\n", &token_count_res);
         		    	if(token_count_res==11){
        			        if(!strcmp(tokens[0],tokens_res[0])){
                                if(strcmp(tokens[5],tokens_res[5])){
                                    area[i] += atof(tokens_res[5])-atof(tokens[5]);
                                    snprintf(buffer, sizeof(char) * 15, "asa_out/%c.asa", i+65);
                                    aa[i] = fopen (buffer,"a");
                                    aa[CHAINS]=fopen("asa_out/concatenate.asa", "a");
                                    fprintf (aa[i], "%s\t\t%s\t\t%s\t\t%s\t\t%s\t\t%s\t\t%7.4f\n",	      //Write to file
                                        tokens_res[0], tokens_res[1], tokens_res[2], tokens_res[3],
                                        tokens_res[4], tokens_res[5], (atof(tokens_res[5])-atof(tokens[5])));
                                    fprintf (aa[CHAINS], "%s\t\t%s\t\t%s\t\t%s\t\t%s\t\t%s\t\t%7.4f\n",	      //Write to file
                                        tokens_res[0], tokens_res[1], tokens_res[2], tokens_res[3],
                                        tokens_res[4], tokens_res[5], (atof(tokens_res[5])-atof(tokens[5])));
                                    fclose (aa[i]);
                                    fclose (aa[CHAINS]);
                                }
                                break;
                            }
        			    }
        		    }
                    break;
                }
            }
    	}
        if (tokens != NULL)
            free(tokens);
    }
    printf("Interface Area:\n\tChain A : %7.4f\t\tChain B : %7.4f\n\tChain C : %7.4f\t\tChain D : %7.4f\n\tChain E : %7.4f\t\tChain F : %7.4f\n\tChain G : %7.4f\t\tChain H : %7.4f\n",
            area[0], area[1], area[2], area[3], area[4], area[5], area[6], area[7]);
    if (line[0] != NULL) free(line[0]);
    fclose(asa[0]);

    return EXIT_SUCCESS;
}
