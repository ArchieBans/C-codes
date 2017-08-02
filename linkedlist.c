/*
 * linkedlist.c
 *
 * Based on the implementation approach described in "The Practice 
 * of Programming" by Kernighan and Pike (Addison-Wesley, 1999).
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"


void *emalloc(size_t n) {
    void *p;

    p = malloc(n);
    if (p == NULL) {
        fprintf(stderr, "malloc of %zu bytes failed", n);
        exit(1);
    }

    return p;
}


charval_t *new_charval(int val,unsigned char c) {
    charval_t *temp;

    temp = (charval_t *) emalloc(sizeof(charval_t));
    temp->c = c;
    temp->val = val;
    temp->next = NULL;

    return temp;
}
charval_t *new_charlist(unsigned char c) {
    charval_t *temp;
	temp = (charval_t *) emalloc(sizeof(charval_t));
    temp->c = c;
    temp->val = 0;
    temp->next = NULL;

    return temp;
}
charval_t *new_charint(int val) {
    charval_t *temp;
	temp = (charval_t *) emalloc(sizeof(charval_t));
    temp->c = '\0';
    temp->val = val;
    temp->next = NULL;

    return temp;
}

charval_t *add_front(charval_t *list, charval_t *cv) {
    cv->next = list;
    return cv;
}


charval_t *add_end(charval_t *list, charval_t *cv) {
    charval_t *curr;

    if (list == NULL) {
        cv->next = NULL;
        return cv;
    }

    for (curr = list; curr->next != NULL; curr = curr->next);
    curr->next = cv;
    cv->next = NULL;
    return list;
}


charval_t *peek_front(charval_t *list) {
    return list;
}


charval_t *remove_front(charval_t *list) {
    if (list == NULL) {
        return NULL;
    }

    return list->next;
}


void free_charval(charval_t *cv) {
    assert(cv != NULL);
    free(cv);
}


void apply(charval_t *list,
           void (*fn)(charval_t *list, void *),
           void *arg)
{
    for ( ; list != NULL; list = list->next) {
        (*fn)(list, arg);
    }
}
charval_t *traverse_and_remove(charval_t *list,unsigned char key)
{
   charval_t *current = list;
   charval_t *previous = NULL;
   charval_t *previous_temp = NULL;
   charval_t *temp_curr = NULL;
   int temp = 1;
   if(current == NULL) {
      return NULL;
   }
   if(current->c==key)
   {
	   return list;
   }
  for(current = list;current!=NULL;current = current->next)
  {
	 
	  if(current->c==key && temp==1)
	  {
		  
		  previous_temp = previous;
		  temp_curr = current->next;
		  temp = 0;
	  }
	  previous = current;
  }
		if(previous_temp==NULL)
		{
			charval_t *new = new_charval(0,key);
			list = add_end(list,new);
			//free(new);
		}
		else //Removing here and adding to front
		{
			charval_t* tofree = previous_temp->next;
			previous_temp->next = temp_curr;
			free(tofree);
			charval_t *new = new_charval(0,key);
			list = add_front(list,new);
			//free(new);
		}	
		
	return list;
}
charval_t *traverse_and_remove_int(charval_t *list,int key)
{
   charval_t *current = list;
   charval_t *previous = NULL;
   charval_t *previous_temp = NULL;
   charval_t *temp_curr = NULL;
   int temp = 1;
   if(current == NULL) {
      return NULL;
   }
   if(current->val==key)
   {
	   return list;
   }
  for(current = list;current!=NULL;current = current->next)
  {
	 
	  if(current->val==key && temp==1)
	  {
		  
		  previous_temp = previous;
		  temp_curr = current->next;
		  temp = 0;
	  }
	  previous = current;
  }
		if(previous_temp==NULL)
		{
			charval_t *new = new_charint(key);
			list = add_end(list,new);
		}
		else //Removing here and adding to front
		{
			charval_t* tofree = previous_temp->next;
			previous_temp->next = temp_curr;
			free(tofree);
			charval_t *new = new_charint(key);
			list = add_front(list,new);
		}	
			
		
	return list;
}
charval_t *delete_at(charval_t *list,int position)
{
	int i=0;
	charval_t *curr = list;
   charval_t *previous = NULL;
   charval_t *previous_temp = NULL;
   
   if(position==0)
   {
	   return list->next;
   }
   else if(position==1)
   {
	   previous = curr;
	   curr = curr->next;
	   previous->next = curr->next;
	   free(curr);
	   return list;
   }
   else
   {
   for(i=0;i<position;i++)
   {
	   previous = curr;
	   curr = curr->next;
	}
   previous_temp = previous->next;
   previous->next = curr->next;
   free(previous_temp);
   }
   
   return list;
}   
   charval_t *insert_at(charval_t *list,charval_t* temp,int position)
   {
	 int i=0;
	charval_t *curr = list;
	charval_t *previous = NULL;
	
     if(position==0)
   {
	   temp->next = list;
	   return list;
   }
   else if(position==1)
   {
	   previous = curr;
	   curr = curr->next;
	   previous->next = temp;
	   temp->next = curr;
	   return list;
   }
   else
   {
   for(i=0;i<position;i++)
   {
	   previous = curr;
	   curr = curr->next;
	   
   }
 
   previous->next = temp;
   temp->next = curr;
 
   }
   return list;
   }
charval_t *make_list(charval_t *list,charval_t *temp_list,unsigned char key)
{
	charval_t *temp = temp_list;
	int count=129;
	int save_count = 0;
	if(temp==NULL)
	{
		return list;
	}
	if(temp->c==key)
	{
		charval_t *temp_value = new_charint(count);
		list  = add_end(list,temp_value);
		//free(temp_value);
		return list;
	}	
	for(temp = temp_list;temp!=NULL;temp=temp->next)
	{
		if(temp->c==key)
		{
			save_count = count;
		}
		count = count+1;
	}
	if(save_count==0)
	{
		
		charval_t *temp_value = new_charval(count,key);
		list  = add_end(list,temp_value);
		//free(temp_value);
		return list;
	}
	charval_t *temp_value = new_charint(save_count);
	list  = add_end(list,temp_value);
	//free(temp_value);
	//free_charval(temp_list);
	return list;
	
}
int listsize(charval_t *list)
{
	charval_t *curr=list;
	if(curr==NULL)
	{
		return 0;
	}
	int count=0;
	for (curr = list; curr->next != NULL; curr = curr->next)
	{
		count++;
	}
	return count+1;
}
int get_value_at(charval_t* list,int position)
{

	int i=0;
	charval_t* curr = list;
	if(position==1)
	{
		return curr->val;
	}
	if (list == NULL) {
        curr->next = NULL;
        return 0;
    }
	for(i=0;i<position-1;i++)
	{
		curr = curr->next;
	}
	int value = curr->val;
	return value;
}

	
