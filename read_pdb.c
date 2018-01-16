//compile with -std=gnu11
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define token_count 12

char **strsplit(const char* pdb_line, const char* delim) {
    char *s = strdup(pdb_line);      //create a copy of the string

    size_t allocated = 1, used =0;
    char **tokens = calloc(allocated, sizeof(char*));
    char *token, *strtok_ctx;
    for (token = strtok_r(s, delim, &strtok_ctx);
            token != NULL;
            token = strtok_r(NULL, delim, &strtok_ctx)) {
        if (used == allocated) {
            allocated += 10;
            tokens = realloc(tokens, allocated * sizeof(char*));
        }
        tokens[used++] = strdup(token);
    }
    // free memory
    if (used == 0) {
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
    FILE * temp;
    char *line = NULL;
    size_t len;
    ssize_t read;
    char **tokens;
    int atoms=0;
    double *centre = calloc(3, sizeof(float));

    pdb = fopen("3fe0.pdb", "r");
    if (pdb == NULL)
        exit(EXIT_FAILURE);

    temp = fopen ("a.pdb","w");
    fclose (temp);

    while ((read = getline(&line, &len, pdb)) != -1) {
        tokens = strsplit(line, ", \t\n");
        if(strcmp(tokens[0],"ATOM") ==0){
            temp = fopen ("a.pdb","a");
            fprintf (temp, "%s  %5s  %-3s %3s %s %3s    %8s %7s %7s  %4s %5s           %s\n",
                    tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], tokens[5], tokens[6],
                    tokens[7], tokens[8], tokens[9], tokens[10], tokens[11]);
            fclose (temp);

            centre[0] += atof(tokens[6]);
            centre[1] += atof(tokens[7]);
            centre[2] += atof(tokens[8]);
            atoms++;
        }
        if (tokens != NULL)
            free(tokens);
    }
    temp = fopen ("a.pdb","a");
    fprintf (temp, "%s  %5s  %-3s %3s %s %3s    %8.3f %7.3f %7.3f  %4s %5s           %s\n",
                    "ATOM", "00000", "XXX", "XXX", "A", "000", centre[0]/atoms,
                    centre[1]/atoms, centre[2]/atoms , "0.00", "00.00", "X");
    fclose (temp);
    
    printf("%.3f\t%.3f\t%.3f\n",centre[0]/atoms, centre[1]/atoms, centre[2]/atoms );

    if (line != NULL) free(line);
    return EXIT_SUCCESS;
}
