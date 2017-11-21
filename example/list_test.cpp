#include <stdio.h>
#include <stdlib.h>

#include "rpc_list.h"

int main(void)
{
    int i = 1;
    int *ptr = &i;
    int *tmp = NULL;
    RPCList list;

    if(list.empty())
        printf("first list is empty\n");
    
    list.insert((void *)ptr);
    if(list.empty())
        printf("second list is empty\n");
    printf("first size : %d\n", list.size());
    
    for(tmp = ptr; tmp < (ptr + 10000); tmp++)
    {
        list.insert((void *)tmp);
    }
    printf("second size : %d\n", list.size());

    if(list.find((void *)ptr))
        printf("ptr addr is exsit.\n");

    list.remove((void *)ptr);
    printf("third size : %d\n", list.size());

    printf("ptr = %p\n",ptr);
    ptr += 5000;
    tmp = (int *)list.find((void *)ptr);
    printf("tmp = %p\n", tmp);

    return 0;
}

