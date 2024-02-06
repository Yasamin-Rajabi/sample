#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define maxn 500
#define max_path 500
#define max_time 100
#define max_massage 100

char Cur_path[max_path];
char Tmp_path[max_path];

typedef struct global_config_data{
	time_t name_date;
	time_t email_date;
	char name[maxn];
	char email[maxn];

} global_config_data;

void do_config(int, char*[]);
global_config_data take_global_config();

int take_cur_path();
int selected_paht();
long long Number(char*);

typedef struct out_fr{
     char path[max_path];
     int er;
	char name[maxn];
	char email[maxn];

} out_fr;

void do_init(int, char*[]);
out_fr find_repository(char*);

void do_add(int, char*[]);
void do_add_stage(char*, char*);
void Dfs(char*, int, int, char*);
void dfs(int, int, char*);
void push_star(char*);
bool is_stage(char*, char*);

void do_reset(int, char*[]);
void do_remove_stage_file(char*, char*);
void do_undo(char*);
void Dfs_r(char*, char*);
void dfs_r(char*);

void do_status();
void dfs_s(char*, char*, int);
int do_check_with_last_commit(char*, time_t, char*);

void do_check_massage(int, char*[]);
void do_commit(char*);

time_t to_time(char*);
int do_log_is_repository();
void do_log_n(int);
void do_log_branch(char*);
void do_log_author(char*);
void do_log_since(int, char*[]);
void do_log_before(int, char*[]);
void do_log_search(char*);
void do_log_show_commit(int);

void do_branch(int, char *argv[]);
void show_branch();

void do_checkout_HEAD();
void do_checkout(int, char*[]);

void do_rep_commit(char*, char*, char*, int, char*, int);

void do_show_tag_all();
void do_tag(int, char*[]);
void do_show_tag(int, char*[]);

void do_revert_n(int, char*[]);
void do_revert_go_back(int, char*[]);
void do_revert_check_merge(int, char*[]);

int main(int argc, char *argv[]){
	if(argc < 2){
		printf("error: invalid command!\n");   

	}else if(!strcmp("config", argv[1])){
		do_config(argc, argv);

	}else if(!strcmp("init", argv[1])){
		do_init(argc, argv);

	}else if(!strcmp("add", argv[1])){
		do_add(argc, argv);

	}else if(!strcmp("reset", argv[1])){
		do_reset(argc, argv);

	}else if(!strcmp("status", argv[1])){
		do_status();

	}else if(argc > 2 && !strcmp("commit", argv[1]) && !strcmp("-m", argv[2])){
		if(argc == 3){
			printf("error: massage in empty!\n");
		}else if(argc == 4)
			do_commit(argv[3]);
		else{
			printf("error: the massage must be in "" when you are using white space!\n");
		}

	}else if(!strcmp("log", argv[1])){
		if(argc == 2){
			if(do_log_is_repository()){
				FILE *cnt_commits = fopen("cnt.txt", "r");
    				int cnt;
    				fscanf(cnt_commits, "%d", &cnt);

    				for(int j = cnt;1 <= j;j--)
        				do_log_show_commit(j);
			}

		}else if(!strcmp("-n", argv[2]) && argc == 4)
	          do_log_n(Number(argv[3]));
	     else if(!strcmp("-branch", argv[2]) && argc == 4)
			do_log_branch(argv[3]);
	     else if(!strcmp("-author", argv[2]) && argc == 4)
	          do_log_author(argv[3]); // :)
	     else if(!strcmp("-since", argv[2]) && argc == 5)
	          do_log_since(argc, argv);
	     else if(!strcmp("-before", argv[2]) && argc == 5)	          
			do_log_before(argc, argv);
	     else if(!strcmp("-search", argv[2]) && argc == 4)
	          do_log_search(argv[3]);
		else{
	     	printf("invalid command!\n");
		}

    	}else if(!strcmp("branch", argv[1])){
		if(argc == 2)
			show_branch();
		else
			do_branch(argc, argv);

	}else if(!strcmp("checkout", argv[1])){
		if(argc == 3 && !strcmp(argv[2], "HEAD"))
			do_checkout_HEAD();
		else
			do_checkout(argc, argv);

	}else if(!strcmp("revert", argv[1])){
		if(argc > 2 && !strcmp("-n", argv[2]))
			do_revert_n(argc, argv);
		else if(*(argv[argc-1]) == 'H')
			do_revert_go_back(argc, argv);
		else 
			do_revert_check_merge(argc, argv);
	
	}else if(!strcmp("tag", argv[1])){
		if(argc == 2)
			do_show_tag_all();
		else if(!strcmp(argv[2], "show"))
			do_show_tag(argc, argv);
		else if(!strcmp(argv[2], "-a"))
			do_tag(argc, argv);
		else
			printf("error: invalid command\n");

	}else{
		printf("error: invalid command\n");
	}
}


char* from_int_to_char(int x){
    char *res = (char*) malloc(maxn * sizeof(char));
    sprintf(res, "%d", x);
    //printf("%s", res);
    return res;
}


void copy(char path1[], char path2[]){ // 2 ro briz to 1
    FILE *file_path_1 = fopen(path1, "w");
    FILE *file_path_2 = fopen(path2, "r");

    char in[maxn];
    while(fgets(in, maxn, file_path_2) != NULL)
        fprintf(file_path_1, "%s", in);

    fclose(file_path_1);
    fclose(file_path_2);
}


