#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <math.h>
#include <sys/stat.h>

#define line_length 2000
#define line_token_size 128
#define line_delimiter " \t\r\n\a"

////////////////////////////

int shell_cd(char** args);

int shell_help(char** args);

int shell_exit(char** args);

int shell_echo(char** args);

int shell_pwd(char** args) ;

int shell_cat(char** args) ;

int shell_clear();

int shell_mkdir(char** args);

int shell_rm(char** args);

int shell_sleep(char** args);

int shell_ls(char** args);

//int shell_pipe(char** args);
///////////////////////////
char cwd[2048];

char* builtin_cmd[]={
			"cd",
			"help",
			"exit",
			"echo",
			"pwd",
			"cat",
			"clear",
			"mkdir",
			"rm",
			"sleep",
			"ls"
		   };


int (*builtin_func[]) (char**) = {
	&shell_cd,
	&shell_help,
	&shell_exit,
	&shell_echo,
	&shell_pwd,
	&shell_cat,
	&shell_clear,
	&shell_mkdir,
	&shell_rm,
	&shell_sleep,
	&shell_ls
};

int num_builtin_func() 
{
	return sizeof(builtin_cmd)/sizeof(char*);
}

int shell_cd(char** args)
{
	if(args[1] == NULL)
	{
		printf("Argument expected to cd command \n");
	}
	else
	{
	//	char cwd[2048];

	//	getcwd(cwd,sizeof(cwd));
	
	//	printf("%s\n",cwd);

		if(chdir(args[1]) != 0)
		{
			perror("cd error\n");
		}

		getcwd(cwd,sizeof(cwd));

	//	printf("%s\n",cwd);
	}

	return 1;
}

int shell_help(char** args)
{
	int i;

	printf("The following the are the builtin functions\n");

	for(i=0;i < num_builtin_func();i++)
	{
		printf(" %s\n",builtin_cmd[i]);
	}

	return 1;
}

int shell_exit(char** args)
{
	write(0,"\x1b[2J",5);
	write(0,"\x1b[H",4);
	return 0;
}


int shell_echo(char** args)
{
	
	
	for(int j=1;j<sizeof(args);j++)
	{
		if(args[j] ==NULL)
		{
			break;
		}

		printf("%s ",args[j]);
	}
	printf("\n");

	return 1;
}

int shell_pwd(char** args)
{
	char cwd[1024];
	getcwd(cwd,sizeof(cwd));
	printf("%s\n",cwd);
}


int shell_cat(char** args)
{
	int count=0;

	for(int i=0;i<sizeof(args);i++)
	{
		if(args[i]!=NULL)
		{
			count++;
		}
		else
		{
			break;
		}
	}

	if(count < 2)
	{
		printf("syntax as follows\n");
		printf("cat <file to read>\n");
		return 1;
	}
	
	if(count >= 3)
	{
		if(count !=4)
		{
			printf("syntax as follows\n");
			printf("cat <file to read> | <file on which the content is to be written\n");
			return 1;
		}
	}

	if(count == 2)
	{
		int fd=open(args[1],O_RDONLY);

		if(fd < 0)
		{
			perror("open");
			return 1;
		}

		char c;
	
		printf("\n");

		while(read(fd,&c,1))
		{
			write(STDOUT_FILENO,&c,1);
		}
	
		close(fd);

		printf("\n");

		return 1;
	}

	if(count == 4)
	{
		if(strcmp(args[2],"|") == 0)
		{
			char* filename1 = args[1];

			char* filename2 = args[3];

			int fd = open(filename1,O_RDONLY);

			int fd1 = open(filename2,O_WRONLY);

			if(fd < 0)
			{
				perror("open");
				return 1;
			}

			char c;

			while(read(fd,&c,1))
			{
				write(fd1,&c,1);
			}
			
			close(fd1);
			close(fd);

			return 1;
		}
	}
		

}

int shell_clear()
{
	write(0,"\x1b[2J",5);
	write(0,"\x1b[H",4);

	return 1;
}


int shell_mkdir(char** args)
{
	int count=0;

	for(int i=0;i<sizeof(args);i++)
	{
		if(args[i]!=NULL)
		{
			count++;
		}
		else
		{
			break;
		}
	}

	if(count != 2)
	{
		printf("Please provide the filename\n");
		return 1;
	}

	char* dirname = args[1];

	int check = mkdir(dirname,0777);

	if(!check)
	{
		printf("Directory created\n");
	}
	else
	{
		printf("Unable to create directory\n");
		return 1;
	}

	return 1;
}

