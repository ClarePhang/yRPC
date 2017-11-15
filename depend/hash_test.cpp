#include <stdio.h>
#include <stdlib.h>

#include "rpc_hash.h"

int main(void)
{
    int i = 1;
    int *ptr = &i;
    RPCHash hash;
    string test = "test";
    
    if(hash.empty())
        printf("first list is empty\n");
    
    hash.insert(test,(void *)ptr);
    if(hash.empty())
        printf("second list is empty\n");
    printf("first size : %d\n", hash.size());

/*
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
*/
    return 0;
}

