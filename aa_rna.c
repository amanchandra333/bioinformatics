//compile with -std=gnu11
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define token_count 12					//Number of Columns

char **strsplit(const char* pdb_line, const char* delim) {
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
    free(s);
    return tokens;
}
int main(void) {
    FILE * pdb;
    FILE * pdb2;
    FILE * aa;
    FILE * rna;
    char *line = NULL;
    char *line2 = NULL;
    size_t len, len2;
    ssize_t read, read2;
    char **tokens1 = calloc(20, sizeof(char*));
    char **tokens2 = calloc(20, sizeof(char*));
    double dist;

    pdb = fopen("1asy.pdb", "r");								//Check if file is valid
    if (pdb == NULL)
        exit(EXIT_FAILURE);
    pdb2 = fopen("1asy.pdb", "r");                                                               //Check if file is valid
    if (pdb2 == NULL)
        exit(EXIT_FAILURE);

    aa = fopen ("aa.pdb","w");
    fclose (aa);
    rna = fopen ("rna.pdb","w");
    fclose (rna);

    while ((read = getline(&line, &len, pdb)) != -1) {						//Read one line at a time
	tokens1 = strsplit(line, " \t\n");
	if(!strcmp(tokens1[0],"ATOM")){
	    if(!strcmp(tokens1[4],"A")){
		while ((read2 = getline(&line2, &len2, pdb2)) != -1) {
		    tokens2 = strsplit(line2, " \t\n");
		    //printf("hre3\n");
		    if(!strcmp(tokens2[0],"ATOM")){
			if(!strcmp(tokens2[4],"R")){
			    //printf("calculating distance\n");
			    dist = sqrt(pow(atof(tokens1[6])-atof(tokens2[6]),2) + 
				pow(atof(tokens1[7])-atof(tokens2[7]),2) + pow(atof(tokens1[8])-atof(tokens2[8]),2));
			    if(dist <= 4.5){
				aa = fopen ("aa.pdb","a");
				fprintf (aa, "%s  %5s  %-3s %3s %s%4s    %8s%8s%8s  %4s %5s           %s\n",    //Write to file
                        		tokens1[0], tokens1[1], tokens1[2], tokens1[3], tokens1[4], tokens1[5], tokens1[6],
                        		tokens1[7], tokens1[8], tokens1[9], tokens1[10], tokens1[11]);
                		fclose (aa);

				rna = fopen ("rna.pdb","a");
                		fprintf (rna, "%s  %5s  %-3s %3s %s%4s    %8s%8s%8s  %4s %5s           %s\n",    //Write to file
                        		tokens2[0], tokens2[1], tokens2[2], tokens2[3], tokens2[4], tokens2[5], tokens2[6],
                        		tokens2[7], tokens2[8], tokens2[9], tokens2[10], tokens2[11]);
                		fclose (rna);
			    }
			}
		    }
		    if (tokens2 != NULL)
                    	free(tokens2);
		}
		if (line2 != NULL) free(line2);
	    }
        }
        if (tokens1 != NULL)
            free(tokens1);
    }
    if (line != NULL) free(line);
    return EXIT_SUCCESS;
}
