//Avinash Aryal 1001727418
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <wait.h>
#include <pthread.h>




pthread_mutex_t mutex;

// A typedef to store the range value
typedef struct ranges
{
	int start;
	int end;
}ranges;
//A typedf struct to read data and from file
typedef struct data
{
	char detail[500];
	double latitude;
}data;

typedef struct 
{
	data *earthquake;
	int start;
	int end;
	
}argument;

void parray(data *earthquake,int begin,int count,int what);
int readline(FILE *fp)
{
	int count=0;
	int buffer_size=500;
	char buffer[buffer_size];
	int maxsize=0;;
	while(fgets(buffer,buffer_size-1,fp)!=NULL)
		count++;
	return count;
}

//ref: stackoverflow provided by porfesspr i.e https://stackoverflow.com/questions/5656530/how-to-use-shared-memory-with-linux-in-c
void* create_shared_memory(size_t size) {
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  return mmap(NULL, size, protection, visibility, -1, 0);
}

// This function read the file and populate the array.
void readlatitude(FILE *fp,data *earthquake)
{
	fseek(fp,0,0);
	int index=0;
	int buffer_size=500;
	char buffer[buffer_size];
	fgets(buffer,buffer_size-1,fp);  //Throw out the first line of file
	char *token;
	while(fgets(buffer,buffer_size-1,fp))
	{
		strcpy(earthquake[index].detail,buffer);
		token=strtok(buffer,",");// get the token string uptp first comma
		token=strtok(NULL,",");//get sedond token
		earthquake[index++].latitude=atof(token); // convert the strtok to floating point number
		
	}

}
//ref:https://www.geeksforgeeks.org/bubble-sort/
void swap(data *xp, data *yp)
{
	data temp=*xp;
	*xp=*yp;
	*yp=temp;
}

void *bubblesort(void *a)
{
	argument fun_argument= *(argument *) a;
	data *earthquake= fun_argument.earthquake;
	int start= fun_argument.start;
	int end= fun_argument.end;
	int i,j;
	//pthread_mutex_lock (&mutex);

	for(i=start;i<end-1;i++)
	{
		for(j=start;j<end-1;j++)
		{
			if(earthquake[j].latitude>earthquake[j+1].latitude)
			{
				//pthread_mutex_lock (&mutex);
				swap(&earthquake[j],&earthquake[j+1]);
				//pthread_mutex_unlock (&mutex);
			}
		}
	}
	//pthread_mutex_unlock (&mutex);
	//printf("i am thread printing from %d to %d \n",start,end );
	//parray(earthquake,start,end,2);

	pthread_exit(0);

}

//The function takes the data struct and print the values
void parray(data *earthquake,int begin,int count,int what)
{
	int i=begin;
	for(i=begin;i<count;i++)
	{
		if(what==1)
			printf("%s  ", earthquake[i].detail);
		else
			printf("%f\n",earthquake[i].latitude );
	}
	
}

//ref:https://www.interviewbit.com/tutorial/merge-sort-algorithm/
//This function merge the sorted array.
void merge(data *earthquake, int firstbegin, int firstend, int secondend)
{
	data *temp =malloc(sizeof(data)*(secondend-firstbegin));
	//double temp[secondend-firstbegin];
	int i=firstbegin, j=firstend,k=0;
	// Compare two values and put small into temp array
	while(i<firstend && j<secondend)
	{
		if(earthquake[i].latitude<=earthquake[j].latitude)
		{

			temp[k]=earthquake[i];
			k++; 
			i++;
		}
		else
		 {

			temp[k] = earthquake[j];
			k++;
			j++;
		}
	
	}
	
	// add elements left in the first interval 
	while(i < firstend) 
	{

		temp[k] = earthquake[i];
		k++; 
		i++;
	}

	// add elements left in the second interval 
	while(j <secondend) 
	{
		temp[k] = earthquake[j];
		k++;
		j++;
	}

	// copy temp to original interval
	for(i =0; i <secondend; i++)
	{
		earthquake[i]=temp[i];
	}
	free(temp);
	
}
int main(int args, char  *argv[])
{
	FILE *fp=NULL;
	fp=fopen("all_month.csv","r+");
	if(fp==NULL)
	{
		printf("Wouldn't open file\n");
		exit(-1);
	}
	time_t begin,end;
	int count=readline(fp)-1;

	//struct timeval start, end;

	data *earthquake= create_shared_memory(sizeof(data)*count);
	readlatitude(fp,earthquake);
	int nothreads;
	printf("How many threads you want to use: ");
	scanf("%d",&nothreads);
	if(nothreads<=0)
	{
		printf("processes <=0. Countinuing with 1 process\n");
		nothreads=1;

	}
	// get the ranges array values based on the process
	//ranges *range= malloc(sizeof(ranges)*nothreads);
	ranges *range =create_shared_memory(sizeof(ranges)*nothreads);
	int middle=count/nothreads;
	int i;
	range[0].start=0;
	range[0].end=middle;
	// populate all the ranges that will be passed into the process. 
	for(i=1;i<nothreads;i++)
	{
		range[i].start=range[i-1].end;
		range[i].end=range[i-1].end+middle;
		if(nothreads==(i+1))
			range[i].end+=count%nothreads;
	}
	printf("Count: %d\n",count);
	begin=time(NULL);
	
	/*
	if(nothreads==1)
		//bubblesort(earthquake,0,count);
		printf("\n");
	else
	{
	*/
		pthread_t threads[nothreads];
		argument *fun_argument=malloc(sizeof(argument)*nothreads);
		
		for(i=0;i<nothreads;i++)
		{
			//fun_argument=malloc(sizeof(earthquake));
			fun_argument[i].earthquake= earthquake;
			fun_argument[i].start=range[i].start;
			fun_argument[i].end=range[i].end;
		}
		
		for(i=0;i<nothreads;i++)
		{
			pthread_create(&threads[i], NULL, bubblesort, (void *) &fun_argument[i]);
			//printf("I am at this %d thread from main range is %d to %d \n",i, range[i].start,range[i].end );

			//parray(earthquake,range[i].start,range[i].end,2);
		}
		
		void *status;
		
		for(i=0;i<nothreads;i++)
		{
			pthread_join(threads[i], &status);
		}
		free(fun_argument);
		
	//}
	for(i=0;i<nothreads-1;i++)
		merge(earthquake,0,range[i].end,range[i+1].end);// merge the array in proper format
	if(args>1)
	{
		if(strcmp(argv[1],"print")==0)
			parray(earthquake,0,count,1);
		else if(strcmp(argv[1],"lat")==0)
			parray(earthquake,0,count,2);
	}
	end=time(NULL);
	
	time_t diff= end -begin;

	printf("Begin time: %s\n",ctime(&begin) );
	printf("End time; %s\n", ctime(&end) );

	printf("The time taken is %ld\n",diff);

	
	
	
	
	return 0;
}

