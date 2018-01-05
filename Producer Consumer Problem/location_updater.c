  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <sys/types.h>
  #include<pthread.h>
  #include<stdbool.h>

  int num;
  pthread_mutex_t lck;
  pthread_cond_t empty, full;
  char **buf;
  int k;
void *email_filter(void *l)
{
    char str[1000];
    int *a, in;
    in=0;
    a= l;
	fgets(str, 200, stdin);
    do
    {
        if(str[24] == '/' && str[27] == '/' && str[35] == ':' && str[38] == ',' && str[10] == ',')
	{	
            if ((str[22] == '0' && ((int)(str[23])) < 58) ||  (str[22] == '1' && ((int)(str[23])) < 51))
	    {
                if ((str[25] <51 ) ||  (str[25] == 51 && ((int)(str[26])) < 50) )
                {
                    if ((((str[33] == 50 ) && (str[34] < 53)) || (str[33] < 50 ))  && str[36] < 55)
                    {
                        pthread_mutex_lock(&lck);   // acquiring lock
                        while(num == *a)		// wait when buffer is full
                            pthread_cond_wait(&full, &lck);  
                        strcpy(buf[in], str + 9);        // send data to buffer
                        in++;					// incrementing pointer to producer
                        num++;					// incrementing number of elements in buffer
                        if(num == 1)				//signal/ waking up consumer when there is one element in buffer
                            pthread_cond_signal(&empty);
			if(in == *a)					// when buffer size is smaller than array size OR when producer pointer reaches buffer size, setting pointer to 0 (from start point)
			{
			    in=0;		
			}
                        pthread_mutex_unlock(&lck);		// release the lock
                    }
                }
	    }
	}
        memset(str, 0, sizeof(str));
    }while(fgets(str, 200, stdin));		// while ctrl + D is entered
    k=1;					// when producer thread is about to terminate changing the value to 1
    pthread_cond_signal(&empty);		//sending signal to consumer if it is waiting before terminating
    return 0;
}



  struct hash *hash_table = NULL;
  struct node {
        int key;
        char ttl[200];
        char dt[200];
        char tm[200];
	char lc[200];
        struct node *next;
  };

  struct hash {
        struct node *head;
        int count;
  };

  struct node * make_node(int key, char *ttl, char *dt, char *tm, char *lc) {
        struct node *new_nd;
        new_nd = (struct node *)malloc(sizeof(struct node));
        new_nd->key = key;
        strcpy(new_nd->dt, dt);
        strcpy(new_nd->tm, tm);
        strcpy(new_nd->ttl, ttl);
	strcpy(new_nd->lc, lc);
        new_nd->next = NULL;
        return new_nd;
  }


  void hash_insert(int key, char *ttl, char *dt, char *tm, char *lc) {
        int hash_index = key;
	struct node *pr_node;
	char tn[200], tf[200];
        if (hash_table[hash_index].count == 0)
	{
		struct node *new_nd =  make_node(key, ttl, dt, tm, lc);
		hash_table[hash_index].head = new_nd;
               	hash_table[hash_index].count = 1;
		printf("%s:%s\n",new_nd->dt, new_nd->lc);
	     	return;
	}
	else
	{
		pr_node = hash_table[hash_index].head;
		while (pr_node != NULL) {
			if (*pr_node->dt == *dt)
			{
				strcpy(tn, pr_node->tm);
				strcpy(tf, tm);
				if(tf[0] > tn[0] )
				{
					if(tf[0] == tn[0] && tf[1] > tn[1] )
					{
						if(tf[1] == tn[1] && tf[3] > tn[3] )
						{
							if(tf[3] == tn[3] && tf[4] > tn[4])
							{
			//					printf("time is greater than earlier");
							}
						}
					}
				}
				else
				{
					strcpy(pr_node->tm, tm);
					strcpy(pr_node->lc, lc);
					strcpy(pr_node->ttl, ttl);
					printf("%s:%s\n",pr_node->dt, pr_node->lc);
				}

			}
			else
			{
				struct node *new_nd =  make_node(key, ttl, dt, tm, lc);
        			new_nd->next = (hash_table[hash_index].head); /* adding new node */
        			hash_table[hash_index].head = new_nd;  /* change the head and no of nodes after chaining*/
       				hash_table[hash_index].count++;
        			return;
			}

                       	pr_node = pr_node->next;
		}
	}
  }


  void delete_hash(int key) {
        int hash_index = key;
        struct node *temp, *pr_node;
        pr_node = hash_table[hash_index].head;
        temp = pr_node;
        while (pr_node != NULL) {
                if (pr_node->key == key) {
			printf("%s:None\n",pr_node->dt);
                        if (pr_node == hash_table[hash_index].head)
                                hash_table[hash_index].head = pr_node->next;
                        else
                                temp->next = pr_node->next;
                        hash_table[hash_index].count--;
                        free(pr_node);
                        break;
                }
                temp = pr_node;
                pr_node = pr_node->next;
        }
        return;
  }

