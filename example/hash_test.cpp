#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>

#include "rpc_hash.h"

using namespace std;

int main(void)
{
    int i = 1;
    RPCHash hash;
    
    stringstream ss;
    string name, test;
    
    if(hash.empty())
        printf("first list is empty\n");
    
    hash.insert(test,(void *)&i);
    if(hash.empty())
        printf("second list is empty\n");
    printf("first size : %d\n", hash.size());

    
    for(i = 1; i < 10000; i++)
    {
        ss.clear();
        ss << i;
        ss >> test;
        name = "test" + test;
        //cout << name << endl;
        hash.insert(name,(void *)(&i));
    }
    
    printf("second size : %d\n", hash.size());

    name = "test6000";
    if(hash.find(name))
        printf("%s is exsit.\n",name.c_str());

    printf("name addr %p\n",&name);
    hash.change(name,(void *)(&name));
    printf("find addr %p\n",hash.find(name));
    
    hash.remove(name);
    printf("third size : %d\n", hash.size());
    
    //hash.print();
    hash.clear();
    printf("five size : %d\n", hash.size());
    
    return 0;
}

