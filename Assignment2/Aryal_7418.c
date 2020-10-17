//Avinash Aryal 1001727418
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/mman.h>
#include <wait.h>

typedef struct ranges
{
	int start;
	int end;
}ranges;
int readline(FILE *fp)
{
	int count=0;
	int buffer_size=500;
	char buffer[buffer_size];
	while(fgets(buffer,buffer_size-1,fp)!=NULL)
		count++;
	return count;
}
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
void readlatitude(FILE *fp,double *latitude)
{
	fseek(fp,0,0);
	int index=0;
	int buffer_size=500;
	char buffer[buffer_size];
	fgets(buffer,buffer_size-1,fp);
	char *token;
	while(fgets(buffer,buffer_size-1,fp))
	{
		token=strtok(buffer,",");
		token=strtok(NULL,",");
		latitude[index++]=atof(token);
		
	}

}
//ref:https://www.geeksforgeeks.org/bubble-sort/
void swap(double *xp, double *yp)
{
	double temp=*xp;
	*xp=*yp;
	*yp=temp;
}
void bubblesort(double latitude[], int start, int end)
{
	int i,j;
	for(i=start;i<end-1;i++)
		for(j=start;j<end-1;j++)
			if(latitude[j]>latitude[j+1])
				swap(&latitude[j],&latitude[j+1]);

}
void parray(double *latitude,int begin,int count)
{
	int i=begin;
	for(i=begin;i<count;i++)
		printf("%f  ", latitude[i] );
}

//ref:https://www.interviewbit.com/tutorial/merge-sort-algorithm/
//This function merge the sorted array.
void merge(double latitude[], int firstbegin, int firstend, int secondend)
{
	double *temp =malloc(sizeof(double)*(secondend-firstbegin));
	//double temp[secondend-firstbegin];
		int i=firstbegin, j=firstend,k=0;
	// Compare two values and put small into temp array
	while(i<firstend && j<secondend)
	{
		if(latitude[i]<=latitude[j])
		{
			temp[k] = latitude[i];
			k++; 
			i++;
		}
		else
		 {
			temp[k] = latitude[j];
			k++;
			j++;
		}
	
	}
	
	// add elements left in the first interval 
	while(i < firstend) 
	{
		temp[k] = latitude[i];
		k++; 
		i++;
	}

	// add elements left in the second interval 
	while(j <secondend) 
	{
		temp[k] = latitude[j];
		k++;
		j++;
	}

	// copy temp to original interval
	for(i =0; i <secondend; i++)
	{
		latitude[i] = temp[i];
	}
	free(temp);
	
}
int main()
{
	FILE *fp=fopen("all_month.csv","r+");
	time_t begin,end;
	int count=readline(fp)-1;

	double *latitude= create_shared_memory(sizeof(double)*count);
	readlatitude(fp,latitude);
	int noprocess;
	printf("How many process you want to use: ");
	scanf("%d",&noprocess);
	if(noprocess<=0)
	{
		printf("processes <=0. Countinuing with 1 process\n");
		noprocess=1;

	}

	ranges *range= malloc(sizeof(ranges)*noprocess);
	int middle=count/noprocess;
	int i;
	range[0].start=0;
	range[0].end=middle;
	for(i=1;i<noprocess;i++)
	{
		range[i].start=range[i-1].end;
		range[i].end=range[i-1].end+middle;
		if(noprocess==(i+1))
			range[i].end+=count%noprocess;
	}
	printf("Count: %d\n",count);
	for(i=0;i<noprocess;i++)
	{
		printf("Start: %d End: %d\n", range[i].start, range[i].end );
	}
	begin=time(NULL);

	for(i=0;i<noprocess;i++)
	{
		if(noprocess==1)
		{
			bubblesort(latitude,range[i].start,range[i].end);
		}
		else if(fork()==0)
		{
			bubblesort(latitude,range[i].start,range[i].end);
			//printf("\nI am child %d\n", getpid() );
			//parray(latitude,range[i].start,range[i].end);
			//printf("\nI am ending %d\n",getpid() );
			exit(0);
		}

	}
	
	for(i=0;i<noprocess;i++)
		wait(NULL);
	for(i=0;i<noprocess-1;i++)
		merge(latitude,0,range[i].end,range[i+1].end);

	end=time(NULL);
	time_t diff= end -begin;
	printf("Begin time: %s\n",ctime(&begin) );
	printf("End time; %s\n", ctime(&end) );

	printf("The time taken is %ld\n",diff);
	
	
	
	return 0;
}