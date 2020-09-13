//Avinash Aryal 1001727418 
/*  Some example code and prototype - 
    contains many, many problems: should check for return values 
    (especially system calls), handle errors, not use fixed paths,
    handle parameters, put comments, watch out for buffer overflows,
    security problems, use environment variables, etc.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>
#define MAXFILE 1024

//Typedef to get fileinfo
typedef struct info
{
	char name[NAME_MAX];
	long size;
	char modif[25];
	

} info;
/* 
	The function is used to check if the given command is directory or file
	refernce: https://man7.org/linux/man-pages/man2/stat.2.html
*/
int isdir(const char *path)
{
	struct stat buff;
	stat(path,&buff);
	switch(buff.st_mode & S_IFMT)
	{
		case S_IFDIR:  return 1;
		case S_IFREG:  return 2;
		
	}
	return 0;	
}
/* This is the function do get file info like size and time
reference:https://man7.org/linux/man-pages/man2/stat.2.html
*/
void get_info(const char *path,info files[MAXFILE],int n)
{
	struct stat sb;
	stat(path,&sb);
	strcpy(files[n].modif,ctime(&sb.st_mtime));
	strftime(files[n].modif, 24, "%m-%d-%y", localtime(&(sb.st_ctime)));// Reference: https://stackoverflow.com/questions/10446526/get-last-modified-time-of-file-in-linux
	files[n].size=sb.st_size;
}
//refernce:https://stackoverflow.com/questions/6105513/need-help-using-qsort-with-an-array-of-structs
int comparesize(const void *a, const void *b)
{
	info *c=(info *) a;
	info *d=(info *) b;
	return (c->size -d->size);
}	
int comparename(const void *a, const void *b)
{
	info *c=(info *) a;
	info *d=(info *) b;
	return (strcmp(c->name,d->name));
	
}
int comparedate(const void *a, const void *b)
{
	info *c=(info *) a;
	info *d=(info *) b;
	return(strcmp(c->modif,d->modif));
}
//Function to print array
void print_names(info filedir[MAXFILE],int n,int isfile)
{
	char k='a';
	int c=0;
	if(!isfile)
		printf("Printing Directory:\n");
	else
		printf("Printing file\n");
	printf("%3s.  %20s %5s %15s\n", "SN","Name","Size","DateModified");
	do
	{
		if((c==0 || c % 5) !=0)
		{
			if(!isfile)
				printf("%3d.  %20s %5.2f kb %15s \n",c++ ,filedir[c].name,(float)filedir[c].size/1024,filedir[c].modif);
			else
				printf("%3d.  %20s %5.2f kb %15s \n",c++ ,filedir[c].name,(float)filedir[c].size/1024,filedir[c].modif);
		}
		else if (( c % 5 ) == 0) 
		{
			printf( "Hit N for Next, P for previous or A to print all. (Small and capital both accepted)\n" );
			k = getchar( );
			k=(char)tolower(k);
			getchar();
			while(k!='n' && k!='p' && k!='a')
			{
				printf("Wrong input try again\n");
				k=getchar( );
				k=(char)tolower(k);
				getchar();
				
			}
			if(k=='n')
			{
				if(!isfile)
					printf("%3d.  %20s %5.2f kb %15s \n",c++ ,filedir[c].name,(float)filedir[c].size/1024,filedir[c].modif);
				else
					printf("%3d.  %20s %5.2f kb %15s \n",c++ ,filedir[c].name,(float)filedir[c].size/1024,filedir[c].modif);	
			}
			else if(k=='p')
			{
				c-=10;
				if(c<0)
					c=0;
				if(!isfile)
					printf("%3d.  %20s %5.2f kb %15s \n",c++ ,filedir[c].name,(float)filedir[c].size/1024,filedir[c].modif);
				else
					printf("%3d.  %20s %5.2f kb %15s \n",c++ ,filedir[c].name,(float)filedir[c].size/1024,filedir[c].modif);	
			}
			else if(k=='a')
				while(c<n)
				{
					if(!isfile)
						printf("%3d.  %20s %5.2f kb %15s \n",c++ ,filedir[c].name,(float)filedir[c].size/1024,filedir[c].modif);
					else
						printf("%3d.  %20s %5.2f kb %15s \n",c++ ,filedir[c].name,(float)filedir[c].size/1024,filedir[c].modif);
				}
		}
	}
	while(c<n);
	printf( "-----------------------------------------\n" );
}

