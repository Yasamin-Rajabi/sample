#include<stdio.h>

#include<stdlib.h>

#include<string.h>

#define maxn 100000



char input[maxn];

char word[maxn];



int main(){

	FILE *dic = fopen("dic.txt", "r");



	int find_mistake = 0;



	fgets(input, maxn, stdin);



	char *space  = " ";

	char *token = strtok(input, space);

	//scanf("%s", input);

	while(token != NULL){

		//*(input + strlen(input)) = '\0';

		int exist = 0;



		fscanf(dic, "%s", word);

		while(!feof(dic) && !exist){

			if(!strcmp(word, token))

				exist = 1;

			fscanf(dic, "%s", word);

		}

		rewind(dic);



		if(!exist){

			if(!find_mistake){

				find_mistake = 1;

				printf("misspelled word(s):\n");

			}

			//printf("%s\n", input);

			printf("%s\n", token);

		}



		//scanf("%s", input);

		token = strtok(NULL, space);

	}



	if(!find_mistake)

		printf("there is no misspelled word!");



	fclose(dic);



}