/* nil_pointer_list.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-14
 * Author: Konishi
 * Email : konishi5202@163.com
 */
#ifndef NIL_POINTER_LIST_H__
#define NIL_POINTER_LIST_H__
#include <list>

class NILPointerList
{
public:
    NILPointerList();
    ~NILPointerList();

public:
    int size(void);
    void clear(void);
    bool empty(void);
    void *find(void *addr);
    int insert(void *addr);
    int remove(void *addr);

private:
    list<void *> nilp_list;
    list<void *>::iterator it;
};

#endif // NIL_POINTER_LIST_H__

