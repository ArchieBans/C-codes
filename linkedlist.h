#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

typedef struct charval charval_t;
struct charval {
    unsigned char c;
    int           val;
    charval_t *next;
};

charval_t *new_charval(int val,unsigned char c);
charval_t *add_front(charval_t *, charval_t *);
charval_t *add_end(charval_t *, charval_t *);
charval_t *peek_front(charval_t *);
charval_t *remove_front(charval_t *);
charval_t *traverse_and_remove(charval_t *,unsigned char key);
charval_t *traverse_and_remove_int(charval_t *,int key);
charval_t *new_charlist(unsigned char c);
charval_t *make_list(charval_t *,charval_t *,unsigned char key);
charval_t *delete_at(charval_t *,int position);
charval_t *insert_at(charval_t *,charval_t*,int position);
charval_t *new_charint(int val);
int listsize(charval_t *);
int get_value_at(charval_t*,int position);
void free_charval(charval_t *);

void       apply(charval_t *, void(*fn)(charval_t *, void *), void *arg);

#endif