int main(int argc, char *argv[]) 
{
	pid_t child;
	DIR * d;
	struct dirent * de;
	
	int i, c, k;
	char s[256], cmd[256];
	time_t t;
	
	info dirinfo[MAXFILE];
	info fileinfo[MAXFILE];
	// check command line arguments
	if(argc==2)
		if(isdir(argv[1])==1)
			chdir(argv[1]);
	while (1) 
	{

		int dirno=0;
		int fileno=0;
		t = time( NULL );
		printf( "Time: %s\n", ctime( &t ));
		printf("-----------------------------------------------\n" );

		getcwd(s, 200);
		printf( "\nCurrent Directory: %s \n", s);

		d = opendir( "." );
		
		while ((de = readdir(d)))
		{
			if ((de->d_type) & DT_DIR)
			{ 
				//printf( " ( %d Directory:  %s ) \n", c++, de->d_name);
				
				strcpy(dirinfo[dirno].name,de->d_name); //copy name to the struct
				get_info(de->d_name,dirinfo,dirno); //get info of file
				dirno++;
			}	  
		}
		
		qsort(dirinfo,dirno,sizeof(dirinfo[0]),comparename);
		print_names(dirinfo,dirno,0);
		closedir( d );
		
		//Open directory for files
		d = opendir( "." );
		                   
		while ((de = readdir(d)))
		{                  
			if (((de->d_type) & DT_REG))  
			{
				//printf( " ( %d File:  %s ) \n", c++, de->d_name);
				strcpy(fileinfo[fileno].name,de->d_name);
				get_info(de->d_name,fileinfo,fileno);
				fileno++;
			}
		}
		qsort(fileinfo,fileno,sizeof(fileinfo[0]),comparename);
		print_names(fileinfo,fileno,1);
		closedir( d );
		
		printf("Operation:\nE Edit\nR Run\nC Change Directory\nS Sort Directory listing\nQ Quit\n");
		c = getchar( ); getchar( );
		c=tolower(c);
		switch (c) 
		{
        	case 'q': 
        		exit(0); /* quit */
			case 'e': 
				printf( "Edit what? Input fileno or name:" );
				scanf( "%s", s );
				getchar( );
				strcpy( cmd, "pico ");
				// Check if the command is number
				if(atoi(s))
				{
					if(atoi(s) <0 || atoi(s)>fileno)
						printf("Wrong input\n");
					else
						strcat(cmd,fileinfo[atoi(s)].name);
				}
				else
					strcat( cmd, s );
				system( cmd );
				break;
			case 'r': 
				printf( "Run what? Please enter ./ and name of the programme:" );
				scanf( "%s", cmd );
				getchar();
				system( cmd );
				break;
			case 'c': 
				printf( "Change To? Input directory name,path or dirno:" );
				scanf( "%s", cmd );
				getchar();
				if(atoi(cmd))
				{
					if(atoi(cmd) <0 || atoi(cmd)>dirno)
						printf("Wrong input\n");
					else
						strcpy(cmd,dirinfo[atoi(cmd)].name);
				}
				//check if the input is directory
				if(isdir(cmd)==1)
					chdir( cmd );  
				else
					printf("Wrong Directory  input\n"); 
			
				break; 
			case 's':
				printf("Enter 'D to sort by date' or 'S' to sort by size \n");
				k=getchar( );getchar();
				if(k=='D' || k=='d')
				{
					//strcpy(cmd,"ls -S");
					//system(cmd);
					
					qsort(dirinfo,dirno,sizeof(dirinfo[0]),comparedate);
					qsort(fileinfo,fileno,sizeof(fileinfo[0]),comparedate);

					print_names(dirinfo,dirno,0);
					print_names(fileinfo,fileno,1);
				}
				else if(k=='S' || k=='s')
				{
					//strcpy(cmd,"ls -hlt");
					//system(cmd);
					qsort(dirinfo,dirno,sizeof(dirinfo[0]),comparesize);
					qsort(fileinfo,fileno,sizeof(fileinfo[0]),comparesize);
					print_names(dirinfo,dirno,0);
					print_names(fileinfo,fileno,1);
				
				}
				else
					printf("Wrong input\n");
				break;
			default:
				printf("Wrong Input\n");
		}

	}
}
