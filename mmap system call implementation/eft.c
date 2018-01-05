#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include<pthread.h>
#include<stdbool.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
typedef struct trans		// struct for transaction
{
    int acc1, acc2, amt;
};
struct account			// struct for account detail
{
    int acc_no;
    int acc_id;
    int bal;
};
int main(int argc, char *argv[])
{
    char str[1000];
    int count=0, x, l, num_th, th, d,w;
    th=0;
    d=0;
    pid_t pd[num_th+1];
    if(argc <3)
    {
        printf("enter file name and number of processes \n");     // prompt user to input file name and number of processes
        goto y;
    }
    num_th= atoi(argv[2]);
    struct trans transfer[20];
   // printf("num processes: %d \n", num_th);
    FILE *fl;
    struct account acc[100];
    struct account *acc_m = mmap(acc, sizeof(acc), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,-1, 0);   //maps *acc_m area of the parent process's virtual memory to Anonymous
    char dup[1000];
    fl= fopen(argv[1], "r");				//opening file
    if (fl != NULL)
    {
        while(!feof(fl))				// until end of file or cntrl+D is pressed
        {
            fgets(str, 200, fl);			//input from file

/* start of account updates */
            if(str[0] != 'T' && (int)str[0]<58 && (int)str[0]>47 && th >0)
            {
                count++;    				//counts number of accounts
                int i;
                strcpy(dup,str);
                for (i=0; i<200; i++)
                {
                    if (str[i] == ' ')
                    {
                        str[i]='\0';
                        acc_m[count].acc_no = atoi(str);	//saves account number in struct variable acc[count].acc_no
                        acc_m[count].acc_id= count;	//saves account id in struct variable acc[count].acc_id which is same as lock array variable
                        acc_m[count].bal = atoi(dup+i+1);
                      //  printf("acc deatils %d: %d: %d:  \n", acc_m[count].acc_id, acc_m[count].acc_id, acc_m[count].bal);
                        break;
                    }
                }
            }
/*end of account updates*/
/*start of transcations details*/
            if(str[0] == 'T' && th>0)
            {
                    strcpy(dup,str+8);
                    memset(str, 0, sizeof(str));
                    int i,l, cnt;
                    cnt=0;
                    l= strlen(dup);
                    for (i= l ;i>=0; i--)
                    {
                        if(dup[i] == ' ' & cnt==0 )
                        {
                            transfer[d].amt= atoi(dup+i+1);
                            cnt++;
                            dup[i]='/0';
                            strncpy(str,dup,i);
                            memset(dup, 0, sizeof(dup));
                        }
                        if(str[i] == ' ' & cnt==1 )
                        {
                            int g,x;
                            g= atoi(str+i+1);
                            for(x=1;x<=count;x++)
                            {
                                if(acc_m[x].acc_no == g)
                                {
                                    transfer[d].acc2=acc_m[x].acc_id;
                                }
                            }
                            cnt++;
                            str[i]='/0';
                            strncpy(dup,str, i);
                            memset(str, 0, sizeof(str));
                        }
                        if(dup[i] == ' ' & cnt==2 )
                        {
                            int g,x;
                            g= atoi(dup+i+1);
                            for(x=1;x<=count;x++)
                            {
                                if(acc_m[x].acc_no == g)
                                {
                                    transfer[d].acc1=acc_m[x].acc_id;
                                }
                            }
                            cnt++;
                        }
                    }
		    //printf("number %d : transfer deatils %d: %d: %d:  \n",d, transfer[d].acc1, transfer[d].acc2, transfer[d].amt);
                    d++;   // d has no of transfers
            }
	    if(th==0){				// checking if first line of file gives number of accounts
		w= (int)str[0]-48;
		th++;	
	    }
/*end of transcations details updates*/
            memset(dup, 0, sizeof(dup));
            memset(str, 0, sizeof(str));
        }
    }
    else
    {
        printf("Unable to open the file");
	
    }
    fclose(fl);
    int p;
    pthread_mutex_t *chopstick;		//creating locks = number of accounts for each account
    chopstick = (pthread_mutex_t*) mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED| MAP_ANONYMOUS,-1,0);	    	//maps *chopstick area of the parent process's virtual memory to Anonymous
    pthread_mutexattr_t mutexAttr;		// creating mutex attributes
    pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);		// adding process sharing in mutex attributes
    for(p=1;p<=count;p++)
    {
	pthread_mutex_init(&chopstick[p], &mutexAttr);			// initalising mutex with process sharing attributes
    }
    int i, j,k;
    i=0;
    int b,s;
    k=0;
    if (num_th > d)		// if number of processes input is more than number of transfers, limit the number of processes created
    {
        num_th =d;
    }
    pd[0]=1;		//pd[0] not used for fork
    do
    {
        for(i=0; i< num_th; i++)
        {
            if(k < d)
            {
	      if(pd[i]>0){			//ensuring creation of chil in parent process only		
		pd[i+1]=fork();			// creating child
		if(pd[i+1]==0)			// executed only by child
		{
			//printf("child pid : %d,  parent pid: %d\n", getpid(), getppid());
			if(transfer[k].acc1 > transfer[k].acc2)			// checking if value of first account id to be greater than second account id 
			{
				b=transfer[k].acc1;
				s=transfer[k].acc2;
			}
			else{
				b=transfer[k].acc2;
				s=transfer[k].acc1;
			}
/* critical region started for acc[s] and acc[b] */
    			pthread_mutex_lock(&chopstick[s]);	//lock for mutual exclusion of shared variable 'acc[s]'
    			pthread_mutex_lock(&chopstick[b]);	//lock for mutual exclusion of shared variable 'acc[b]'
    			acc_m[transfer[k].acc1].bal-=transfer[k].amt;
    			acc_m[transfer[k].acc2].bal+=transfer[k].amt;
			//printf("amount in accs  %d,  %d   in child id : %d  \n", acc_m[transfer[k].acc2].bal, acc_m[transfer[k].acc1].bal, getpid());
    			pthread_mutex_unlock(&chopstick[s]);  //unlock
    			pthread_mutex_unlock(&chopstick[b]);  //unlock	
			exit(0);				// exit chil process
			//kill(getpid(), NULL);
		}
	      }
            }
	k++;
        }
	for(i=1; i<=num_th;i++)				// stop the execution of parent process untill child process specified by pd[i] (i.e. number of processes given by terminal input) are completed  ---- new process by fork in next iteration of while loop can be created only if all the child processes (i.e. number of processes given by terminal input) are completed.
	{
		waitpid(pd[i], NULL,0);
	}
    }while(k<d);

    pthread_mutexattr_destroy(&mutexAttr);		// destroy mutex attribute
    for(p=1;p<=count;p++)
    {
         pthread_mutex_destroy(&chopstick[p]);		// destroy all the mutexes
    }

    munmap(chopstick, sizeof(pthread_mutex_t));		// unmap the memory

    for(p=1;p<=count;p++)
    {
         printf("%d  %d\n",acc_m[p].acc_no, acc_m[p].bal);	//printing the result
    }
    munmap(acc_m, sizeof(acc));				// unmap the memory
y:  return 0;
}
