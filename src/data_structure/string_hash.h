/* string_hash.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-14
 * Author: Konishi
 * Email : konishi5202@163.com
 */

#ifndef STRING_HASH_H__
#define STRING_HASH_H__
#include <map>
#include <string>

using namespace std;

typedef map<string, void *>STRING_MAP;

class StringHash
{
public:
    StringHash();
    ~StringHash();

public:
    void print(void);
    int size(void);
    void clear(void);
    bool empty(void);
    void *find(const string &key);
    int remove(const string &key);
    void remove(void *value);
    int insert(const string &key, void *value);
    int change(const string &key, void *value);

private:
    STRING_MAP string_map;
    STRING_MAP::iterator it;
};

#endif // STRING_HASH_H__