int shell_rm(char** args)
{
	int count=0;

	for(int i=0;i<sizeof(args);i++)
	{
		if(args[i]!= NULL)
		{
			count++;
		}
		else
		{
			break;
		}
	}

	if(count != 2)
	{
		printf("Please provide a filename!\n");

		return 1;
	}

	int check =remove(args[1]);

	if(!check)
	{
		printf("File is succesfully removed!\n");
	}
	else
	{
		printf("Unable to remove file!\n");

		return 1;
	}

	return 1;
}


int shell_sleep(char** args)
{
	int count = 0;

	for(int i=0;i<sizeof(args);i++)
	{
		if(args[i] !=NULL)
		{
			count++;
		}
		else
		{
			break;
		}
	}

	if(count !=2)
	{
		printf("Please provide time for which sleep will occur\n");

		return 1;
	}

	char* s=args[1];
	int time=0;
	
	int size = strlen(s);

	sscanf(s,"%d",&time);

	sleep(time);


	return 1;
}

int shell_ls(char** args)
{
	
	struct dirent* d;

	DIR* dr = opendir(".");

	if(dr == NULL)
	{
		printf("Couldn't not list the current directory\n");

		return 1;
	}

	while((d = readdir(dr))!=NULL)
	{
		printf("%s\n",d->d_name);
	}

	closedir(dr);

	return 1;
}
/*
int shell_pipe(char**  args)
{
	char* filename1 = args[0];

	char* filename2= args[2];

	int pipefd[2];

	if(pipe(pipefd) == -1)
	{
		perror("pipe");
		return 1;
	}


}
*/

char* shell_read_line()
{
	int length = line_length;

	int pos = 0;

	char*  line = (char*) malloc(sizeof(char) * length);
	char c;

	if(!line)
	{
		printf("Memory Allocation Fail!\n");
		exit(0);
	}
	else
	{
		while(1)
		{
			c= getchar();

			if(c == '\n')
			{
				line[pos] = '\0';
				return line;
			}
			else
			{
				line[pos]=c;
				
			}

			pos++;

			if(pos >=length)
			{
				length = length + line_length;

				line = realloc(line,length);
				
				if(!line)
				{
					printf("Memory Allocation Fail\n");

					exit(0);
				}
			}

		}
	}

//	printf("Success\n");
	return line;
}


char** shell_get_args(char* line)
{
	int token_size = line_token_size;
	int pos = 0;

	char** token_list = (char**) malloc(sizeof(char*) * token_size);

	char* token;

	if(!token_list)
	{
		printf("Memory Allocation failed\n");
		exit(0);
	}

	token = strtok(line,line_delimiter);

	while(token!=NULL)
	{
		token_list[pos] = token;
		pos++;

		if(pos >= token_size)
		{
			token_size += line_token_size;

			token_list = (char**) realloc(token_list, token_size);

			if(!token_list)
			{
				printf("Memory Allocation failed\n");
				exit(0);
			}

		}

		
		token = strtok(NULL,line_delimiter);
	}

	token_list[pos] = NULL;

	return token_list;
}


int shell_launch(char** token_list)
{
	pid_t pid,wpid;

	int status;

	pid = fork();

	if(pid == 0)
	{
		if(execvp(token_list[0],token_list) == -1)
		{
			perror("shell");
		}

		exit(0);

	}
	else if (pid < 0)
	{
		perror("shell");
	}
	else
	{
		do{
			wpid = waitpid(pid, &status,WUNTRACED);
		}while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}


int shell_execute(char** args)
{
	int i;

	if(args[0] == NULL)
	{
		return 1;
	}

	for(i=0;i<num_builtin_func();i++)
	{
		if(strcmp(args[0],builtin_cmd[i])==0)
		{
			return (*builtin_func[i])(args);
		}

	}

	return shell_launch(args);
}


void loop()
{
	char* command;
	char **args;
	int status;
	
	getcwd(cwd,sizeof(cwd));

	do{
		printf("\033[1;32m");
		printf("%s",cwd);
		
		printf("\033[1;34m");

		printf(" $ ");

		printf("\033[0m");

		command =shell_read_line();

		args = shell_get_args(command);

		status = shell_execute(args);

		free(command);

		free(args);
	
	}while(status);

	


}


int main(int argc, char *argv[])
{
	write(0,"\x1b[2J",5);
	write(0,"\x1b[H",4);
	loop();

	return 0;
}

