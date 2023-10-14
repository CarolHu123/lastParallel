#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

void Mark_prime(int *isPrime,int start,int end) {
	//fprintf(stderr,"|%d-%d-%d|\n", step,start,end);
  	for (int i = 2; i * i < end; i++) {
		if (isPrime[i]) {
		    for (int j = fmax(i * 2, (start / i) * i); j<end; j+=i)
		    {
		        //fprintf(stderr,"from st-end-j: %d-%d-%d\n", start,end,j); 
		        isPrime[j] = 0;
		    }
		}
		 
    }
    /*	fprintf(stderr,"\n_______\n");
		fprintf(stderr,"\n on pro :%d \n",rank);
    	for(int i=1;i<=n;i++)
    	{
    		fprintf(stderr," %d ", isPrime[i]); 
    	}
    	fprintf(stderr,"\n_______\n");*/
   
}

int main(int argc, char *argv[]) {
    int rank, size;
    int n=atoi(argv[1]);  // 用户输入的最大数
    int *isPrime = (int*)malloc((n+3)*(sizeof(int)));
    MPI_Init(&argc, &argv); //离谱。。在init前和Finalize之后，其实仍然是并行的。试了很多种方法，还是没办法。因为是从头到尾并行的，所以isprime会不定期的初始化。。。。。
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int step = n/size;
    double start_time, end_time;
	start_time = MPI_Wtime();
    if(rank==0){
		
		for (int i = 0; i <=n ; i++) {
			isPrime[i] = 1; // 初始化为素数状态
	    }
    	isPrime[1] = 0;
		Mark_prime(isPrime,0,step);//线程0的markprime
		int *prime = (int*)malloc((n+3)*(sizeof(int)));
		for(int i=0;i<step;i++)
	    {
	    		prime[i]=isPrime[i];
	    }
		for(int i=1;i<size;i++)			//发送线程0处理完之后的isprime
	    {
			MPI_Send(isPrime,n+3,MPI_INT,i,99,MPI_COMM_WORLD);
		}
	    //MPI_Recv(isPrime,n+3,MPI_INT,MPI_ANY_SOURCE,114,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	    for(int i=1;i<size;i++)
	    {
	    	MPI_Recv(isPrime,n+3,MPI_INT,i,114,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	    	int start=i*step;
			int end=(i==(size-1))? (n+1):(i+1)*step;
	    	for(int i=start;i<end;i++)
	    	{
	    		prime[i]=isPrime[i];
	    	}
	    }	
    	    //printf("size= %d \n", size);
		end_time = MPI_Wtime();
	    printf("Elasped time is %.8f\n",end_time-start_time);
	    for(int i = 1; i <= n; i++){
			if(prime[i] == 1){
		  	  	printf("%d ",i);
			}
	    }
	    printf("\n");

    }else{
		MPI_Recv(isPrime,n+3,MPI_INT,0,99,MPI_COMM_WORLD,MPI_STATUS_IGNORE);//接受初始化好的isprime
		int start=rank*step;
		int end=(rank==(size-1))? (n+1):(rank+1)*step;//n+1为了和j配合，j不能等于end,因为end是下一个的起点，所以j<end。但是又要处理n这个数字，所以end=n+1
		Mark_prime(isPrime,start,end);
		MPI_Send(isPrime,n+3,MPI_INT,0,114,MPI_COMM_WORLD);					//发送 markprime 的isprime
    }

    MPI_Finalize();
    return 0;
}

