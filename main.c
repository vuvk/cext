#include <stdio.h>
#include <stdlib.h>

#include "cext.h"
#include "list.h"

List* list = NULL;


void main()
{        
    #ifdef _DEBUG
    ListTest();
    #endif // _DEBUG
        
    /* test swap values */
    int a = 10,
        b = 20;
                
    printf("a = %d\tb = %d\n", a, b);
    swap(&a, &b);
    printf("a = %d\tb = %d\n", a, b);
    
    /* High-Level create array */
    int* array = new(int, 10);
    
    /* High-Level create doubly-linked list */
    list = new(List);
    
    /* set values for array and list */
    printf("from array - ");
    for (int i = 0; i < 10; ++i)
    {
        array[i] = i;
        printf("%d ", array[i]);
        
        /* set values for list */
        list->push_back(list, &array[i]);
    }
    printf("\n");
    
    int value;
    void* ptr;
    /* slow get all values */
    printf("from list  - ");
    for (unsigned i = 0; i < list->size; ++i)
    {
        ptr = list->at(list, i);
        value = (ptr)        ? 
                *(int*)(ptr) : 
                0;
        printf("%d ", value);        
    }
    printf("\n");
    
    /* fast get all values */
    printf("from list  - ");
    for (ListElement* it = list->first; it; it = it->next)
    {
        value = (it->value)        ? 
                *(int*)(it->value) : 
                0;
        printf("%d ", value);    
    }
    printf("\n");
    
    /* get front and back values from list */
    int front, back;
    front = (list->front(list))      ? 
            *(int*)list->front(list) : 
            -1;
    back  = (list->back(list))       ? 
            *(int*)list->back(list)  : 
            -1;
    printf("front element - %d\n", front);
    printf("back  element - %d\n", back);
    
    /* check list */        
    printf("list is empty? - %d\n", list->empty(list));
    list->clear(list);
    printf("list is empty? - %d\n", list->empty(list));
    
    /* clear and free list */
    delete(list);
    if (!list)
        printf("WOW! List is NULL now!\n");
        
    /* clear and free array */
    delete(array);    
    if (!array)
        printf("WOW! Array is NULL now!\n");
}