void *calendar_filter(void *l)
{
    hash_table = (struct hash *)calloc(10, sizeof (struct hash));
	char title[100], tim[10], dat[11], loc[11];
	char str[1000];
	int key = 0, x, *a, out;
	a=l;
	int c=0;
	out=0;
	while(num>0 || k==0)
    	{
        	pthread_mutex_lock(&lck);       //acquiring lock
        	while(num == 0 && k==0)		// wait if buffer is empty and producer is producing
		{            
			pthread_cond_wait(&empty, &lck);
		}	
		if (num > 0)					// get item from buffer only if there is item in it (else it will take old value from buffer pointer 'out')
		{
        		strcpy(str, buf[out]);
		}
        	out++;						// incrementing pointer to consumer
        	num--;						// decrementing the number of items in buffer
		if(out == *a)					// when buffer size is smaller than array size OR when consumer pointer reaches buffer size, setting pointer to 0 (from start point of array data structure)				
		{
	    	out=0;
		}
	
        	if(num == *a-1)					// signal/waking up producer when there is 1 space in buffer
		{            
			pthread_cond_signal(&full);
		}        
		pthread_mutex_unlock(&lck);			// release lock
        	key=0;
		for(x=0; x< 10; x++)
		{
			title[x] = str[x+2];
		}
		title[x]= '\0';
		for(x=0; x< 10; x++)
		{
			dat[x] = str[x+13];
		}
		dat[x]= '\0';
		for(x=0; x< 5; x++)
		{
			tim[x] = str[x+24];
		}
		tim[x]= '\0';
		for(x=0; x< 10; x++)
		{
			loc[x] = str[x+30];
		}
		loc[x]= '\0';
		for(x=0; x< strlen(dat); x++)
		{
			key = key + (int)(dat[x]);
		}
		key = key % 10;
		if (str[0] == 'C' || str[0] == 'X')
		{
			hash_insert(key, title, dat, tim, loc);
		}
		if (str[0] == 'D')
		{
			delete_hash(key);
		}
	memset(str, 0, sizeof(str));
	}    
}

int main(int argc, char *argv[])
{
	int p,i ;
	if(argc < 2)
	{
		printf("Enter the buffer size\n");		//prompting user to enter buffer size
		goto d;
	}
	p= atoi(argv[1]);
	if(p < 1)
	{
		printf("buffer size should be a positive integer\n");	// buffer size should be positive integer
		goto d;							// else go to end of program
	}
	num=0;
	k=0;
	buf=malloc(p*sizeof(char*));					// dynamically allocating 2D array of string of size (buffer size X 100)
	for(i=0; i<p; i++)
	{
		buf[i]= malloc(100*sizeof(char));
	}
	pthread_t thread1, thread2;
 	pthread_mutex_init(&lck, NULL);
   	pthread_cond_init (&empty, NULL);
	pthread_cond_init (&full, NULL);
	pthread_create(&thread1, NULL, email_filter, (void *) &p);
  	pthread_create(&thread2, NULL, calendar_filter, (void *) &p);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_mutex_destroy(&lck);
	pthread_cond_destroy(&empty);
	pthread_cond_destroy(&full);
	pthread_exit(NULL);
d:	return 0;
}
