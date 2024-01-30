#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#define max_path 500


char primary_path[max_path];


long long Number(char*);

typedef struct out_fr{
     char *path;
     int er;
} out_fr;

void do_init(int, char*[]);
out_fr find_repository(char*);

void do_add(int, char*[]);
void do_add_stage(char*, char*);
void Dfs(char*, int, int, char*);
void dfs(char*, int, int, char*);
bool is_stage(char*, char*);


int main(int argc, char *argv[]){
	if(argc < 2)
		printf("error: invalid command!\n");   
	if (getcwd(primary_path, sizeof(primary_path)) == NULL)
         return 0;

	if(!strcmp("init", argv[1]))
		do_init(argc, argv);
	if(!strcmp("add", argv[1]))
		do_add(argc, argv);
	



}

long long Number(char *command){
    long long number = 0;
    for(int i = 0;i < strlen(command);i++){
        if(command[i] < '0' || '9' < command[i]){
            return -1;
        }else
            number *= 10LL, number += (int)(command[i] - '0');
    }
    return number;
}

out_fr find_repository(char *path){
	// deafult output
	out_fr output;
     output.path = NULL, output.er = 0; // NULL, error nadarim

	// go to the given path
	if(chdir(path) != 0){
		output.er = 1;
		return output;
	}
	char cur_path[max_path];
	if (getcwd(cur_path, sizeof(cur_path)) == NULL){
     	output.er = 1;
          return output;
     }
	
	// bool
    	bool exist = false;
    	bool seen_root = false;
	
    	while(strcmp(cur_path, "/") || !seen_root){
		// seen root
		if(!strcmp(cur_path, "/"))
          	seen_root = true;

		// read directory
        	DIR *dir = opendir(".");
        	if (dir == NULL) {
			output.er = 1;
			break;
        	}
		struct dirent *entry;
        	while ((entry = readdir(dir)) != NULL)
            	if (entry -> d_type == DT_DIR && !strcmp(entry -> d_name, ".fp"))
                	exist = true;
        	closedir(dir);

		// check exist
		if(exist){
			output.path = cur_path;
			break;
		}
		
		// go to the parent
		if (chdir("..") != 0){
			output.er = 1;
          	break;
		}
		if (getcwd(cur_path, sizeof(cur_path)) == NULL){
               output.er = 1;
               break;
          }
	}

	chdir(primary_path);
	return output;
}

void do_init(int argc, char *argv[]) {
    out_fr get_repository = find_repository(primary_path);
    if (get_repository.er){
	    printf("error: get repository\n");
	    return;
    }

    if(get_repository.path == NULL){
	     if(mkdir(".fp", 0755) != 0)
			return;
	    	chdir(".fp");
		if(mkdir("stage", 0755) != 0 || mkdir("commits", 0755) != 0)
			return;

		FILE *inf_file = fopen("inf.txt", "w");
		fclose(inf_file);

    }else
        printf("error: .fp repository has already initialized!\n");
    
}

void do_add(int argc, char *argv[]){
	if(argc < 3)
          return;

	// add with path
	if(((*argv[2]) != '-') && (strstr(argv[2], "*") == NULL) && (argc == 3)){
		//check repository
		out_fr get_repository = find_repository(argv[2]);
          if (get_repository.path == NULL || get_repository.er){
          	printf("error: you do not have a repository!\n");
               return;
          }

		chdir("/");
		DIR *input_dir = opendir(argv[2]);
		chdir(primary_path);
		FILE *input_file = fopen(argv[2], "r");

		//check directory
		if(input_dir != NULL){
			closedir(input_dir);
			Dfs(argv[2], 0, 0, get_repository.path);

		// check file
		}else if(input_file != NULL){
			fclose(input_file);
			do_add_stage(argv[2], get_repository.path);

		// not exist
		}else
			printf("error: there is no file or directory with path:%s!\n", argv[2]);
		
	}else{
		//check repository
          out_fr get_repository = find_repository(primary_path);
          if (get_repository.path == NULL || get_repository.er){
               printf("error: you do not have a repository!\n");
               return;
          }

		// add with wildcard
		if(((*argv[2]) != '-') && argc == 3){
		
		}

		// add with -f
		if(!strcmp(argv[2], "-f") && argc > 3){
			FILE *tmp_file;
			DIR *tmp_dir;
			char *entry_path = (char*) malloc(max_path * sizeof(char));
			
			for(int i = 3;i < argc;i++){

				// make path
				strcpy(entry_path, primary_path);
				strcat(entry_path, "/");
				strcat(entry_path, argv[i]);

				// check directory
				if((tmp_dir = opendir(argv[i])) != NULL){
					closedir(tmp_dir);
					dfs(entry_path, 0, 0, get_repository.path);
					printf("directory:%s added successfully\n", argv[i]);

				// check file
				}else if((tmp_file = fopen(argv[i], "r")) != NULL){
					fclose(tmp_file);
					do_add_stage(entry_path, get_repository.path);
					printf("file:%s added successfully\n", argv[i]);

				// not exist
				}else{
					printf("error: there is no file or directory with name:%s!\n", argv[i]);
				}
			}

		}

		// add with depth
		if(!strcmp(argv[2], "-n") && argc == 4){
			int nu = Number(argv[3]);
			Dfs(primary_path, nu, 1, get_repository.path);
		}
	}
}

