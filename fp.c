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

void do_reset(int, char*[]);
void do_remove_stage_file(char*);

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

    	if(get_repository.path == NULL){
		if(mkdir(".fp", 0755) != 0)
        		return;
	    
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
			
			take_cur_path();
			for(int i = 3;i < argc;i++){

				// make path
				strcpy(entry_path, Cur_path);
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
			take_cur_path();
			Dfs(Cur_path, nu, 1, get_repository.path);
		}
	}
}

void Dfs(char *path, int deep, int case_dfs, char *repository_path){
	take_cur_path();
	dfs(path, deep, case_dfs, repository_path);
	return_selected_path();
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
	strcat(repository_path, "/.fp/stage");

	take_cur_path();
	chdir(repository_path);
	
	FILE *stage_file = fopen("stage.txt", "r");
	
	FILE *inf_stage = fopen("inf.txt", "r");
	int cnt;
	fscanf(inf_stage, "%d", &cnt);
	cnt++;
	fclose(inf_stage);
	fopen("inf.txt", "w");
	fprintf(inf_stage, "%d", cnt);
	fclose(inf_stage);
	
	FILE *tmp_stage = fopen("tmp_stage.txt", "w");
	
	char in[max_path];
	while(fgets(in, max_path, stage_file) != NULL){
		if(strcmp(in, file_path)){
			fputs(in, tmp_stage);
			fgets(in, max_path, stage_file);
			fputs(in, tmp_stage);
			fgets(in, max_path, stage_file);
			fputs(in, tmp_stage);
		}else{
			fgets(in, max_path, stage_file);
			fgets(in, max_path, stage_file);
		}
	}
	
	fputs(file_path, tmp_stage);
	// fputs date last version
	fprintf(tmp_stage, "%d\n", cnt);

	// copy file in here
	remove("stage.txt");
	rename("tmp_stage.txt", "stage.txt");
	
	fclose(stage_file);

	return_selected_path();
}


bool is_stage(char *path, char *repository_path){
    take_cur_path();

    bool is_stage = false;

    chdir(repository_path);
    chdir(".fp/stage");
    FILE *stage_file = fopen("stage.txt", "r");

    char *in = (char*) malloc(max_path * sizeof(char));

    in = fgets(in, max_path, stage_file);
    while(!feof(stage_file)){
        if(!strcmp(path, in)){
            is_stage = true;
            break;
        }
        in = fgets(in, max_path, stage_file);
        in = fgets(in, max_path, stage_file);
        in = fgets(in, max_path, stage_file);
    }
    fclose(stage_file);
    
    return_selected_path();

    return is_stage;
}

void do_reset(int argc, char *argv[]){
    if(argc < 3){
        printf("error: invalid command\n");
        return;
    }

    out_fr get_repository = find_repository(argv[2]);
    if (get_repository.path == NULL || get_repository.er){
        printf("error: you do not have a repository!\n");
        return;
    }

    if(strcmp(argv[2], "-undo") && argc == 3){

		take_cur_path();
		
		DIR *input_dir = opendir(argv[2]);
		
		char file_path[max_path];
		strcpy(file_path, Cur_path);
		strcat(file_path, "/");
		strcat(file_path, argv[2]);
		FILE *input_file = fopen(file_path, "r");

		//check directory
		if(input_dir != NULL){
			closedir(input_dir);
			Dfs_r(file_path);

		// check file
		}else if(input_file != NULL){
			fclose(input_file);
			do_remove_stage_file(file_path);

		// not exist
		}else{
			printf("error: there is no file or directory with path:%s!\n", argv[2]);
        	}
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

void do_remove_stage_file(char *file_path) {
    out_fr get_repository = find_repository(file_path);
    if (get_repository.path == NULL || get_repository.er){
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

            int length = strlen(in);
            if (length > 0 && in[length - 1] == '\n')
                in[length - 1] = '\0';
            fputs(in, tmp_file);

            fgets(in, max_path, stage_file);
            length = strlen(in);
            if (length > 0 && in[length - 1] == '\n')
                in[length - 1] = '\0';
            fputs(in, tmp_file);

            fgets(in, max_path, stage_file);
            length = strlen(in);
            if (length > 0 && in[length - 1] == '\n')
                in[length - 1] = '\0';
            fputs(in, tmp_file);
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
/*
void do_commit(int argc, char argv[]){
    // check massage
    if(!check_massage(argc, argv[]))
        return;

    // check repository
    take_cur_path();
    out_fr get_repository = find_repository(Cur_path);
    if (get_repository.path == NULL || get_repository.er){
        printf("error: you do not have a repository!\n");
        return;
    }

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

    // commit
    chdir(get_repository.path);
    chdir(".fp/commits");

    FILE *inf_file = fopen("cnt.txt", "r");
    int cnt_commit;
    fscanf(inf_file, "%d", &cnt_commit);
    fclose(inf_file);
    cnt_commit++;

    chdir("../stage")
    FILE *stage_file = fopen("stage.txt", "r");
    char line[max_path];

    // bre akhar 
    chdir("commits");
    char dir_commit_name[max_name];
    sprintf(dir_commit_name, "%d", cnt_commit);
    if(mkdir(dir_commit_name, 0755) != 0){
        printf("can not make dir_commit\n");
        return;
    }
    chdir(dir_commit_name);

    int cnt_commit_files = 0;
    while(fgets(line, sizeof(line), stage_file) != NULL){
        int length = strlen(line);

        // remove '\n'
        if (length > 0 && line[length - 1] == '\n') {
            line[length - 1] = '\0';
        }

        cnt_commit_file += commit_staged_file(line);
    }

    FILE *commit_inf_file = fopen("inf.txt", "w");
    fclose(commit_inf_file);
    commit_inf_file = fopen("inf.txt", "w");

    // information
    time_t ziro_time = time(0);
    fprintf(commit_inf_file, "%s\n", ctime(&ziro_time));
    fprintf(commit_inf_file, "%s\n", massage);
    // author
    fprintf(commit_inf_file, "%d\n", cnt_commit);
    fprintf(commit_inf_file, "%d\n", cnt_commit_files);
}
*/
int do_log_is_repository(){
    take_cur_path();
    out_fr get_repository = find_repository(Cur_path);
    if (get_repository.path == NULL || get_repository.er){
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

/*
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
*/


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

