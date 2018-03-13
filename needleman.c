//compile with -std=gnu11
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MATCH 1
#define MISMATCH 1
#define GAP 2

int max(int a, int b, int c, int **arrow, int i, int j){
	int k=0, val=0;
	if(a>=b) k=a;
	else k=b;
	if(c>=k) k=c;

	if(k==a) val+=2;
	if(k==b) val+=8;
	if(k==c) val+=4;
	arrow[i][j] = val;

	return k;
}

void find_seq(char* X, char* Y, int m, int n){
	int i,j;
	int **grid = calloc((n+1), sizeof(int*));
	int **arrow = calloc((n+1), sizeof(int*));
	for(i=0; i<m+1; i++){
		grid[i] = calloc((m+1), sizeof(int));
		arrow[i] = calloc((m+1), sizeof(int));
	}

	for(i=0; i<m+1; i++){
		for(j=0; j<n+1; j++){
			if(i==0 || j==0){
				if(i==0 && j==0){
					grid[i][j] = 0;
					arrow[i][j] = 0;
				}
				else if(i==0){
					grid[i][j] = grid[i][j-1] - GAP;
					arrow[i][j] = 8;
				}
				else{
					grid[i][j] = grid[i-1][j] - GAP;
					arrow[i][j] = 2;
				}
			}

			else if(X[i-1] == Y[j-1]){
				grid[i][j] = grid[i-1][j-1] + MATCH;
				arrow[i][j] = 4;
			}

			else
				grid[i][j] = max(grid[i-1][j] - GAP, grid[i][j-1] - GAP, grid[i-1][j-1] - MISMATCH, arrow, i, j);
		}
	}

	for(i=0; i<m+1; i++){
		for(j=0; j<n+1; j++){
			printf("%d\t", grid[i][j]);
		}
		printf("\n");
	}

	for(i=0; i<m+1; i++){
		for(j=0; j<n+1; j++){
			printf("%d\t", arrow[i][j]);
		}
		printf("\n");
	}

	int k;
	if(m>n) k=m;
	else k=n;
	char buffer1[k+1];
	char buffer2[k+1];
	buffer1[k+1] = '\0';
	buffer2[k+1] = '\0';
	i=m; j=n;
	int t=0,p=k-1;
	while(i && j){
		if(arrow[i][j] ==4){
			i-=1; j-=1; t=0;
		}
		else if(arrow[i][j] ==2){
			i-=1; t=1;
		}
		else{
			j-=1; t=2;
		}

		if(t==0){
			printf("%c%c  \n", X[i], Y[j]);
			buffer1[p] = X[i];
			buffer2[p] = Y[j];
			p--;
		}
		else if(t==1){
			printf(" %c \n", X[i]);
			buffer1[p] = X[i];
			buffer2[p] = ' ';
			p--;
		}
		else{
			printf("%c  \n", Y[j]);
			buffer1[p] = ' ';
			buffer2[p] = Y[j];
			p--;
		}
	}
	printf("%s\n", buffer1);
	printf("%s\n", buffer2);
}

int main(int argc, char** argv) {
	if(argc!=3){
		printf("%s\n", "Please enter two sequences as arguments");
		exit(EXIT_FAILURE);
	}

	char *seq1, *seq2;
	int m,n;
	seq1 = strdup(argv[1]);  seq2 = strdup(argv[2]);
	m = strlen(seq1);  n=strlen(seq2);

	find_seq(seq1, seq2, m, n);
	return 0;
}