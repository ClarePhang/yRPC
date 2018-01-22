/* uint_hash.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-14
 * Author: Konishi
 * Email : konishi5202@163.com
 */

#ifndef UINT_HASH_H__
#define UINT_HASH_H__
#include <map>

using namespace std;

typedef map<unsigned int, void *>UINT_MAP;

class UINTHash
{
public:
    UINTHash();
    ~UINTHash();

public:
    void print(void);
    int size(void);
    void clear(void);
    bool empty(void);
    void *find(unsigned int key);
    int remove(unsigned int key);
    void remove(void *value);
    int insert(unsigned int key, void *value);
    int change(unsigned int key, void *value);
    
private:    
    UINT_MAP uint_map;
    UINT_MAP::iterator it;
};

#endif // UINT_HASH_H__

