//compile with -std=gnu11
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct pose {
   float bfactor;
   int res;
   char* atom_name;
   char* res_name;
};

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
    FILE * temp;
    char *line = NULL;
    size_t len;
    ssize_t read;
    char **tokens;
    int token_count;

    pdb = fopen("1oj6.pdb", "r");								//Check if file is valid
    if (pdb == NULL)
        exit(EXIT_FAILURE);

    temp = fopen ("bfactor.pdb","w");
    fclose (temp);

    size_t allocated_a = 1, used_a =0;
    size_t allocated_b = 1, used_b =0;
    size_t allocated_c = 1, used_c =0;
    size_t allocated_d = 1, used_d =0;
    struct pose *aa = calloc(allocated_a, sizeof(struct pose));
    struct pose *ab = calloc(allocated_b, sizeof(struct pose));
    struct pose *ac = calloc(allocated_c, sizeof(struct pose));
    struct pose *ad = calloc(allocated_d, sizeof(struct pose));

    while ((read = getline(&line, &len, pdb)) != -1) {						            //Read one line at a time
    	tokens = strsplit(line, " \t\n", &token_count);
    	if(!strcmp(tokens[0],"ATOM")){
	    if(!strcmp(tokens[4],"A")){
                if (used_a == allocated_a) {			                                            //Increase allocated space if needed
                    allocated_a *= 2;
                    aa= realloc(aa, allocated_a * sizeof(struct pose));
                }
                aa[used_a].bfactor= atof(tokens[10]);
	        aa[used_a].res= atof(tokens[5]);
	        aa[used_a].atom_name= strdup(tokens[2]);
                aa[used_a++].res_name= strdup(tokens[3]);
	    }
	    if(!strcmp(tokens[4],"B")){
                if (used_b == allocated_b) {                                                                //Increase allocated space if needed
                    allocated_b *= 2;
                    ab= realloc(ab, allocated_b * sizeof(struct pose));
                }
                ab[used_b].bfactor= atof(tokens[10]);
                ab[used_b].res= atof(tokens[5]);
                ab[used_b].atom_name= strdup(tokens[2]);
                ab[used_b++].res_name= strdup(tokens[3]);
            }
	    if(!strcmp(tokens[4],"C")){
                if (used_c == allocated_c) {                                                                //Increase allocated space if needed
                    allocated_c *= 2;
                    ac= realloc(ac, allocated_c * sizeof(struct pose));
                }
                ac[used_c].bfactor= atof(tokens[10]);
                ac[used_c].res= atof(tokens[5]);
                ac[used_c].atom_name= strdup(tokens[2]);
                ac[used_c++].res_name= strdup(tokens[3]);
            }
	    if(!strcmp(tokens[4],"D")){
                if (used_d == allocated_d) {                                                                //Increase allocated space if needed
                    allocated_d *= 2;
                    ad= realloc(ad, allocated_d * sizeof(struct pose));
                }
                ad[used_d].bfactor= atof(tokens[10]);
                ad[used_d].res= atof(tokens[5]);
                ad[used_d].atom_name= strdup(tokens[2]);
                ad[used_d++].res_name= strdup(tokens[3]);
            }
        }
        if (tokens != NULL)
            free(tokens);
    }
    aa= realloc(aa, used_a * sizeof(struct pose));
    ab= realloc(ab, used_b * sizeof(struct pose));
    ac= realloc(ac, used_c * sizeof(struct pose));
    ad= realloc(ad, used_d * sizeof(struct pose));
    if (line != NULL) free(line);
    fclose(pdb);

    int i;
    double main=0, side=0;
    int main_count=0, side_count=0;
    int prev_res=3, res_count=0;
    char * max_res;
    double max_bfactor=0, res_bfactor;
    for(i=0; i<used_a; i++){
        if(!strcmp(aa[i].atom_name,"N") || !strcmp(aa[i].atom_name,"CA") ||
		!strcmp(aa[i].atom_name,"C") || !strcmp(aa[i].atom_name,"O")){
	    main_count++;
	    main+=aa[i].bfactor;
        }
	else{
	    side_count++;
	    side+=aa[i].bfactor;
	    if(aa[i].res==prev_res){
		res_bfactor+=aa[i].bfactor;
		res_count++;
	    }
	    else{
		if(max_bfactor<res_bfactor/res_count){
		    max_bfactor=res_bfactor/res_count;
		    max_res = strdup(aa[i].res_name);
		}
		res_bfactor=0;
		res_count=0;
	    }
	    prev_res=aa[i].res;
	}
    }
    printf("A Chain\t\tMain Chain : %f\t\tSide Chain : %f\t\tMost : %s, %f\t\tLeast : \n", main/main_count, side/side_count, max_res, max_bfactor);
    main=side=0;
    main_count=side_count=0;

    for(i=0; i<used_b; i++){
        if(!strcmp(ab[i].atom_name,"N") || !strcmp(ab[i].atom_name,"CA") ||
                !strcmp(ab[i].atom_name,"C") || !strcmp(ab[i].atom_name,"O")){
            main_count++;
            main+=ab[i].bfactor;
        }
	else{ 
            side_count++;
            side+=ab[i].bfactor;
        }
    }
    printf("A Chain\t\tMain Chain : %f\t\tSide Chain : %f\t\tMost : %s, %f\t\tLeast : \n", main/main_count, side/side_count, max_res, max_bfactor);
    main=side=0;
    main_count=side_count=0;

    for(i=0; i<used_c; i++){
        if(!strcmp(ac[i].atom_name,"N") || !strcmp(ac[i].atom_name,"CA") ||
                !strcmp(ac[i].atom_name,"C") || !strcmp(ac[i].atom_name,"O")){
            main_count++;
            main+=ac[i].bfactor;
        }
	else{ 
            side_count++;
            side+=ac[i].bfactor;
        }
    }
    printf("A Chain\t\tMain Chain : %f\t\tSide Chain : %f\t\tMost : %s, %f\t\tLeast : \n", main/main_count, side/side_count, max_res, max_bfactor);
    main=side=0;
    main_count=side_count=0;

    for(i=0; i<used_d; i++){
        if(!strcmp(ad[i].atom_name,"N") || !strcmp(ad[i].atom_name,"CA") ||
                !strcmp(ad[i].atom_name,"C") || !strcmp(ad[i].atom_name,"O")){
            main_count++;
            main+=ad[i].bfactor;
        }
	else{ 
            side_count++;
            side+=ad[i].bfactor;
        }
    }
    printf("A Chain\t\tMain Chain : %f\t\tSide Chain : %f\t\tMost : %s, %f\t\tLeast : \n", main/main_count, side/side_count, max_res, max_bfactor);
    main=side=0;
    main_count=side_count=0;

    return EXIT_SUCCESS;
}