char* parent(char *path){
     char *parent_path = (char*) malloc(maxn * sizeof(char));
	strcpy(parent_path, path);

     int len = strlen(parent_path);
     while(len && *(parent_path + len - 1) != '/')
          *(parent_path + len - 1) = '\0', len--;
	*(parent_path + len - 1) = '\0';

     return parent_path;
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


int take_cur_path(){
    if(getcwd(Cur_path, sizeof(Cur_path)) == NULL)
        return 0;
    return 1;
}


int return_selected_path(){
    if(chdir(Cur_path) != 0)
        return 0;
    return 1;
}

global_config_data take_global_config(){
    chdir("/home/yasamin");
    FILE *name_file = fopen("user_name.txt", "r");

    global_config_data config;
    fgets(config.name, maxn, name_file);
    config.name[strlen(config.name)-1] = '\0';

    char nd[maxn];
    fgets(nd, maxn, name_file);
    config.name_date = to_time(nd);

    fclose(name_file);

    FILE *email_file = fopen("user_email.txt", "r");

    fgets(config.email, maxn, email_file);
    config.email[strlen(config.email)-1] = '\0';

    char ed[maxn];
    fgets(ed, maxn, email_file);
    config.email_date = to_time(ed);

    fclose(email_file);

    return config;
}

void do_config(int argc, char *argv[]){
    if(!strcmp("-global", argv[2])){
        chdir("/home/yasamin");

	   FILE *update_file;
        if(!strcmp("user.name", argv[3]))
            update_file = fopen("user_name.txt", "w");
        else
            update_file = fopen("user_email.txt", "w");

        fprintf(update_file, "%s\n", argv[4]);

        time_t now; time(&now);
        char cur_time[maxn];
        strftime(cur_time, maxn, "%Y/%m/%d %H:%M:%S", localtime(&now));
        fprintf(update_file, "%s\n", cur_time);

        fclose(update_file);

    }else{
        take_cur_path();
    	   out_fr get_repository = find_repository(Cur_path);
    	   if (get_repository.path == NULL || get_repository.er){
            printf("error: you do not have a repository!\n");
            return;
    	   }
        chdir(get_repository.path), chdir(".fp");

        FILE *update_file;
    	   if(!strcmp("user.name", argv[2]))
            update_file = fopen("user_name.txt", "w");
        else
            update_file = fopen("user_email.txt", "w");

        fprintf(update_file, "%s\n", argv[3]);

        time_t now; time(&now);
        char cur_time[maxn];
        strftime(cur_time, maxn, "%Y/%m/%d %H:%M:%S", localtime(&now));
        fprintf(update_file, "%s\n", cur_time);

        fclose(update_file);
    }
}

out_fr find_repository(char *path){
	take_cur_path();
	
	// deafult output
	out_fr output;
     strcpy(output.path, ""), output.er = 0; // NULL, error nadarim

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
			strcpy(output.path , cur_path);
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


	if(output.er != 1 && output.path != ""){
		global_config_data config = take_global_config();
		strcpy(output.name , config.name);
		strcpy(output.email , config.email);

		char in[maxn];

		chdir(output.path), chdir(".fp");
		FILE *name_file = fopen("user_name.txt", "r");
		if(fgets(in, maxn, name_file) != NULL){
			in[strlen(in)-1] = '\0';
			char date[maxn];
			fgets(date, maxn, name_file);
			if(difftime(to_time(date), config.name_date) > 0)
				strcpy(output.name , in);
		}
		fclose(name_file);

          FILE *email_file = fopen("user_email.txt", "r");
          if(fgets(in, maxn, email_file) != NULL){
               in[strlen(in)-1] = '\0';
               char date[maxn];
               fgets(date, maxn, email_file);
               if(difftime(to_time(date), config.email_date) > 0)
                    strcpy(output.email , in);
          }
          fclose(email_file);

	}
	return_selected_path();
	return output;
}


void make_repository_files(){
	if(mkdir("stage", 0755) != 0 || mkdir("commits", 0755) != 0 || mkdir("branch", 0755) != 0 || mkdir("tag", 0755) != 0)
     	return;

     // inf files
     FILE * make_file = fopen("last_commit_id.txt", "w");
	fprintf(make_file, "%d\n", 0);
     fclose(make_file);

     make_file = fopen("last_branch_id.txt", "w");  
	fprintf(make_file, "%d\n", 1);
	fprintf(make_file, "%s\n", "master");
	fclose(make_file);

	make_file = fopen("permission_commit.txt", "w");
     fprintf(make_file, "%d\n", 1);
     fclose(make_file);

	// files relative to tag
	chdir("tag");
	make_file = fopen("tag.txt", "w");
     fclose(make_file);

	// files relative to stage
	chdir(".."), chdir("stage");

	make_file = fopen("inf.txt", "w");
	fprintf(make_file, "%d", 0);
	fclose(make_file);

	make_file = fopen("stage.txt", "w");
	fclose(make_file);

	make_file = fopen("general_stage.txt", "w");
     fclose(make_file);

	make_file = fopen("history_stage.txt", "w");
	fclose(make_file);

	// files relative to commit
	chdir(".."), chdir("commits");
     
	make_file = fopen("cnt.txt", "w");
	fprintf(make_file, "%d", 0);
	fclose(make_file);
     
	make_file = fopen("massage.txt", "w");
	fclose(make_file);

	make_file = fopen("date.txt", "w");     
	fclose(make_file);
     
	make_file = fopen("author_name.txt", "w");     
	fclose(make_file);
     
	make_file = fopen("author_gmail.txt", "w");     
	fclose(make_file);
     
	make_file = fopen("branch.txt", "w");
	fclose(make_file);

	make_file = fopen("total_commited_files.txt", "w");     
	fclose(make_file);
     

	// files relative to branch
	chdir(".."), chdir("branch");
     
	make_file = fopen("cnt.txt", "w");
	fprintf(make_file, "%d\n", 1);
	fclose(make_file);

	make_file = fopen("name.txt", "w");     
	fprintf(make_file, "%s\n", "master");     
	fclose(make_file);

	make_file = fopen("last_commit.txt", "w");
	fprintf(make_file, "%d\n", 0);
	fclose(make_file);
}



void do_init(int argc, char *argv[]) {
	take_cur_path();
	out_fr get_repository = find_repository(Cur_path);
	if (get_repository.er){
		printf("error: get repository\n");
		return;
    	}

    	if(!strcmp(get_repository.path,"")){
		if(mkdir(".fp", 0755) != 0)
        		return;
		chdir(".fp");
		make_repository_files();
		printf("the repository was created successfully!\n");
    }else
        printf("error: .fp repository has already initialized!\n");
}

void do_add(int argc, char *argv[]){
	if(argc < 3)
          return;

	if(((*argv[2]) != '-') && (argc == 3)){
		take_cur_path();
		strcat(Cur_path, "/");
		strcat(Cur_path, argv[2]);
		strcpy(argv[2], Cur_path);
		take_cur_path();

		out_fr get_repository = find_repository(argv[2]);
          if ((!strcmp(get_repository.path , "") || get_repository.er)){
			get_repository = find_repository(parent(argv[2]));
			if ((!strcmp(get_repository.path , "") || get_repository.er)){
	          	printf("error: you do not have a repository!\n");
     	          return;
			}
          }
		
		take_cur_path();
		chdir("/");
		DIR *input_dir = opendir(argv[2]);
		return_selected_path();
		FILE *input_file = fopen(argv[2], "r");

		//check directory
		if(input_dir != NULL){
		    	push_star(get_repository.path);

			closedir(input_dir);
			Dfs(argv[2], 0, 0, get_repository.path);

		// check file
		}else if(input_file != NULL){
		     push_star(get_repository.path);

			fclose(input_file);
			do_add_stage(argv[2], get_repository.path);

		// not exist
		}else
			printf("error: there is no file or directory with path:%s!\n", argv[2]);
		
	}else{
		//check repository
		take_cur_path();
          out_fr get_repository = find_repository(Cur_path);
          if (!strcmp(get_repository.path , "") || get_repository.er){
               printf("error: you do not have a repository!\n");
               return;
          }

		// add with wildcard
		if((*argv[2]) != '-'){
      		push_star(get_repository.path);

			FILE *tmp_file;
               DIR *tmp_dir;
               char *entry_path = (char*) malloc(max_path * sizeof(char));

               take_cur_path();
               for(int i = 2;i < argc;i++){

                    // make path
                    strcpy(entry_path, Cur_path);
                    strcat(entry_path, "/");
                    strcat(entry_path, argv[i]);

                    // check directory
                    if((tmp_dir = opendir(argv[i])) != NULL){
                         closedir(tmp_dir);
                         Dfs(entry_path, 0, 0, get_repository.path);
                         printf("directory:%s added successfully\n", argv[i]);

                    // check file
                    }else if((tmp_file = fopen(argv[i], "r")) != NULL){
                         fclose(tmp_file);
                         do_add_stage(entry_path, get_repository.path);
                         //printf("file:%s added successfully\n", argv[i]);

                    // not exist
                    }else{
                         printf("error: there is no file or directory with name:%s!\n", argv[i]);
                    }
                }
		 }
		
		// add with -f
		if(!strcmp(argv[2], "-f") && argc > 3){
		     push_star(get_repository.path);

			FILE *tmp_file;
			DIR *tmp_dir;
			char *entry_path = (char*) malloc(max_path * sizeof(char));
			
			take_cur_path();
			for(int i = 3;i < argc;i++){

				// make path
				strcpy(entry_path, Cur_path);
				strcat(entry_path, "/");
				strcat(entry_path, argv[i]);

				// check directory
				if((tmp_dir = opendir(argv[i])) != NULL){
					closedir(tmp_dir);
					Dfs(entry_path, 0, 0, get_repository.path);
					printf("directory:%s added successfully\n", argv[i]);

				// check file
				}else if((tmp_file = fopen(argv[i], "r")) != NULL){
					fclose(tmp_file);
					do_add_stage(entry_path, get_repository.path);
					//printf("file:%s added successfully\n", argv[i]);

				// not exist
				}else{
					printf("error: there is no file or directory with name:%s!\n", argv[i]);
				}
			}
		}

		// add with depth
		if(!strcmp(argv[2], "-n") && argc == 4){
		     push_star(get_repository.path);

			int nu = Number(argv[3]);
			take_cur_path();
			Dfs(Cur_path, nu, 1, get_repository.path);
		}
	}
}

void Dfs(char path[], int deep, int case_dfs, char repository_path[]){
	char our_path[max_path];
	getcwd(our_path, sizeof(our_path));

	chdir(path);
	dfs(deep, case_dfs, repository_path);
	
	chdir(our_path);
}

void dfs(int deep, int case_dfs, char repository_path[]){
	if(!deep && case_dfs == 1){
		chdir("..");
		return;
	}

	DIR *dir = opendir(".");

	struct dirent* entry;
	while((entry = readdir(dir)) != NULL){
		if(strcmp(entry -> d_name, ".") && strcmp(entry -> d_name, "..") && strcmp(entry -> d_name, ".fp") && entry -> d_type == DT_REG){
			//char _path[max_path];// = (char*) malloc(max_path * sizeof(char));
			getcwd(Tmp_path, sizeof(Tmp_path));
			strcat(Tmp_path, "/");
			strcat(Tmp_path, entry -> d_name);

			if(case_dfs == 0){
				do_add_stage(Tmp_path, repository_path);
			}
			if(case_dfs == 1){
				printf("file name: %s\nfile path: %s\nstage: %d\n", entry -> d_name, Tmp_path, is_stage(Tmp_path, repository_path));
			}

		//	free(file_path);
		}
		if(strcmp(entry -> d_name, ".") && strcmp(entry -> d_name, "..") && strcmp(entry -> d_name, ".fp") && entry -> d_type == DT_DIR){
			chdir(entry -> d_name);
               dfs(deep-1, case_dfs, repository_path);
		}
	}

	closedir(dir);
	chdir("..");
}

void do_add_stage(char *file_path, char *repository_path){
	take_cur_path();

	chdir(repository_path), chdir(".fp"), chdir("stage");
	
	// open inf.txt and update cnt
	int cnt;
	FILE *inf_stage = fopen("inf.txt", "r"); fscanf(inf_stage, "%d", &cnt); fclose(inf_stage);
	cnt++ ,inf_stage = fopen("inf.txt", "w"); fprintf(inf_stage, "%d\n", cnt); fclose(inf_stage);

	//
	FILE *stage_file = fopen("stage.txt", "r");
	FILE *tmp_stage = fopen("tmp_stage.txt", "w");
	
	char in[max_path];

	char fp_compare[max_path];
    	strcpy(fp_compare, file_path);
    	strcat(fp_compare, "\n");

	while(fgets(in, max_path, stage_file) != NULL){
		if(strcmp(in, fp_compare)){
			fprintf(tmp_stage, "%s", in);
			fgets(in, max_path, stage_file);
			fprintf(tmp_stage, "%s", in);
			fgets(in, max_path, stage_file);
			fprintf(tmp_stage, "%s", in);
		}else{
			fgets(in, max_path, stage_file);
			fgets(in, max_path, stage_file);
		}
	}

	// update stage.txt
	fprintf(tmp_stage, "%s\n", file_path);
	
	struct stat info;
     stat(file_path, &info);
     time_t last_change = info.st_mtime;
	char char_last_change[max_time];
	strftime(char_last_change, max_time, "%Y/%m/%d %H:%M:%S", localtime(&last_change));
	fprintf(tmp_stage, "%s\n", char_last_change);

	int cnt_int = (int) cnt;
	fprintf(tmp_stage, "%d\n", cnt);

	// copy file
	char file_path1[maxn], file_path2[maxn];
	strcpy(file_path2, file_path);

	// we are in satage
	getcwd(file_path1, sizeof(file_path1));
	strcat(file_path1, "/");
	strcat(file_path1, from_int_to_char(cnt));
	strcat(file_path1, ".txt");

	copy(file_path1, file_path2);

	//
	fclose(tmp_stage);
	remove("stage.txt");
	rename("tmp_stage.txt", "stage.txt");

	// history_stage
	FILE *history_stage = fopen("history_stage.txt", "a");
	fprintf(history_stage, "%s\n", file_path);
	fclose(history_stage);

	return_selected_path();

	printf("file with path:%s added successfully!\n", file_path);
}

bool is_stage(char path[], char repository_path[]){
    take_cur_path();

    bool is_stage = false;

    chdir(repository_path);
    chdir(".fp"), chdir("stage");
    FILE *stage_file = fopen("stage.txt", "r");

    char *in = (char*) malloc(max_path * sizeof(char));
    in = fgets(in, max_path, stage_file);

    char fp_compare[max_path];
    strcpy(fp_compare, path);
    strcat(fp_compare, "\n");

    while(fgets(in, maxn, stage_file) != NULL){
	  if(!strcmp(fp_compare, in)){
      	  fgets(in, max_path, stage_file);
	  		
		  if(strlen(in) != 1){
		       struct stat info;
	       	  stat(path, &info);
     	  	  time_t last_change = info.st_mtime;
     	  	  char char_last_change[max_time];
     	  	  strftime(char_last_change, max_time, "%Y/%m/%d %H:%M:%S\n", localtime(&last_change));
			  if(!strcmp(char_last_change, in))
				 is_stage = 1;

		  }else{
			 is_stage = 1;
		  }

		  fgets(in, max_path, stage_file);
            break;

        }else{
        	  fgets(in, max_path, stage_file);
        	  fgets(in, max_path, stage_file);
	   }
    }
    fclose(stage_file);
    free(in);

    return_selected_path();

    return is_stage;
}

void do_reset(int argc, char *argv[]){
    if(argc < 3){
        printf("error: invalid command\n");
        return;
    }

    if(((*argv[2]) != '-') && (argc == 3)){
		take_cur_path();
          strcat(Cur_path, "/");
          strcat(Cur_path, argv[2]);
          strcpy(argv[2], Cur_path);
          take_cur_path();

          out_fr get_repository = find_repository(argv[2]);
          if ((!strcmp(get_repository.path , "") || get_repository.er)){
               get_repository = find_repository(parent(argv[2]));
               if ((!strcmp(get_repository.path , "") || get_repository.er)){
                    printf("error: you do not have a repository!\n");
                    return;
               }
          }

          take_cur_path();
          chdir("/");
          DIR *input_dir = opendir(argv[2]);
          return_selected_path();
          FILE *input_file = fopen(argv[2], "r");

          //check directory
          if(input_dir != NULL){
			chdir(argv[2]);
               closedir(input_dir);
			Dfs_r(argv[2], get_repository.path);

          // check file
          }else if(input_file != NULL){
               fclose(input_file);
			do_remove_stage_file(argv[2], get_repository.path);

          // not exist
          }else
               printf("error: there is no file or directory with path:%s!\n", argv[2]);


    }else{
		take_cur_path();
		out_fr get_repository = find_repository(Cur_path);
          if ((!strcmp(get_repository.path , "") || get_repository.er)){
			printf("error: you do not have a repository!\n");
			return;
		}

		if((*argv[2]) != '-'){
		     FILE *tmp_file;
               DIR *tmp_dir;
               char *entry_path = (char*) malloc(max_path * sizeof(char));

               take_cur_path();
               for(int i = 2;i < argc;i++){

                    // make path
                    strcpy(entry_path, Cur_path);
                    strcat(entry_path, "/");
                    strcat(entry_path, argv[i]);

                    // check directory
                    if((tmp_dir = opendir(argv[i])) != NULL){
                         closedir(tmp_dir);
                         Dfs_r(entry_path, get_repository.path);
                         printf("directory:%s removed successfully\n", argv[i]);

                    // check file
                    }else if((tmp_file = fopen(argv[i], "r")) != NULL){
                         fclose(tmp_file);
                         do_remove_stage_file(entry_path, get_repository.path);
                         //printf("file:%s added successfully\n", argv[i]);

                    // not exist
                    }else{
                         printf("error: there is no file or directory with name:%s!\n", argv[i]);
                    }
               }
               // wildcard

		// remove with -f
		}else if(!strcmp(argv[2], "-f") && argc > 3){
               FILE *tmp_file;
               DIR *tmp_dir;
               char *entry_path = (char*) malloc(max_path * sizeof(char));

               take_cur_path();
               for(int i = 3;i < argc;i++){

                    // make path
                    strcpy(entry_path, Cur_path);
                    strcat(entry_path, "/");
                    strcat(entry_path, argv[i]);

                    // check directory
                    if((tmp_dir = opendir(argv[i])) != NULL){
                         closedir(tmp_dir);
                         Dfs_r(entry_path, get_repository.path);
                         printf("directory:%s removed successfully\n", argv[i]);

                    // check file
                    }else if((tmp_file = fopen(argv[i], "r")) != NULL){
                         fclose(tmp_file);
                         do_remove_stage_file(entry_path, get_repository.path);
                         //printf("file:%s added successfully\n", argv[i]);

                    // not exist
                    }else{
                         printf("error: there is no file or directory with name:%s!\n", argv[i]);
                    }
               }

    		}else if(!strcmp(argv[2], "-undo") && argc == 3){
        		take_cur_path();

        		chdir(get_repository.path);
        		chdir(".fp");

        		do_undo(get_repository.path);

        		return_selected_path();
    		}else{
         		printf("invliad command!\n");
    		}

	}
}

void push_star(char repository_path[]){
    	take_cur_path();

	chdir(repository_path);
     chdir(".fp"), chdir("stage");
 
     FILE *history_stage = fopen("history_stage.txt", "a");
	fprintf(history_stage, "*\n");
	fclose(history_stage);
	
	return_selected_path();
}

void do_undo(char repository_path[]){
     take_cur_path();
	chdir(repository_path);
	chdir(".fp"), chdir("stage");

	FILE *history_stage = fopen("history_stage.txt", "r");
	char in[maxn];

	int i = 1, pnt = 1;
	while(fgets(in, maxn, history_stage) != NULL){
		if(!strcmp(in, "*\n"))
		    pnt = i;
		i++;
	}
	fclose(history_stage);

	FILE *tmp_h = fopen("tmp_history.txt", "w");
	history_stage = fopen("history_stage.txt", "r");

	i = 1;
	while(i != pnt){
	    fgets(in, maxn, history_stage);
	    fprintf(tmp_h, "%s", in);
	    i++;
	}
	
	if(fgets(in, maxn, history_stage) == NULL)
	    return;

	printf("%s", in);
	while(fgets(in, maxn, history_stage) != NULL){
		printf("%s", in);
	    in[strlen(in)-1] = '\0';
	    do_remove_stage_file(in, repository_path);
	}

	fclose(tmp_h);
	fclose(history_stage);

	remove("history_stage.txt");
	rename("tmp_history.txt", "history_stage.txt");

	return_selected_path();
}

void Dfs_r(char *dir_path, char repository_path[]){
	char our_path[max_path];
     getcwd(our_path, sizeof(our_path));

     chdir(dir_path);
     dfs_r(repository_path);

     chdir(our_path);
}

void dfs_r(char repository_path[]){
     DIR *dir = opendir(".");

     struct dirent* entry;
     while((entry = readdir(dir)) != NULL){
          if(strcmp(entry -> d_name, ".") && strcmp(entry -> d_name, "..") && strcmp(entry -> d_name, ".fp") && entry -> d_type == DT_REG){
               getcwd(Tmp_path, sizeof(Tmp_path));
               strcat(Tmp_path, "/");
               strcat(Tmp_path, entry -> d_name);

               do_remove_stage_file(Tmp_path, repository_path);
          }
          if(strcmp(entry -> d_name, ".") && strcmp(entry -> d_name, "..") && strcmp(entry -> d_name, ".fp") && entry -> d_type == DT_DIR){
               chdir(entry -> d_name);
               dfs_r(repository_path);
          }
     }

     closedir(dir);
	chdir("..");
}

void do_remove_stage_file(char file_path[], char repository_path[]) {
    take_cur_path();
    chdir(repository_path);
    chdir(".fp");
    chdir("stage");

    FILE *stage_file = fopen("stage.txt", "r");
    FILE *tmp_file = fopen("tmp_stage.txt", "w");

    char fp_compare[max_path];
    strcpy(fp_compare, file_path);
    strcat(fp_compare, "\n");

    char in[max_path];
    while (fgets(in, max_path, stage_file) != NULL) {
        if (strcmp(fp_compare, in)){
            fprintf(tmp_file, "%s", in);

            fgets(in, max_path, stage_file);
            fprintf(tmp_file, "%s", in);

            fgets(in, max_path, stage_file);
            fprintf(tmp_file, "%s", in);
        }else{
            fgets(in, max_path, stage_file);
            fgets(in, max_path, stage_file);
        }
    }


    remove("stage.txt");
    rename("tmp_stage.txt", "stage.txt");

    fclose(stage_file);
    fclose(tmp_file);

    return_selected_path();
    printf("file with path:%s removed successfully!\n", file_path);
    return;
}

int check_massage(int argc, char *argv[]){
    if(argc < 3){
        printf("error: invalid command!\n");
        return 0;
    }

    if(argc == 3){
        printf("error: set a commit masssage\n");
        return 0;
    }

    int len_massage = -1;
    for(int i = 3;i < argc;i++){
        len_massage += strlen(argv[i]) + 1;
    }

    // without space
    if((argc == 4 && (*(argv[3] + strlen(argv[3]-1)) != '"') && (*(argv[3]) != '"'))){
        len_massage += 2;

    // with space
    }else if(argc > 4 && (*argv[3]) == '"' && (*(argv[argc-1] + strlen(argv[argc-1]) -1) == '"')){
        len_massage = len_massage;

    // invalid
    }else{
        printf("error: follow the double coatation rule!\n");
        return 0;
    }

    if(len_massage > 74){
        printf("error: the message is too long!\n");
        return 0;
    }

    return 1;
}

void do_add_general_state(){
	FILE *stage_file = fopen("stage.txt", "r");

	char input_stage[maxn];
	while(fgets(input_stage, maxn, stage_file) != NULL){

//		printf("input: %s", input_stage);
		FILE *general_stage_file = fopen("general_stage.txt", "r");
		FILE *tmp_general = fopen("tmp_general.txt", "w");

//		printf("dddd\n");
		char in[maxn];
		while(fgets(in, maxn, general_stage_file) != NULL){
//			printf("KKKK\n");
			if(strcmp(in, input_stage)){
				fprintf(tmp_general, "%s", in);

				fgets(in, maxn, general_stage_file);
				fprintf(tmp_general, "%s", in);

				fgets(in, maxn, general_stage_file);
				fprintf(tmp_general, "%s", in);
			}else{

				fgets(in, maxn, general_stage_file);
				fgets(in, maxn, general_stage_file);
			}
		}

//		printf("bad while\n");
		fprintf(tmp_general, "%s", input_stage);

          fgets(input_stage, maxn, stage_file);
          fprintf(tmp_general, "%s", input_stage);

          fgets(input_stage, maxn, stage_file);
          fprintf(tmp_general, "%s", input_stage);

		fclose(general_stage_file);
		fclose(tmp_general);

//		printf("ghabl rename o remove\n");
		remove("general_stage.txt");
		rename("tmp_general.txt", "general_stage.txt");
	}

	fclose(stage_file);
}

void update_informations_about_commits(char repository_path[], char user_name[], char user_email[], char massage[], char last_branch_name[], int cnt_commited_files, int commit_id){
	chdir(repository_path);
     chdir(".fp"), chdir("commits");

	// update commit
	// cnt
	FILE *update_file = fopen("cnt.txt", "r");
	int cnt_commits;
    	fscanf(update_file, "%d", &cnt_commits);

    	fclose(update_file);
    	update_file = fopen("cnt.txt", "w");
    	cnt_commits++;
    	fprintf(update_file, "%d\n", cnt_commits);
    	fclose(update_file);

     // massage
     FILE *update_commit = fopen("massage.txt", "a");
     fprintf(update_commit, "%s\n", massage);
     fclose(update_commit);

     // date
     update_commit = fopen("date.txt", "a");
     char cur_time[max_time];
     time_t now; time(&now);
     strftime(cur_time, max_time, "%Y/%m/%d %H:%M:%S", localtime(&now));
     fprintf(update_commit, "%s\n", cur_time);
     fclose(update_commit);

     // branch
     update_commit = fopen("branch.txt", "a");
     fprintf(update_commit, "%s", last_branch_name);
     fclose(update_commit);

	// user name
	update_commit = fopen("author_name.txt", "a");
     fprintf(update_commit, "%s\n", user_name);
     fclose(update_commit);

	// user email
	update_commit = fopen("author_gmail.txt", "a");
     fprintf(update_commit, "%s\n", user_email);
     fclose(update_commit);


     // total commit files
     update_commit = fopen("total_commited_files.txt", "a");
     fprintf(update_commit, "%d\n", cnt_commited_files);
     fclose(update_commit);

     // clear stage
     chdir(".."), chdir("stage");
	do_add_general_state();

     update_commit = fopen("stage.txt", "w");
     fclose(update_commit);

	update_commit = fopen("history_stage.txt", "w");
     fclose(update_commit);
	

	// update branch
	chdir(".."), chdir("branch");
	FILE *branch_name = fopen("name.txt", "r");
	FILE *branch_last_commit = fopen("last_commit.txt", "r");
	FILE *tmp_branch_last_commit = fopen("tmp_last_commit.txt", "w");


	char in[maxn];
	while(fgets(in, maxn, branch_name) != NULL){
		if(!strcmp(in, last_branch_name)){
			fgets(in, maxn, branch_last_commit);
			fprintf(tmp_branch_last_commit, "%d\n", commit_id);

		}else{
			fgets(in, maxn, branch_last_commit);
			fprintf(tmp_branch_last_commit, "%s", in);
		}

	}
	fclose(branch_name);
	fclose(branch_last_commit);
	fclose(tmp_branch_last_commit);
	remove("last_commit.txt");
	rename("tmp_last_commit.txt", "last_commit.txt");


     // change inf files
     chdir("..");
     update_commit = fopen("last_commit_id.txt", "w");
     fprintf(update_commit, "%d\n", commit_id);
     fclose(update_commit);
}

void make_dir_commit(char repository_path[], int commit_id, char file_path1[], char file_path2[]){
	chdir(repository_path);
	chdir(".fp"), chdir("commits");
	if(mkdir(from_int_to_char(commit_id), 0755) != 0){
        printf("can not make dir_commit\n");
        return;
     }
	copy(file_path1, file_path2);
}

void do_commit(char massage[]){
    // check repository
    take_cur_path();
    out_fr get_repository = find_repository(Cur_path);
    if (get_repository.path == NULL || get_repository.er){
    		printf("error: you do not have a repository!\n");
     	return;
    }

    // check massage
    if(strlen(massage) > 72)
	    return;

    // check permmision
    chdir(get_repository.path), chdir(".fp");
    FILE *per_file = fopen("permission_commit.txt", "r");
    int per;
    fscanf(per_file, "%d", &per);
    if(per == 0){
        printf("error: you must be in head of a branch!\n");
        return;
    }
    fclose(per_file);


    char in[maxn];
    // check stage != NULL
    chdir(get_repository.path);
    chdir(".fp"), chdir("stage");
    FILE *tmp_file = fopen("stage.txt", "r");

    if(fgets(in, maxn, tmp_file) == NULL){
		printf("stage is empty!\n");
		return;
    }
    fclose(tmp_file);

    chdir(get_repository.path);
    chdir(".fp");

    // get last branch
    tmp_file = fopen("last_branch_id.txt", "r");
    char last_branch_name[maxn];
    fgets(in, maxn, tmp_file);
    fgets(in, maxn, tmp_file);
    strcpy(last_branch_name, in);
    fclose(tmp_file);

    // get cnt commits
    chdir("commits");
    tmp_file = fopen("cnt.txt", "r");
    int cnt_commits;
    fscanf(tmp_file, "%d", &cnt_commits);
    fclose(tmp_file);

    // get total commitd files
    chdir(".."), chdir("stage");

    FILE *stage_file = fopen("stage.txt", "r");
    int cnt_commited_files = 0;
    while(fgets(in, maxn, stage_file) != NULL){
          fgets(in, maxn, stage_file);
          fgets(in, maxn, stage_file);
          cnt_commited_files++;
    }

    //printf("SSSSS\n");
    update_informations_about_commits(get_repository.path, get_repository.name, get_repository.email,  massage, last_branch_name, cnt_commited_files, cnt_commits+1);
//	printf("bad update\n");	

    // commit
    char file_path1[maxn];
    strcpy(file_path1, get_repository.path);
    strcat(file_path1, "/.fp/commits/");
    strcat(file_path1, from_int_to_char(cnt_commits + 1));
    strcat(file_path1, "/inf.txt");

    char file_path2[maxn];
    strcpy(file_path2, get_repository.path);
    strcat(file_path2, "/.fp/stage/general_stage.txt"); /// CHANGE

  //  printf("ghabl make_dir\n");
    make_dir_commit(get_repository.path, cnt_commits+1, file_path1, file_path2);

    printf("your commit created sucsseccfully!\n");
}

int do_log_is_repository(){
    take_cur_path();
    out_fr get_repository = find_repository(Cur_path);
    if (!strcmp(get_repository.path , "") || get_repository.er){
        printf("error: you do not have a repository!\n");
        return 0;
    }

    chdir(get_repository.path);
    chdir(".fp"), chdir("commits");
    return 1;
}

void do_log_n(int number){
    if(!do_log_is_repository())
        return;

    FILE *cnt_commits = fopen("cnt.txt", "r");
    int cnt;
    fscanf(cnt_commits, "%d", &cnt);

    for(int i = cnt-number+1;i <= cnt; i++)
        do_log_show_commit(i);
}

void do_log_branch(char *branch_name){
     if(!do_log_is_repository())
         return;
     FILE *branch_commit = fopen("branch.txt", "r");

     int i = 1;
     char in[maxn];
     fgets(in, maxn, branch_commit);
	strcat(branch_name, "\n");

     while(!feof(branch_commit)){
         if(!strcmp(in, branch_name))
             do_log_show_commit(i);
         fgets(in, maxn, branch_commit);
         i++;
     }
}

void do_log_author(char *author_name){
	if(!do_log_is_repository())
         return;
     FILE *author_commit = fopen("author_name.txt", "r");

     int i = 1;
     char in[maxn];
     fgets(in, maxn, author_commit);
     strcat(author_name, "\n");

     while(!feof(author_commit)){
         if(!strcmp(in, author_name))
             do_log_show_commit(i);
         fgets(in, maxn, author_commit);
         i++;
     }
}

time_t to_time(char *string_time){
    int yy, month, dd, hh, mm, ss;
    sscanf(string_time, "%d/%d/%d %d:%d:%d\n", &yy, &month, &dd, &hh, &mm, &ss);

    struct tm strc_time;
    strc_time.tm_year = yy - 1900;
    strc_time.tm_mon = month - 1;
    strc_time.tm_mday = dd;
    strc_time.tm_hour = hh;
    strc_time.tm_min = mm;
    strc_time.tm_sec = ss;
    strc_time.tm_isdst = -1;

    time_t your_time;
    your_time = mktime(&strc_time);
    return your_time;
}

void do_log_since(int argc, char *argv[]){
    if(!do_log_is_repository())
        return;
    FILE *date_commit = fopen("date.txt", "r");

    int i = 1;
    char in[maxn];
 
    char given_date[max_time];
    strcpy(given_date, argv[3]);
    strcat(given_date, " ");
    strcat(given_date, argv[4]);
    strcat(given_date, "\n");

    time_t given_time = to_time(given_date);

    while(fgets(in, maxn, date_commit) != NULL){
        time_t commit_time = to_time(in);
	   //printf("commit time %sgiven time %s", in, given_date);
        if(difftime(commit_time, given_time) >= 0){
            break;
        }
        i++;
    }

    FILE *cnt_commits = fopen("cnt.txt", "r");
    int cnt;
    fscanf(cnt_commits, "%d", &cnt);

    for(int j = cnt;i <= j;j--)
        do_log_show_commit(j);
}

void do_log_before(int argc, char *argv[]){
    if(!do_log_is_repository())
        return;
    FILE *date_commit = fopen("date.txt", "r");

    int i = 1;
    char in[maxn];

    char given_date[max_time];
    strcpy(given_date, argv[3]);
    strcat(given_date, " ");
    strcat(given_date, argv[4]);
    strcat(given_date, "\n");

    time_t given_time = to_time(given_date);

    while(fgets(in, maxn, date_commit) != NULL){
        time_t commit_time = to_time(in);
        if(difftime(commit_time, given_time) >= 0){
            break;
        }
        i++;
    }

    FILE *cnt_commits = fopen("cnt.txt", "r");
    int cnt;
    fscanf(cnt_commits, "%d", &cnt);

    for(int j = i-1;1 <= j;j--)
        do_log_show_commit(j);
}

void do_log_search(char *search){
    if(!do_log_is_repository())
        return;
    FILE *massage_commit = fopen("massage.txt", "r");

    int i = 1;
    char in[maxn];
    fgets(in, maxn, massage_commit);

    while(!feof(massage_commit)){
        if(strstr(in, search) != NULL)
            do_log_show_commit(i);
        fgets(in, maxn, massage_commit);
        i++;
    }
}

void do_log_show_commit(int x){
    // open files
    FILE *date_commit = fopen("date.txt", "r");
    FILE *massage_commit = fopen("massage.txt", "r");
    FILE *author_name = fopen("author_name.txt", "r");
    FILE *author_email = fopen("authoe_gmail.txt", "r");
    FILE *branch_commit = fopen("branch.txt", "r");
    FILE *cnt_file_commit = fopen("total_commited_files.txt", "r");

    // go to the selected commit
    int i = 1;
    char in[maxn];
    while(i != x){
        fgets(in ,maxn, date_commit);
        fgets(in, maxn, massage_commit);
	   fgets(in, maxn, author_name);
	   fgets(in, maxn, author_email);
        fgets(in, maxn, branch_commit);
        fgets(in, maxn, cnt_file_commit);
        i++;
    }

    // printf selected commit
    printf("id: %d\n", i);
    fgets(in, maxn, massage_commit);
    printf("massage: %s", in);
    fgets(in, maxn, date_commit);
    printf("date: %s", in);
    
    fgets(in, maxn, author_name);
    printf("author name: %s", in);

    fgets(in, maxn, author_email);
    printf("author email: %s", in);

    fgets(in, maxn, branch_commit);
    printf("branch: %s", in);
    fgets(in, maxn, cnt_file_commit);
    printf("the number of commited files: %s", in);
    printf("\n");

    // close files
    fclose(massage_commit);
    fclose(date_commit);
    fclose(author_name);
    fclose(author_email);
    fclose(branch_commit);
    fclose(cnt_file_commit);
}

void do_status(){
    // chekck repository
    take_cur_path();
    out_fr get_repository = find_repository(Cur_path);
    if (!strcmp(get_repository.path , "") || get_repository.er){
        printf("error: you do not have a repository!\n");
        return;
    }

    chdir(get_repository.path);
    chdir(".fp");

    // get last commit id
    FILE *last_commit_id = fopen("last_commit_id.txt", "r");
    int nu_last_commit;
    fscanf(last_commit_id, "%d", &nu_last_commit);
    fclose(last_commit_id);

    if(nu_last_commit == 0){
	    chdir(get_repository.path);
	    dfs_s(get_repository.path, "", 1);
	    return;
    }


    // make dir last commit path
    char name_dir_last_commit[maxn];
    sprintf(name_dir_last_commit, "%d/inf.txt", nu_last_commit);

    char inf_last_commit_path[maxn];
    strcpy(inf_last_commit_path, get_repository.path);
    strcat(inf_last_commit_path, "/.fp/commits/");
    strcat(inf_last_commit_path, name_dir_last_commit);

    
    // output D
    FILE *inf_last_commit = fopen(inf_last_commit_path, "r");

    // output nulls
    chdir("..");
    chdir("stage");
    char in[max_path];

    char in2[maxn];
    while(fgets(in, max_path, inf_last_commit) != NULL){
		strcpy(in2, in);
		in2[strlen(in2)-1] = '\0';
        	if(fopen(in2, "r") == NULL)
            	printf("file path:%s-D\n", in);
        	fgets(in, max_path, inf_last_commit);
	   	fgets(in, max_path, inf_last_commit);
    }

    printf("before_dfs\n");

    chdir(get_repository.path);
    dfs_s(get_repository.path, inf_last_commit_path, 0);
}

void dfs_s(char repository_path[], char inf_last_commit_path[], int case_without_commit){
    DIR *dir = opendir(".");

    struct dirent* entry;
    while((entry = readdir(dir)) != NULL){
        if(strcmp(entry -> d_name, ".") && strcmp(entry -> d_name, "..") && strcmp(entry -> d_name, ".fp") && entry -> d_type == DT_REG){
            // make file path
            getcwd(Tmp_path, sizeof(Tmp_path));
            strcat(Tmp_path, "/");
            strcat(Tmp_path, entry -> d_name);

            // check stage
            printf("file path: %s\n", Tmp_path);
            if(is_stage(Tmp_path, repository_path)){
                printf("+");// is_stage_file = 1;
            }else{
                printf("-");
            }

		  if(case_without_commit){
			  printf("A\n");
			  continue;
		  }

            // get last_change_date
            struct stat info;
            stat(entry -> d_name, &info);
            time_t the_last_change = info.st_mtime;


            // check with_last_commit
            if(!do_check_with_last_commit(Tmp_path, the_last_change, inf_last_commit_path))
                printf("A");
            printf("\n");

        }
        if(strcmp(entry -> d_name, ".") && strcmp(entry -> d_name, "..") && strcmp(entry -> d_name, ".fp") && entry -> d_type == DT_DIR){
            chdir(entry -> d_name);
            dfs_s(repository_path, inf_last_commit_path, case_without_commit);
        }
    }
    closedir(dir);
    chdir("..");
}

int do_check_with_last_commit(char file_path[], time_t the_last_change, char inf_last_commit_path[]){
	FILE *inf = fopen(inf_last_commit_path, "r");

	char in[maxn];

     char fp_compare[max_path];
     strcpy(fp_compare, file_path);
     strcat(fp_compare, "\n");

     while(fgets(in, maxn, inf) != NULL){
		if(!strcmp(fp_compare, in)){
            	fgets(in, maxn, inf);
	//	   	printf("XXXDDD\n %s\n %s\n", in, ctime(&the_last_change));
             	if(difftime(to_time(in), the_last_change) != 0){
                 	printf("M");
				fclose(inf);
				return 1;
			}else{
				fclose(inf);
				return 0;
			}

         }else{
             fgets(in, maxn, inf);
             fgets(in, maxn, inf);
         }
     }
     fclose(inf);
     return 0;
}

void do_branch(int argc, char *argv[]){
    // check repository
    take_cur_path();
    out_fr get_repository = find_repository(Cur_path);
    if (get_repository.path == NULL || get_repository.er){
        printf("error: you do not have a repository!\n");
        return;
    }

    chdir(get_repository.path);
    chdir(".fp");

    int last_commit_id;
    FILE *last_commit = fopen("last_commit_id.txt", "r");
    fscanf(last_commit, "%d", &last_commit_id);
    fclose(last_commit);

    chdir("branch");

    char branch_name[maxn];
    strcpy(branch_name, argv[2]);
    //strcpy(branch_name, argv[2]+1);
    //for(int i = 3;i < argc;i++)
    //    strcat(branch_name, " "), strcat(branch_name, argv[i]);
    //branch_name[strlen(branch_name)-1] = '\0';

    FILE *cnt_file_branch = fopen("cnt.txt", "r");
    int cnt;
    fscanf(cnt_file_branch, "%d", &cnt);
    fclose(cnt_file_branch);

    FILE *file_name_branch = fopen("name.txt", "r");

    bool exist_branch = 0;

    char in[maxn];
    char nb_compare[max_path];
    strcpy(nb_compare, branch_name);
    strcat(nb_compare, "\n");

    while(fgets(in, maxn, file_name_branch) != NULL){
        if(!strcmp(in, nb_compare)){
            exist_branch = 1;
            break;
        }
    }
    fclose(file_name_branch);

    if(exist_branch){
        printf("error: you already have a branch with name %s!\n", branch_name);
        return;
    }

    file_name_branch = fopen("name.txt", "a");
    FILE *file_last_commit_branch = fopen("last_commit.txt", "a");
    fprintf(file_name_branch, "%s\n", branch_name);
    fprintf(file_last_commit_branch, "%d\n", last_commit_id);
    fclose(file_name_branch);
    fclose(file_last_commit_branch);

    cnt_file_branch = fopen("cnt.txt", "w");
    fprintf(cnt_file_branch, "%d\n", cnt+1);
}

void show_branch(){
    // check repository
    take_cur_path();
    out_fr get_repository = find_repository(Cur_path);
    if (get_repository.path == NULL || get_repository.er){
        printf("error: you do not have a repository!\n");
        return;
    }

    chdir(get_repository.path);
    chdir(".fp"), chdir("branch");

    FILE *branch_name = fopen("name.txt", "r");
    FILE *branch_last_commit = fopen("last_commit.txt", "r");

    char in[maxn];
    while(fgets(in, maxn, branch_name) != NULL){
        printf("branch nama: %s", in);
        fgets(in, maxn, branch_last_commit);
        printf("last_commit_id: %s", in);
        printf("\n");
    }

    fclose(branch_name);
    fclose(branch_last_commit);
}

int get_perm(char repository_path[], long long nu){
	take_cur_path();
	
	chdir(repository_path), chdir(".fp"), chdir("stage");
	FILE *per_file = fopen("per.txt", "r");

	int i = 1;
	char in[maxn];
	while(i != nu)
		fgets(in, maxn, per_file);
	
	// CHANGE
	return 777;

	return_selected_path();
}

void change_current_files(char repository_path[], int nu_commit, int case_permission){
    chdir(repository_path);
    chdir(".fp");

    printf("UUUU\n");
    chdir("commits");

    printf("JJJ %d\n", nu_commit);
    printf("XD\n");
    char tmptmp[maxn];
    strcpy(tmptmp , from_int_to_char(nu_commit));
    //char tmptmp[maxn];
    //strcpy(tmptmp, "HHHH");
    printf("tmp:%s\n", tmptmp);
    chdir(tmptmp);

    printf("GGGGG");
    FILE *inf_commit = fopen("inf.txt", "r");

    char in[maxn];
    
    while(fgets(in, maxn, inf_commit) != NULL){
        in[strlen(in)-1] = '\0';
        char file_path1[maxn];
        strcpy(file_path1, in);

        fgets(in, maxn, inf_commit);
        fgets(in, maxn, inf_commit);
        in[strlen(in)-1] = '\0';

        char file_path2[maxn];
        strcpy(file_path2, repository_path);
        strcat(file_path2, "/.fp/stage/");
        strcat(file_path2, in);
        strcat(file_path2, ".txt");

        copy(file_path1, file_path2);
        
        if(case_permission == 0){ // read only
            struct stat file_info;
            if(stat(file_path1, &file_info) != 0){
                printf("we can not get the premmision of file with path %s!\n", file_path1);
                continue;
            }
		  printf("HERE\n");
            mode_t file_mod = file_info.st_mode;
        //    chmod(file_path1, 0555);
            
        }else if(case_permission == 1){ // default permissions CHANGE
            //chmod(file_path1, get_perm(repository_path, get_perm(repository_path, Number(in))));
		//  chmod(file_path1, 0777);
        }
    }
    
    fclose(inf_commit);
}


void do_checkout(int argc, char* argv[]){
    	// check repository
	take_cur_path();
    	out_fr get_repository = find_repository(Cur_path);
	if (get_repository.path == NULL || get_repository.er){
     	printf("error: you do not have a repository!\n");
     	return;
    	}

	chdir(get_repository.path), chdir(".fp"), chdir("stage");
    	// check there is no modified fil
	// CHANGE

	//
    	chdir("..");

    	// make branch name  
    	char branch_name[maxn];
    	strcpy(branch_name, argv[2]);

     chdir("branch");

	//printf("before find check exist\n");
     // filnd bool exist_branch ?
     bool exist_branch = 0;
     FILE *file_name = fopen("name.txt", "r");
     FILE *file_last_commit = fopen("last_commit.txt", "r");

     char in[maxn];
     char bn_compare[max_path];
     strcpy(bn_compare, branch_name);
     strcat(bn_compare, "\n");

     int branch_id = 1;
     while(fgets(in, maxn, file_name) != NULL){
         if(!strcmp(in, bn_compare)){
             fgets(in, maxn, file_last_commit);
             exist_branch = 1;
             break;
         }
         branch_id++;
         fgets(in, maxn, file_last_commit);
     }
     fclose(file_name);
	fclose(file_last_commit);

	//printf("before check exist\n");
	// exist_branch = 1
     if(exist_branch){
         in[strlen(in)-1] = '\0';
         int nu = Number(in);
         chdir("..");
	
         FILE *update_file = fopen("last_commit_id.txt", "w");
         fprintf(update_file, "%d\n", nu);
         fclose(update_file);

         update_file = fopen("last_branch_id.txt", "w");
         fprintf(update_file, "%d\n", branch_id);
         fprintf(update_file, "%s\n", branch_name);
         fclose(update_file);
		
	    // premission_commit
	    update_file = fopen("permission_commit.txt", "w");
         fprintf(update_file, "%d\n", 1);
         fclose(update_file);

	    //printf("before fvfbgb\n");
         change_current_files(get_repository.path, nu, 1); // vaziat file ha be commit nu mire

	// exist_brach = 0
     }else{
	    printf("%s\n", branch_name);
         int nu = Number(branch_name);
         chdir("..");

	    // permission_commit
         FILE *update_file = fopen("permission_commit.txt", "w");
         fprintf(update_file, "%d\n", 0);
         fclose(update_file);

	    printf("before 0\n");
         change_current_files(get_repository.path, nu, 0); // vaziat file ha be commit nu mire v ejaze WRITE 0 mishe
     }
}

void do_checkout_HEAD(){
    // check repository
    take_cur_path();
    out_fr get_repository = find_repository(Cur_path);
    if (get_repository.path == NULL || get_repository.er){
        printf("error: you do not have a repository!\n");
        return;
    }

    chdir(get_repository.path);
    chdir(".fp");

    // get last_commit_id
    int nu_last_commit;
    FILE *update_file = fopen("last_commit_id.txt", "r");
    fscanf(update_file, "%d", &nu_last_commit);
    fclose(update_file);

    update_file = fopen("valid_commit.txt", "w");
    fprintf(update_file, "%d\n", 1);
    fclose(update_file);

    change_current_files(get_repository.path, nu_last_commit, 1);
}

void do_revert_n(int argc, char *argv[]){
    // check repository
    take_cur_path();
    out_fr get_repository = find_repository(Cur_path);
    if (get_repository.path == NULL || get_repository.er){
          printf("error: you do not have a repository!\n");
          return;
    }

    chdir(get_repository.path);
    chdir(".fp");

    FILE *last_commit_id = fopen("last_commit_id", "r");
    int nu_last_commit;
    fscanf(last_commit_id, "%d", &nu_last_commit);
    fclose(last_commit_id);

    // different cases
    if(argc == 3){
        change_current_files(get_repository.path, nu_last_commit, 1);

    }else if(argc == 4){
        change_current_files(get_repository.path, Number(argv[3]), 1);

    }else if(argc > 4)
        printf("error: invalid commit name\n");
}

void do_revert_go_back(int argc, char *argv[]){
    // check repository
    take_cur_path();
    out_fr get_repository = find_repository(Cur_path);
    if (get_repository.path == NULL || get_repository.er){
          printf("error: you do not have a repository!\n");
          return;
    }

//    chdir(".fp");

    // make_massage
    int is_massage = 0;
    char massage[maxn];

    if(!strcmp("-m", argv[2])){
        is_massage = 1;
	   strcpy(massage, argv[3]);

/*        strcpy(massage, argv[3]+1);
        if(argc > 5){
            for(int i = 4;i < argc;i++){
                strcat(massage, " ");
                strcat(massage, argv[i]);
                if(*(argv[i] + strlen(argv[i]) - 1) == '"'){
                    break;
                }
            }
        }*/
        //massage[strlen(massage)-1] = '\0';

        // check massage
        if(!strlen(massage) > 72){
		  printf("error: the message is too long!\n");
            return;
	   }
    }

    chdir(get_repository.path), chdir(".fp");
    FILE *per_file = fopen("permission_commit.txt", "r");
    int per;
    fscanf(per_file, "%d", &per);
    if(per == 0){
        printf("error: you must be in head of a branch!\n");
        return;
    }
    fclose(per_file);

    int nu = Number(argv[argc-1] + 5);

    chdir(get_repository.path);
    chdir(".fp"), chdir("commits");

    FILE *cnt_commits =  fopen("cnt.txt", "r");
    int cntc;
    fscanf(cnt_commits, "%d", &cntc);
    fclose(cnt_commits);

    nu = cntc - nu + 1;
    if(1 > nu){
        printf("error: you do not have %d commits!\n", nu);
        return;
    }

    if(is_massage)
        do_rep_commit(get_repository.path, get_repository.name, get_repository.email, nu, massage, 1);
    else
        do_rep_commit(get_repository.path, get_repository.name, get_repository.email, nu, "", 0);

    change_current_files(get_repository.path, nu, 1);
}

void do_revert_check_merge(int argc, char *argv[]){
    // check repository
    take_cur_path();
    out_fr get_repository = find_repository(Cur_path);
    if (get_repository.path == NULL || get_repository.er){
          printf("error: you do not have a repository!\n");
          return;
    }

    chdir(get_repository.path);
    chdir(".fp");

    // check permission commit
    FILE *per_file = fopen("permission_commit.txt", "r");
    int per;
    fscanf(per_file, "%d", &per);
    if(per == 0){
        printf("error: you must be in head of a branch!\n");
        return;
    }
    fclose(per_file);

    char massage[maxn];
    int is_massage = 0;
    if(!strcmp(argv[2], "-m")){
        is_massage = 1;

        // make_massage
        strcpy(massage, argv[3]+1);
        if(argc > 5){
            for(int i = 4;i < argc;i++){
                strcat(massage, " ");
                strcat(massage, argv[i]);
                if(*(argv[i] + strlen(argv[i]) - 1) == '"'){
                    break;
                }
            }
        }
        massage[strlen(massage)-1] = '\0';

        // check massage
        if(strlen(massage) > 72){
		  printf("error: the message is too long!\n");
            return;
	   }
    }

    char char_id_commit[maxn];
    strcpy(char_id_commit, argv[argc-1]+1);
    char_id_commit[strlen(char_id_commit)-1] = '\0';
    int nu = Number(char_id_commit);

    // check merge par

    if(is_massage)
        do_rep_commit(get_repository.path, get_repository.name, get_repository.email, nu, massage, 1);
    else
        do_rep_commit(get_repository.path, get_repository.name, get_repository.email, nu, "", 0);

    change_current_files(get_repository.path, nu, 1);
}

void show_tag(char tag_name[]){
    FILE *tag_file = fopen("tag.txt", "r");
    char in[maxn];
    int exist_tag = 0;

    while(fgets(in, maxn, tag_file) != NULL){
	   //printf("UUUUU");
        in[strlen(in)-1] = '\0';
        if(!strcmp(tag_name, in)){
            printf("tag name: %s\n", in);

            fgets(in, maxn, tag_file);
            printf("commid id: %s", in);

            fgets(in, maxn, tag_file);
            printf("author name: %s", in);

		  fgets(in, maxn, tag_file);
            printf("author email: %s", in);

            fgets(in, maxn, tag_file);
            printf("date: %s", in);

            fgets(in, maxn, tag_file);
            printf("massage: %s", in);

            printf("\n");
            exist_tag = 1;
            break;
        }else{
            fgets(in, maxn, tag_file);
            fgets(in, maxn, tag_file);
            fgets(in, maxn, tag_file);
            fgets(in, maxn, tag_file);
            fgets(in, maxn, tag_file);
        }
    }
    fclose(tag_file);

    if(!exist_tag)
        printf("error: you do not have any tag with name %s!", tag_name);
}

void do_show_tag(int argc, char *argv[]){
    // check repository
    take_cur_path();
    out_fr get_repository = find_repository(Cur_path);
    if (get_repository.path == NULL || get_repository.er){
          printf("error: you do not have a repository!\n");
          return;
    }


    // make tag name
    char tag_name[maxn];
    strcpy(tag_name, argv[3]);
    for(int i = 4;i < argc;i++)
        strcat(tag_name, " "), strcat(tag_name, argv[i]);

    //tag_name[strlen(tag_name)-1] = '\0';

    //

    chdir(get_repository.path);
    chdir(".fp");
    chdir("tag");

    //
    show_tag(tag_name);
}

int compare_word(char a[], char b[]){
    int as = strlen(a), bs = strlen(b);
    int min_len = (as <= bs ? as : bs);
    for(int i = 0;i < min_len ;i++){
        if(*(a+i) < *(b+i))
            return 1;
        if(*(a+i) > *(b+i))
            return 0;
    }
    if(as <= bs)
        return 1;
    return 0;
}

void do_show_tag_all(){
    // check repository
    take_cur_path();
    out_fr get_repository = find_repository(Cur_path);
    if (get_repository.path == NULL || get_repository.er){
          printf("error: you do not have a repository!\n");
          return;
    }

    chdir(get_repository.path);
    chdir(".fp");
    chdir("tag");

    //printf("HERE\n");// mark_tag
    int *remove = (int*) calloc(maxn , sizeof(int));
    while(1){
        FILE *tag_file = fopen("tag.txt", "r");

        int i = 0, is = -1;
        char tag_name[maxn];
        char in[maxn];

        while(fgets(in, maxn, tag_file) != NULL){
            in[strlen(in)-1] = '\0';
            if(*(remove + i) == 0){
                if(is != -1 && !compare_word(tag_name, in))
                    *(remove + is) = 0;
                if(is == -1 || !compare_word(tag_name, in))
                    is = i, *(remove + i) = 1, strcpy(tag_name, in);
            }

	//	  printf("i=%d tag-name=%s\n", i, tag_name);
            i++;
            fgets(in, maxn, tag_file);
            fgets(in, maxn, tag_file);
		  fgets(in, maxn, tag_file);
            fgets(in, maxn, tag_file);
            fgets(in, maxn, tag_file);
        }
        fclose(tag_file);

        if(is != -1){
            show_tag(tag_name);
        }else{
            break;
        }
    }
}

void do_tag(int argc, char *argv[]){
    // check repository
    take_cur_path();
    out_fr get_repository = find_repository(Cur_path);
    if (get_repository.path == NULL || get_repository.er){
          printf("error: you do not have a repository!\n");
          return;
    }
    chdir(get_repository.path);
    chdir(".fp");

    //printf("HERE\n"); // mark do tag
    int j = 4;

    // make tag name
    char tag_name[maxn];
    strcpy(tag_name, argv[3]);

    while(j < argc && strcmp(argv[j], "-m") && strcmp(argv[j], "-c") && strcmp(argv[j], "-f")){
	   strcat(tag_name, " "), strcat(tag_name, argv[j]);
	   //printf("j = %d * argv = %s * tag_name = %s\n", j, argv[j], tag_name);
        j++;
    }
    //printf("j = %d * argv = %s * tag_name = %s\n", j, argv[j], tag_name);


    // make massage
    char tag_massage[maxn];
    strcpy(tag_massage, "");
    if(j < argc && !strcmp("-m", argv[j])){
        strcat(tag_massage, argv[j+1]);
        j += 2; 
	   while(j < argc && strcmp(argv[j], "-c") && strcmp(argv[j], "-f")){
        	   strcat(tag_massage, " "), strcat(tag_massage, argv[j]);
        	   j++;
    	   }
    }

    // make commit id
    FILE *last_commit_file = fopen("last_commit_id.txt", "r");
    int commit_id;
    fscanf(last_commit_file, "%d", &commit_id);
    fclose(last_commit_file);

    if(j < argc && !strcmp("-c", argv[j]))
        commit_id = Number(argv[j+1]);


    chdir("tag");

    // find exist tag
    FILE *tag_file = fopen("tag.txt", "r");
    char in[maxn];
    int exist_tag = 0;

    while(fgets(in, maxn, tag_file) != NULL){
        in[strlen(in)-1] = '\0';
        if(!strcmp(tag_name, in)){
            exist_tag = 1;
            break;
        }

        fgets(in, maxn, tag_file);
        
	   fgets(in, maxn, tag_file);
	   fgets(in, maxn, tag_file);

        fgets(in, maxn, tag_file);
        fgets(in, maxn, tag_file);
    }
    fclose(tag_file);

    if(exist_tag && strcmp(argv[argc-1], "-f")){
        printf("error: you already have a tag with name %s! use -f to overwrite it!", tag_name);
        return;
    }else{
	   tag_file = fopen("tag.txt", "r");
        FILE *tmp_tag_file = fopen("tmp_tag.txt", "w");

        while(fgets(in, maxn, tag_file) != NULL){
            in[strlen(in)-1] = '\0';
            if(strcmp(tag_name, in)){
                fprintf(tmp_tag_file, "%s\n", in);

                fgets(in, maxn, tag_file);
                fprintf(tmp_tag_file, "%s", in);

                fgets(in, maxn, tag_file);
                fprintf(tmp_tag_file, "%s", in);

			 fgets(in, maxn, tag_file);
                fprintf(tmp_tag_file, "%s", in);

                fgets(in, maxn, tag_file);
                fprintf(tmp_tag_file, "%s", in);

                fgets(in, maxn, tag_file);
                fprintf(tmp_tag_file, "%s", in);

            }else{
                fgets(in, maxn, tag_file);
                // author
                fgets(in, maxn, tag_file);
                fgets(in, maxn, tag_file);
            }
        }


//	   printf("%s\n%s\n", tag_name, tag_massage);
        fprintf(tmp_tag_file, "%s\n", tag_name);
        fprintf(tmp_tag_file, "%d\n", commit_id);
        fprintf(tmp_tag_file, "%s\n", get_repository.name);
	   fprintf(tmp_tag_file, "%s\n", get_repository.email);
        time_t now; time(&now);
        fprintf(tmp_tag_file, "%s", ctime(&now));
        fprintf(tmp_tag_file, "%s\n", tag_massage);

	   fclose(tag_file);
	   fclose(tmp_tag_file);

	   remove("tag.txt");
	   rename("tmp_tag.txt", "tag.txt");

    }
}

void do_rep_commit(char repository_path[], char user_name[], char user_email[], int nu, char massage[], int is_massage){
    take_cur_path();
    
    chdir(repository_path);
    chdir(".fp"), chdir("commits");
    
    FILE *total_commited_files = fopen("total_commited_files.txt", "r");
    FILE *last_branch_name = fopen("branch.txt", "r");
    FILE *massage_commmit = fopen("massage.txt", "r");
    
    char in[maxn];
    int x = 1;
    while(x != nu){
        fgets(in, maxn, total_commited_files);
        fgets(in, maxn, last_branch_name);
        fgets(in, maxn, massage_commmit);
    }
    
    int tcf;
    char lbn[maxn], massage_c[maxn];
    fscanf(total_commited_files, "%d", &tcf);
    fscanf(last_branch_name, "%s", lbn);
    fscanf(massage_commmit, "%s", massage_c);
    massage_c[strlen(massage_c)-1] = '\0';
    
    fclose(total_commited_files);
    fclose(last_branch_name);
    fclose(massage_commmit);
    
    FILE *cnt_commits = fopen("cnt.txt", "r");
    int cntc;
    fscanf(cnt_commits, "%d", &cntc);
    fclose(cnt_commits);

    if(is_massage)
        update_informations_about_commits(repository_path, user_name, user_email, massage, lbn, tcf, cntc+1);
    else
        update_informations_about_commits(repository_path, user_name, user_email, massage_c, lbn, tcf, cntc+1);
    
    char file_path1[maxn];
    strcpy(file_path1, repository_path);
    strcat(file_path1, "/.fp/commits/");
    strcat(file_path1, from_int_to_char(cntc+1));
    strcat(file_path1, "/inf.txt");
    
    char file_path2[maxn];
    strcpy(file_path2, repository_path);
    strcat(file_path2, "/.fp/commits/");
    strcat(file_path2, from_int_to_char(nu));
    strcat(file_path2, "/inf.txt");
    
    make_dir_commit(repository_path, cntc+1, file_path1, file_path2);
    
    change_current_files(repository_path, nu, 1);
    return_selected_path();
}

