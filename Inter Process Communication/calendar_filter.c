  #include <unistd.h>
  #include <sys/types.h>
  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>

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


void main()
{
	hash_table = (struct hash *)calloc(10, sizeof (struct hash));
	char title[100], tim[10], dat[10], loc[10];
	char str[1000];
	struct node *ret_node;
	int key = 0, x, l;
	FILE *fl;
	fl= fopen("datadata.txt.txt", "r");
	fgets(str, 200, stdin);
	do
        {
		
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
	}while(fgets(str, 200, stdin));			// while ctrl + D is pressed

    exit(0);
}


