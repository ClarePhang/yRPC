/* uint_hash.cpp
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-09
 * Author: Konishi
 * Email : konishi5202@163.com
 */
#include <stdio.h>

#include "uint_hash.h"

#define K_DEBUG   printf
#define K_INFO    printf
#define K_WARN    printf
#define K_ERROR   printf

INTHash::INTHash()
{
    uint_map.clear();
}

INTHash::~INTHash()
{
    uint_map.clear();
}

void INTHash::print(void)
{
    K_INFO("Hash Table:\n");
    for(it = uint_map.begin(); it != uint_map.end(); it++)
        K_INFO("%d => %p\n", it->first, it->second);
}

int INTHash::size(void)
{
    return uint_map.size();
}

void INTHash::clear(void)
{
    uint_map.clear();
}

bool INTHash::empty(void)
{
    return uint_map.empty();
}

void *INTHash::find(unsigned int &key)
{
    it = uint_map.find(key);
    return (it == uint_map.end()) ? NULL : it->second;
}

int INTHash::remove(unsigned int &key)
{
    it = uint_map.find(key);
    if(it == uint_map.end())
        return 0;
    uint_map.erase(it);
    return 0;
}

void INTHash::remove(void *value)
{
    for(it = uint_map.begin(); it != uint_map.end(); it++)
    {
        if(value == it->second)
        {
            uint_map.erase(it->first);
            break;
        }
    }
}

int INTHash::insert(unsigned int &key, void *value)
{
    it = uint_map.find(key);
    if(it != uint_map.end()) // exsit
        return 1;

    uint_map[key] = value;
//    uint_map.insert(std::pair<int, void *>(key, value));
    return 0;
}

int INTHash::change(unsigned int &key, void *value)
{
    it = uint_map.find(key);
    if(it == uint_map.end()) // not exsit
        return -1;
    uint_map[key] = value;
    return 0;
}

