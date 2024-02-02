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

int take_cur_path();
int selected_paht();
long long Number(char*);

typedef struct out_fr{
     char path[max_path];
     int er;
} out_fr;

void do_init(int, char*[]);
out_fr find_repository(char*);

void do_add(int, char*[]);
void do_add_stage(char*, char*);
void Dfs(char*, int, int, char*);
void dfs(int, int, char*);
bool is_stage(char*, char*);

void do_reset(int, char*[]);
void do_remove_stage_file(char*);
void Dfs_r(char*);
void dfs_r(char*);

time_t to_time(char*);
int do_log_is_repository();
void do_log_n(int);
void do_log_branch(char*);
void do_log_author(char*);
void do_log_since(int, char*[]);
void do_log_before(int, char*[]);
void do_log_search(char*);
void do_log_show_commit(int);

int main(int argc, char *argv[]){
	if(argc < 2)
		printf("error: invalid command!\n");   
	//if (getcwd(primary_path, sizeof(primary_path)) == NULL)
     //    return 0;

	if(!strcmp("init", argv[1]))
		do_init(argc, argv);

	if(!strcmp("add", argv[1]))
		do_add(argc, argv);

	if(!strcmp("reset", argv[1]))
		do_reset(argc, argv);

	if(!strcmp("status", argv[1]))
		do_status();

	if(!strcmp("commit", argv[1]) && argc == 4 && !strcmp("-m", argv[2]))
		do_commit(argc, argv);

	if(!strcmp("log", argv[1])){
		if(argc == 2)
	     	do_log_n(1);
		else if(!strcmp("-n", argv[2]) && argc == 4)
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
    	}
}

int Strcmp(char c[], char d[]){
	char a[maxn], b[maxn];
	strcpy(a, c), strcpy(b, d);

	int a_len = strlen(a);
	int b_len = strlen(b);

	if(a_len > 0 && a[a_len-1] == '\n')
		a[a_len-1] = '\0';
	if(b_len > 0 && b[b_len-1] == '\n')
		b[b_len-1] = '\0';

	return strcmp(a, b);
}

char* parent(char *path){
     char parent_path[max_path];

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

	return_selected_path();
	return output;
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
	    
		// inf files
		FILE * make_file = fopen("last_commit_id.txt", "w");
		fprintf(make_file, "%d\n", 0);
		fclose(make_file);

		make_file = fopen("last_branch_id.txt", "w");
          fprintf(make_file, "%d\n", 1);
		fprintf(make_file, "%s\n", "master");
          fclose(make_file);

		make_file = fopen("last_commit_HEAD.txt", "w");
          fprintf(make_file, "%d\n", 0);
          fclose(make_file);


		chdir(".fp");
		if(mkdir("stage", 0755) != 0 || mkdir("commits", 0755) != 0 || mkdir("branch", 0755) != 0)
			return;
	    
		// files relative to stage
		chdir("stage");
		FILE *make_file = fopen("inf.txt", "w");
		fprintf(make_file, "%d", 0);
		fclose(make_file);
	            
		make_file = fopen("stage.txt", "w");
		fclose(make_file);

		make_file = fopen("stage_delete.txt", "w");
          fclose(make_file);

		// files relative to commit
		chdir("../commits");
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
		chdir("../branch");
		make_file = fopen("cnt.txt", "w");
		fprintf(make_file, "%d\n", 1);
          fclose(make_file);

		make_file = fopen("name.txt", "w");
		fprintf(make_file, "%s\n", "master");
          fclose(make_file);

		make_file = fopen("last_commit.txt", "w");
		fprintf(make_file, "%s\n", 0);
		fclose(make_file);

		printf("the repository was created successfully!\n");
    }else
        printf("error: .fp repository has already initialized!\n");
}

