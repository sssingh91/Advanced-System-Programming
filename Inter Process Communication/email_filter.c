  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>

int main()
{
    char title[100], tim[10], dat[10];
    char str[1000], *b[1000];
    int key = 0, x, l;
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
						*b = str + 9;                            			
						printf("%s",*b);

                        		}
                    		}
                	}
            	}
            	memset(str, 0, sizeof(str));
	}while(fgets(str, 200, stdin));

    return 0;
}

