/* int_hash.cpp
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-09
 * Author: zhangqiyin/Konishi
 * Email : zhangqiyin@hangsheng.com.cn
 */
#include <stdio.h>

#include "int_hash.h"

INTHash::INTHash()
{
    int_map.clear();
}

INTHash::~INTHash()
{
    int_map.clear();
}

void INTHash::print(void)
{
    printf("Hash Table:\n");
    for(it = int_map.begin(); it != int_map.end(); it++)
        printf("%d => %p\n", it->first, it->second);
}

int INTHash::size(void)
{
    return int_map.size();
}

void INTHash::clear(void)
{
    int_map.clear();
}

bool INTHash::empty(void)
{
    return int_map.empty();
}

void *INTHash::find(int &key)
{
    it = int_map.find(key);
    return (it == int_map.end()) ? NULL : it->second;
}

int INTHash::remove(int &key)
{
    it = int_map.find(key);
    if(it == int_map.end())
        return 0;
    int_map.erase(it);
    return 0;
}

int INTHash::insert(int &key, void *value)
{
    it = int_map.find(key);
    if(it != int_map.end()) // exsit
        return 1;

    int_map[key] = value;
//    int_map.insert(std::pair<int, void *>(key, value));
    return 0;
}

int INTHash::change(int &key, void *value)
{
    it = int_map.find(key);
    if(it == int_map.end()) // not exsit
        return -1;
    int_map[key] = value;
    return 0;
}