void do_add(int argc, char *argv[]){
	if(argc < 3)
          return;

	if(((*argv[2]) != '-') && (strstr(argv[2], "*") == NULL) && (argc == 3)){
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
		take_cur_path();
          out_fr get_repository = find_repository(Cur_path);
          if (!strcmp(get_repository.path , "") || get_repository.er){
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

	// make stage path and go in it
	char *stage_path = malloc(max_path * sizeof(char));
	strcpy(stage_path, repository_path);
	strcat(stage_path, "/.fp/stage");
	chdir(stage_path);
	FILE *stage_file = fopen("stage.txt", "r");
	
	// open inf.txt and update cnt
	FILE *inf_stage = fopen("inf.txt", "r");
	int cnt;
	fscanf(inf_stage, "%d", &cnt);
	cnt++;
	fclose(inf_stage);
	fopen("inf.txt", "w");
	fprintf(inf_stage, "%d", cnt);
	fclose(inf_stage);
	printf("cnt=%d\n", cnt); /// 

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
	
	fprintf(tmp_stage, "%s\n", file_path);
	
	struct stat info;
     stat(file_path, &info);
     time_t last_change = info.st_mtime;
	char char_last_change[max_time];
	strftime(char_last_change, max_time, "%Y/%m/%d %H:%M:%S", localtime(&last_change));
	fprintf(tmp_stage, "%s\n", char_last_change);

	fprintf(tmp_stage, "%d\n", cnt);

	// copy file in here
	fclose(tmp_stage);
	remove("stage.txt");
	rename("tmp_stage.txt", "stage.txt");
	
	free(stage_path);

	return_selected_path();

	printf("file with path:%s added successfully!\n", file_path);
}


bool is_stage(char path[], char repository_path[]){
    take_cur_path();

    bool is_stage = false;

    chdir(repository_path);
    chdir(".fp/stage");
    FILE *stage_file = fopen("stage.txt", "r");

    char *in = (char*) malloc(max_path * sizeof(char));
    in = fgets(in, max_path, stage_file);

    char fp_compare[max_path];
    strcpy(fp_compare, path);
    strcat(fp_compare, "\n");

    while(!feof(stage_file)){
        if(!strcmp(fp_compare, in)){
            is_stage = true;
            break;
        }
        in = fgets(in, max_path, stage_file);
        in = fgets(in, max_path, stage_file);
        in = fgets(in, max_path, stage_file);
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

    if(((*argv[2]) != '-') && (strstr(argv[2], "*") == NULL) && (argc == 3)){
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
               closedir(input_dir);
			Dfs_r(file_path);

          // check file
          }else if(input_file != NULL){
               fclose(input_file);
			do_remove_stage_file(file_path);

          // not exist
          }else
               printf("error: there is no file or directory with path:%s!\n", argv[2]);


    }else if(((*argv[2]) != '-') && (argc == 3)){
	    // wildcard

    }else if(!strcmp(argv[2], "-undo") && argc == 3){
        take_cur_path();

        chdir(get_repository.path);
        chdir(".fp");

        //

        return_selected_path();
    }else{
	    printf("invliad command!\n");
    }
}

void Dfs_r(char *dir_path){
	char our_path[max_path];
     getcwd(our_path, sizeof(our_path));

     chdir(dir_path);
     dfs_r();

     chdir(our_path);
}

void dfs_r(){
     DIR *dir = opendir(".");

     struct dirent* entry;
     while((entry = readdir(dir)) != NULL){
          if(strcmp(entry -> d_name, ".") && strcmp(entry -> d_name, "..") && strcmp(entry -> d_name, ".fp") && entry -> d_type == DT_REG){
               getcwd(Tmp_path, sizeof(Tmp_path));
               strcat(Tmp_path, "/");
               strcat(Tmp_path, entry -> d_name);

               do_remove_stage_file(Tmp_path);
          }
          if(strcmp(entry -> d_name, ".") && strcmp(entry -> d_name, "..") && strcmp(entry -> d_name, ".fp") && entry -> d_type == DT_DIR){
               chdir(entry -> d_name);
               dfs_r();
          }
     }

     closedir(dir);
	chdir("..");
}

void do_remove_stage_file(char *file_path) {
    out_fr get_repository = find_repository(file_path);
    if (!strcmp(get_repository.path , "") || get_repository.er){
        printf("error: you do not have a repository!\n");
        return;
    }

    take_cur_path();
    chdir(get_repository.path);
    chdir(".fp/stage");

    FILE *stage_file = fopen("stage.txt", "r");
    FILE *tmp_file = fopen("tmp_stage.txt", "w");

    char in[max_path];
    while (fgets(in, max_path, stage_file) != NULL) {
        if (strcmp(file_path, in)){
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

void do_commit(int argc, char argv[]){
    // check repository
    out_fr get_repository = find_repository(primary_path);
    if (get_repository.path == NULL || get_repository.er){
        printf("error: you do not have a repository!\n");
        return;
    }

    // check massage
    if(!check_massage(argc, argv[]))
        return;

    // check stage != NULL
    chdir(get_repository.path);
    chdir(".fp/stage");
    FILE *tmp_file = fopen("stage.txt", "r");
    FILE *tmp2_file = fopen("stage_delete.txt", "r");
    char in[maxn];
    if(!fgets(in, maxn, inf_file) == NULL){
        if(!fgets(in, maxn, inf2_file) == NULL){
            printf("stage is empty!\n");
            return;
        }
    }
    fclose(inf_file);
    fclose(inf2_file);

    // make massage
    char *massage = (char*) calloc(max_massage * sizeof(char));
    if(argc == 4)
        strcat(massage, "\"");
    for(int i = 3;i < argc;i++){
        if(i != 3)
            strcat(massage, " ");
        strcat(massage, argv[i]);
    }
    if(argc == 4)
        strcat(massage, "\"");

    // get last branch id
    chdir(get_repository.path);
    chdir(".fp");

    tmp_file = fopen("last_branch_id.txt", "r");
    char last_branch_name[maxn];
    fgets(in, maxn, tmp_file);
    fgets(in, maxn, tmp_file);
    strcpy(last_branch_name, id);
    fclose(tmp_file);

    // get cnt commits and increase it by one
    chdir("commits")
    tmp_file = fopen("cnt.txt", "r");
    int cnt_commits;
    fscanf(tmp_file, "%d", &cnt_commits);

    fclose(tmp_file);
    tmp_file = fopen("cnt.txt", "w");
    cnt++;
    fprintf(tmp_file, "%d", cnt);
    fclose(tmp_file)

    // open stage
    chdir("../stage");
    FILE *stage_file = fopen("stage.txt", "r");

    // make dir commit
    chdir("../commits");
    char dir_commit_name[max_name];
    sprintf(dir_commit_name, "%d", cnt_commit);
    if(mkdir(dir_commit_name, 0755) != 0){
        printf("can not make dir_commit\n");
        return;
    }
    chdir(dir_commit_name);
    FILE *inf_commit_file = fopen("inf.txt", "w");

    // commit
    char in[maxn], id[maxn];
    int cnt_commit_files = 0;
    while(fgets(in, maxn, stage_file) != NULL){
        fputs(in, inf_commit_file);

        fgets(in, maxn, stage_file);
        fputs(in, inf_commit_file);

        fgets(in, maxn, stage_file);
        fputs(in,inf_commit_file);

        int id_file;
        sscanf(in, "%d\n", &id_file);
        sprintf(id, "%d.txt", id_file);

        // fili ba esm id az stage ro bayad bardarim copy conim to get_repository.path/.fp/commits/cnt(dir)/ba hamon esm

        cnt_commit_file ++;
    }


    // update commits
    chdir(get_repository.path);
    chdir(".fp/commits");

    // massage
    FILE *update_commit = fopen("massage.txt", "a");
    fputs(update_commit "%s\n", massage);
    fclose(update_commit);

    // date
    update_commit = fopen("date.txt", "a");
    char cur_time[max_time];
    time_t now; time(&now);
    strftime(cur_time, max_time, "%Y/%m/%d %H:%M:%S", localtime(&now));
    fputs(update_commit, "%s\n", cur_time);
    fclose(update_commit);

    // branch
    update_commit = fopen("branch.txt", "a");
    fputs(update_commit, "%s\n", last_branch_name);
    fclose(update_commit);

    // total commit files
    update_commit = fopen("total_commit_files.txt", "a");
    fputs(update_commit, "%d\n", cnt_commit_files);
    fclose(update_commit);
}

int do_log_is_repository(){
    take_cur_path();
    out_fr get_repository = find_repository(Cur_path);
    if (!strcmp(get_repository.path , "") || get_repository.er){
        printf("error: you do not have a repository!\n");
        return 0;
    }

    chdir(get_repository.path);
    chdir(".fp/commits");
    return 1;
}

void do_log_n(int number){
    if(!do_log_is_repository())
        return;

    FILE *cnt_commits = fopen("cnt.txt", "r");
    int cnt;
    fscanf(cnt_commits, "%d", &cnt);

    for(int i = cnt-number+1;i < cnt; i++)
        do_log_show_commit(i);
}

void do_log_branch(char *branch_name){
    if(!do_log_is_repository())
        return;
    FILE *branch_commit = fopen("branch.txt", "r");

    int i = 1;
    char in[maxn];
    fgets(in, maxn, branch_commit);

    while(!feof(branch_commit)){
        if(!strcmp(in, branch_name))
            do_log_show_commit(i);
        fgets(in, maxn, branch_commit);
        i++;
    }
}

void do_log_author(char *author_name){

}

time_t to_time(char *string_time){
    int yy, month, dd, hh, mm, ss;
    sscanf(string_time, "%d/%d/%d %d:%d:%d", &yy, &month, &dd, &hh, &mm, &ss);

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
    fgets(in, maxn, date_commit);

    strcat(argv[3], " ");
    strcat(argv[3], argv[4]);
    time_t given_time = to_time(argv[3]);

    while(!feof(date_commit)){
        time_t commit_time = to_time(in);
        if(difftime(commit_time, given_time) >= 0){
            break;
        }
        fgets(in, maxn, date_commit);
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
    fgets(in, maxn, date_commit);

    strcat(argv[3], " ");
    strcat(argv[3], argv[4]);
    time_t given_time = to_time(argv[3]);

    while(!feof(date_commit)){
        time_t commit_time = to_time(in);
        if(difftime(commit_time, given_time) >= 0){
            break;
        }
            do_log_show_commit(i);
        fgets(in, maxn, date_commit);
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
        // author
    FILE *branch_commit = fopen("branch.txt", "r");
    FILE *cnt_file_commit = fopen("cnt_file.txt", "r");

    // go to the selected commit
    int i = 1;
    char in[maxn];
    while(i != x){
        fgets(in ,maxn, date_commit);
        fgets(in, maxn, massage_commit);
        fgets(in, maxn, branch_commit);
        fgets(in, maxn, cnt_file_commit);
        i++;
    }

    // printf selected commit
    printf("id: %d\n", i);
    fgets(in, maxn, massage_commit);
    printf("massage: %s\n", in);
    fgets(in, maxn, date_commit);
    printf("date: %s\n", in);
        // author
        // gmail
    fgets(in, maxn, branch_commit);
    printf("branch: %s\n", in);
    fgets(in, maxn, cnt_file_commit);
    printf("the number of commited files: %s\n", in);

    // close files
    fclose(massage_commit);
    fclose(date_commit);
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

    // make dir last commit path
    char name_dir_last_commit[maxn];
    sprintf(name_dir_last_commit, "%d/inf.txt", last_commit_id);

    char inf_last_commit_path[maxn];
    strcpy(inf_last_commit_path, get_repository.path);
    strcat(inf_last_commit_path, "/.fp/commits/");
    strcat(inf_last_commit_path, name_dir_last_commit);

    // make stage file path // be che dard mikhore?
    //char stage_file_path[maxn];
    //strcpy(stage_file_path, get_repository.path);
    //strcat(stage_file_path, "/.fp/stage/stage.txt");

    /*
    // output nulls
    chdir("stage");
    FILE *stage_delete = fopen("stage_delete.txt", "r");
    FILE *inf_last_commit = fopen(inf_last_commit_path, "r");


    // output nulls
    chdir("../stage");
    char in[max_path];
    while(fgets(in, max_path, inf_last_commit) != NULL){
        if(fopen(in, "r") == NULL){
            int is_stage = 0;
            in2[max_path];
            while(fgest(in2, max_path, stage_delet) != NULL)
                if(!Strcmp(in, in2))
                    is_stage = 1;
            fclose(stage_delet);
            stage_delet = fopen("stage_delete.txt", "r");

            if(is_stage){
                printf("file path:%s\n+D\n", in);
            }else{
                printf("file path:%s\n+D\n", in);
            }
        }
        fgets(in, max_path, inf_last_commit);
    }*/

    chdir(get_repository.path);
    Dfs_s(get_repository.path);
}

void dfs_s(char repository_path[], char inf_last_commit_path[]){
    DIR *dir = opendir(".");

    struct dirent* entry;
    while((entry = readdir(dir)) != NULL){
        if(strcmp(entry -> d_name, ".") && strcmp(entry -> d_name, "..") && strcmp(entry -> d_name, ".fp") && entry -> d_type == DT_REG){
            // make file path
            getcwd(Tmp_path, sizeof(Tmp_path));
            strcat(Tmp_path, "/");
            strcat(Tmp_path, entry -> d_name);

            // check stage
            printf("file path: %s\n", entry -> d_name, Tmp_path);
            int is_stage = 0;
            is_stage(Tmp_path, repository_path){
                printf("+"), is_stage = 1;
            }else{
                printf("-");
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
            dfs(repository_path, inf_last_commit_path);
        }
    }
    closedir(dir);
    chdir("..");
}

int do_check_with_last_commit(char file_path[], time_t the_last_change, char inf_last_commit_path[]){
    FILE *inf = fopen(inf_last_commit_path, "r");

    char in[maxn];
    while(fgets(in, maxn, inf) != NULL){
        if(!Strcmp(file_path, in)){
            fgets(in, maxn, inf);
            if(difftime(to_time(in), the_last_change) != 0)
                printf("M");
            fclose(inf);
            return 1;

        }else{
            fgets(in, maxn, inf);
            fgets(in, maxn, inf);
        }
    }
    fclose(inf);
    return 0;
}
