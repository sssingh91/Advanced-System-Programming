#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include<pthread.h>
#include<stdbool.h>
#include<unistd.h>
pthread_t  *philosopher ;
pthread_mutex_t *forks;


typedef struct trans  		// struct for sending argument to philosopher thread
{
    int acc1, acc2, amt;
};

struct account			// struct for account detail
{
    int acc_no;
    int acc_id;
    int bal;
}acc[100];

void *worker_func(void *l)	// worker function for each transfer for threading with 'struct trans' as an argument
{
    struct trans ll = *((struct trans*)(l));
    int ak1, ak2,b, s, amtt,i;
    ak1=ll.acc1;		// saving account id of first account to 'ak1';
    ak2=ll.acc2;		// saving account id of second account to 'ak2';
    amtt=ll.amt;		// saving acmout to be transferred to 'amtt';
    if(ak1<1 || ak2 <1)
    {
        goto u;
    } 


/* if-else statement to store smaller account_id value to variable 's' and larger account id value to variable 'b' to prevent deadlock and circular wait*/


    if(ak1>=ak2)		// checking if value of first account id to be greater than second account id 
    {
        b=ak1;
        s=ak2;
    }
    else{
        b=ak2;
        s=ak1;
    }


/* critical region started for acc[s] and acc[b] */
    pthread_mutex_lock(&forks[s]);  //lock for mutual exclusion of shared variable 'acc[s]'
    pthread_mutex_lock(&forks[b]);  //lock for mutual exclusion of shared variable 'acc[b]'
    acc[ak1].bal-=amtt;
    acc[ak2].bal+=amtt;
    pthread_mutex_unlock(&forks[s]);
    pthread_mutex_unlock(&forks[b]);
/* critical region ended for acc[s] and acc[b] */


u:  return;
}

int main(int argc, char *argv[])
{
    char str[1000];
    int count=0, x, l, num_th, th, d;
    th=0;
    d=0;
    if(argc <3)		// prompt user to input file name and number of threads
    {
        printf("enter file name and number of threads \n");
        goto y;
    }

    num_th= atoi(argv[2]);
    struct trans transfer[100];
    philosopher = malloc(num_th * sizeof(pthread_t));   //dynamically allocating memory to thread array
    FILE *fl;
    char dup[1000];
    fl= fopen(argv[1], "r");				//opening file
    if (fl != NULL)
    {
        while(!feof(fl))				// until end of file or cntrl+D is pressed
        {
            fgets(str, 200, fl);			//input from file
	    int f=(int)str[0];


/* start of account updates */
            if(str[0] != 'T' && (int)str[0]<58 && (int)str[0]>47)
            {
                count++;    				//counts number of accounts
            
                int i;
                strcpy(dup,str);
                for (i=0; i<200; i++)
                {
                    if (str[i] == ' ')
                    {
                        str[i]='\0';
                        acc[count].acc_no = atoi(str);	//saves account number in struct variable acc[count].acc_no
                        acc[count].acc_id= count;	//saves account id in struct variable acc[count].acc_id which is same as lock array variable
                        acc[count].bal = atoi(dup+i+1);
                        break;
                    }
                }

            }


/*end of account updates*/

/*start of transcations details*/
            if(str[0] == 'T')
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
                                if(acc[x].acc_no == g)
                                {
                                    transfer[d].acc2=acc[x].acc_id;
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
                                if(acc[x].acc_no == g)
                                {
                                    transfer[d].acc1=acc[x].acc_id;
                                }
                            }
                            cnt++;
                        }
                    }
                    d++;   // d has no of transfers
            }
/*end of transcations details updates*/
            memset(dup, 0, sizeof(dup));
            memset(str, 0, sizeof(str));
        }
    }
    else
    {
        printf("Unable to open the file\n");
	goto y;
    }
    fclose(fl);
    forks = malloc((count+1) * sizeof(pthread_mutex_t));  // creating forks/locks = number of accounts for each account
    int p;
    for(p=1;p<=count;p++)
    {
         pthread_mutex_init(&forks[p],NULL);			// initialising lock
    }
    int i, j,k;
    i=0;
    k=0;
    if (num_th > d)						// if number of threads input is more than number of transfers, limit the number of threads created
    {
        num_th =d;
    }
    do
    {
        for(i=0; i< num_th; i++)
        {
            if(k < d)
            {
                pthread_create(&philosopher[i], NULL, worker_func, &transfer[k]); // creating thread i.e. creating philosopher
                k++;
            }
        }
        for (j=0; j<num_th; j++)
        {
            pthread_join(philosopher[j], NULL);
        }
    }while(k<d);
    for(p=1;p<=count;p++)
    {
         pthread_mutex_destroy(&forks[p]);
    }
    int s;
    for(s=1;s<=count;s++)
    {
        printf("%d    %d \n", acc[s].acc_no, acc[s].bal);
    }
    pthread_exit(NULL);
y:  return 0;
}