void Dfs(char *path, int deep, int case_dfs, char *repository_path){
	dfs(path, deep, case_dfs, repository_path);
	chdir(primary_path);
}

// opendir
void dfs(char *path, int deep, int case_dfs, char *repository_path){
	if(!deep && case_dfs == 1)
		return;

	chdir(path);
	DIR *dir = opendir(".");

	struct dirent* entry;
	while((entry = readdir(dir)) != NULL){
		if(strcmp(entry -> d_name, ".") && strcmp(entry -> d_name, "..") && strcmp(entry -> d_name, ".fp") && entry -> d_type == DT_REG){
			char *file_path = (char*) malloc(max_path * sizeof(char));
			strcpy(file_path, path);
			strcat(file_path, entry -> d_name);

			if(case_dfs == 0){
				do_add_stage(file_path, repository_path);
			}
			if(case_dfs == 1){
				printf("file name: %s\n file path: %s\n stage: %d\n", entry -> d_name, file_path, is_stage(file_path, repository_path));
			}

			free(file_path);
		}
		if(strcmp(entry -> d_name, ".") && strcmp(entry -> d_name, "..") && strcmp(entry -> d_name, ".fp") && entry -> d_type == DT_DIR){
			char *dir_path = (char*) malloc(max_path * sizeof(char));
               strcpy(dir_path, path);
               strcat(dir_path, entry -> d_name);
               dfs(dir_path, deep-1, case_dfs, repository_path);
               free(dir_path);
		}
	}
	closedir(dir);
}

void do_add_stage(char *file_path, char *repository_path){
	strcat(repository_path, "/.fp/stage.txt");
	FILE *stage_file = fopen(repository_path, "a");
	fputs(file_path, stage_file);
	fclose(stage_file);
}

bool is_stage(char *file_path, char *repository_path){
    bool is_stage = false;


    strcat(repository_path, "/.fp/stage.txt");
    FILE *stage_file = fopen(repository_path, "r");

    char *in = (char*) malloc(max_path * sizeof(char));

    in = fgets(in, max_path, stage_file);
    while(!feof(stage_file)){
        if(!strcmp(file_path, in)){
            is_stage = true;
            break;
        }
        in = fgets(in, max_path, stage_file);
    }
    fclose(stage_file);

    return is_stage;
}



/*int main(int argc, char *argv[]){
	printf("%d\n", argc);
	for(int i = 0;i < argc;i++){
		printf("%s\n", argv[i]);
	}

	DIR *this_directory = opendir(".");
	struct dirent* entry;

	while((entry = readdir(this_directory)) != NULL){
		printf("%s ", entry -> d_name);
		printf("%d\n", entry -> d_type);
	}

	char adress[200];
	getcwd(adress, sizeof(adress));
	printf("%s\n", adress);

	while(strcmp(adress , "/")){
		if(!chdir("..")){
			getcwd(adress, sizeof(adress));
			printf("%s\n", adress);
		}else{
			printf("error :)");
		}

		DIR *this_directory = opendir(".");
		struct dirent* entry;

     	while((entry = readdir(this_directory)) != NULL){
          	printf("%s ", entry -> d_name);
          	printf("%d\n", entry -> d_type);
     	}
		printf("\n");
	}
	
//	int s = mkdir("created_file", 0775);
//	printf("%i", s);
//	if(s == -1){
//		printf("eror\n");
//		exit(0);
//	}
	FILE *created = fopen("created_file", "w");
	fclose(created);
}*/

/*
char* parent(char *path){
     char *parent_path = (char*) malloc(max_path * sizeof(char));
     strcpy(parent_path, path);

     if(opendir(parent_path) != NULL)
          return parent_path;

     int len = strlen(parent_path);
     while(len && *(parent_path + len - 1) != '/')
          *(parent_path + len - 1) = '\0', len--;

     return parent_path;
}*/

