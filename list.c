/*  
    =============================================================================
    Copyright [2017-2018] [Anton "Vuvk" Shcherbatykh]

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
    ==============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "list.h"
              
/* FIX ME */       
/* use multithreading? */
#ifdef __MULTITHREADS
    #define MAX_VALUES_FOR_ONE_THRD 50        /* max length of list|dictionary for do search in one thread */
    #ifdef __USE_SDL_THREADS
        #include "SDL2/SDL.h"
    #else  // NOT __USE_SDL_THREADS
        #include "tinycthread.h"
    #endif // __USE_SDL_THREADS
#endif // __MULTITHREADS


#define LIST_CHECK_VALID                            \
                if (!list) return 0;                \
                unsigned id = *(unsigned*)list;    \
                if (id != __LIST_ID) return 0;


void ListInit(void* mem)
{
    if (mem)
    {
        List* list = mem;
        
        /* already initialized? */
        if (list->__id == __LIST_ID)
        {
            ListClear(list);
        }
        else
        {
            memset(list, 0, sizeof(List));
            
            list->__id      = __LIST_ID;

            list->front     = &ListGetFirstValue;
            list->back      = &ListGetLastValue;
            list->push_back = &ListAddElement;
            list->pop_back  = &ListPopBack;
            list->at        = &ListGetValueByNumber;
            list->clear        = &ListClear;
            list->empty        = &ListIsEmpty;            
        }
    }
}

List* ListCreate()
{
    List* list = malloc(sizeof(List));
    if (list == NULL)
        return NULL;

    ListInit(list);

    return list;
}

void ListClear(List* list)
{
    LIST_CHECK_VALID
    
    while (list->size)
    {
        ListPopBack(list);
    }
}

void ListDestroy (List** list)
{    
    if (!list || !(*list))
        return;

    ListClear(*list);
    
    (**list).__id = 0;

    free (*list);
    *list = NULL;
}

bool ListAddElement (List* list, void* value)
{    
    LIST_CHECK_VALID
    
    ListElement* new_element = calloc(sizeof(char), sizeof(ListElement));
    new_element->value = value;
    ++list->size;
    
    /* if it is first element */
    if (list->first == NULL)
    {
        list->first = new_element;
        list->last  = new_element;
    }
    else
    {
        new_element->prev = list->last;
        list->last->next  = new_element;
        list->last        = new_element;
    }
    return true;
}

bool ListDeleteElement (List* list, ListElement* element)
{
    LIST_CHECK_VALID
    
    if (!element)
        return false;

    //free (element->value);
    element->value = NULL;

    --list->size;

    if (element->prev != NULL && element->next != NULL)
    {
        element->prev->next = element->next;
        element->next->prev = element->prev;
        
        goto element_delete;
    }

    if (element->prev == NULL && element->next != NULL)
    {
        list->first = element->next;
        element->next->prev = NULL;

        goto element_delete;
    }

    if (element->prev != NULL && element->next == NULL)
    {
        list->last = element->prev;
        element->prev->next = NULL;

        goto element_delete;
    }

    if ((list->size <= 0) || (element->prev == NULL && element->next == NULL))
    {
        list->first = NULL;
        list->last = NULL;

        list->size = 0;
    }

element_delete:
    free (element);
    element = NULL;

    return true;
}

void ListPopBack(List* list)
{    
    LIST_CHECK_VALID
    
    if (list->size > 0)
    {
        ListElement* prev = list->last->prev;
        free(list->last);
        list->last = prev;
        if (prev)
            prev->next = NULL;
        
        --list->size;
        
        /* empty list */
        if (!list->size)
        {
            list->first = NULL;
            list->last  = NULL;
        }
    }
}

void* ListGetFirstValue(List* list)
{
    LIST_CHECK_VALID
    
    if (list->first)
        return list->first->value;
    return NULL;
}

void* ListGetLastValue(List* list)
{
    LIST_CHECK_VALID
    
    if (list->last)
        return list->last->value;
    return NULL;
}

#ifdef __MULTITHREADS
/* parameters for quick search element by value in list */
typedef struct
{
    ListElement*  startElement;    /* head or tail */
    ListElement** resultElement;
    void*         value;           /* value for search */
    int           maxNumber;       /* maxNumber - when it's stop */
    signed char   direction;       /* direction for finding - 1 (head-to-tail) or -1(tail-to-head) */
} SThrdParameter;

/* func for search element by value in thread */
static int ListGetElementByValueThread(void* arg)
{
    if (arg == NULL)
#ifdef __USE_SDL_THREADS
        return SDL_FALSE;
#else
        return thrd_error;
#endif // __USE_SDL_THREADS

    SThrdParameter* param   = (SThrdParameter*)arg;
    ListElement*   element = param->startElement;
    int maxPos = param->maxNumber;
    ListElement** resultElement = param->resultElement;


    // from head to tail
    if (param->direction > 0)
    {
        register unsigned i = 0;
        while ((element != NULL) &&
               (*resultElement == NULL) &&
               (element->value != param->value) &&
               (i < maxPos))
        {
            element = element->next;
            ++i;
        }
    }
    else
    // from tail to head
    {
        register unsigned i = 0;
        while ((element != NULL) &&
               (*resultElement == NULL) &&
               (element->value != param->value) &&
               (i < maxPos))
        {
            element = element->prev;
            ++i;
        }
    }

    if (*resultElement != NULL)
#ifdef __USE_SDL_THREADS
        return SDL_TRUE;
#else
        return thrd_success;
#endif // __USE_SDL_THREADS

    if (element != NULL && element->value == param->value)
        *(resultElement) = element;

#ifdef __USE_SDL_THREADS
    return SDL_TRUE;
#else
    return thrd_success;
#endif // __USE_SDL_THREADS
}

static ListElement* ListGetElementByValueInThreads (List* list, const void* value)
{
    LIST_CHECK_VALID
    
    if (list->first == NULL || list->last == NULL)
        return NULL;

    ListElement* result = NULL;
    /* center of list */
    int center = list->size / 2;

    SThrdParameter param1 = {list->first, &result, (void*)value, center,               1};
    SThrdParameter param2 = {list->last,  &result, (void*)value, list->size - center, -1};

#ifdef __USE_SDL_THREADS
    SDL_Thread* threadFromHead = SDL_CreateThread(ListGetElementByValueThread, "fromHead", &param1);
    if (threadFromHead == NULL)
        return NULL;
    SDL_Thread* threadFromTail = SDL_CreateThread(ListGetElementByValueThread, "fromTail", &param2);
    if (threadFromTail == NULL)
        return NULL;

    SDL_WaitThread(threadFromHead, NULL);
    SDL_WaitThread(threadFromTail, NULL);

#else // NOT __USE_SDL_THREADS
    thrd_t threadFromHead = 0;
    thrd_t threadFromTail = 0;

    if (thrd_create(&threadFromHead, ListGetElementByValueThread, &param1) != thrd_success)
        return NULL;

    if (thrd_create(&threadFromTail, ListGetElementByValueThread, &param2) != thrd_success)
        return NULL;

    /* waits until threads are completed */
    if (thrd_join(threadFromHead, NULL) != thrd_success)
        return NULL;
    if (thrd_join(threadFromTail, NULL) != thrd_success)
        return NULL;
#endif // __USE_SDL_THREADS

    return result;
}
#endif // __MULTITHREADS

ListElement* ListGetElementByValue (List* list, const void* value)
{
    LIST_CHECK_VALID
    
    #ifdef __MULTITHREADS
    ListElement* element = NULL;
    if (list->size <= MAX_VALUES_FOR_ONE_THRD)
    {
        element = list->first;
        while (element && element->value != value)
            element = element->next;
    }
    else
        element = ListGetElementByValueInThreads(list, value);
    #else // NOT __MULTITHREADS
    ListElement* element = list->first;
    while (element && element->value != value)
            element = element->next;
    #endif // __MULTITHREADS

    return element;
}

ListElement* ListGetElementByNumber (List* list, unsigned numOfElement)
{
    LIST_CHECK_VALID
    
    if (numOfElement >= list->size)
        return NULL;
    
    ListElement* element = NULL;
    /* check which way is faster - from the head or from the tail */
    if ((list->size - numOfElement) >= numOfElement)
    {
        /* go from head */
        element = list->first;
        for (unsigned i = 0; element && (i < numOfElement); ++i)
            element = element->next;
    }
    else
    {
        /* go from tail */
        element = list->last;
        for (unsigned i = list->size - 1; element && (i > numOfElement); --i)
            element = element->prev;
    }

    return element;
}

void ListDeleteElementByValue (List* list, void* value)
{
    LIST_CHECK_VALID
    
    if (value == NULL)
        return;

    ListElement* element = ListGetElementByValue(list, value);
    if (element)
        ListDeleteElement (list, element);
}

void ListDeleteElementByNumber (List* list, unsigned numOfElement)
{
    LIST_CHECK_VALID
    
    ListElement* element = ListGetElementByNumber(list, numOfElement);
    if (element)
        ListDeleteElement (list, element);
}

void* ListGetValueByNumber (List* list, unsigned numOfElement)
{
    LIST_CHECK_VALID
    
    ListElement* element = ListGetElementByNumber(list, numOfElement);
    if (!element)
        return NULL;

    return element->value;;
}

int ListGetNumberByValue (List* list, void* value)
{
    if (!list || !value)
        return -1;

    if (list->first == NULL)
        return -1;

    // ����������� �� ���� ���������, ���� �� ��������� �� �������� ��� �� �����...
    int num = 0;
    ListElement* element = list->first;
    while ((element != NULL) && (element->value != value))
    {
        element = element->next;
        ++num;
    }

    if (element != NULL && element->value == value)
        return num;
    else
        return -1;
}

unsigned ListGetSize (List* list)
{
    LIST_CHECK_VALID

    return list->size;
}

bool ListIsEmpty(List* list)
{
    LIST_CHECK_VALID
    
    return (list->size == 0);    
}

void ListSetValueByNumber(List* list, unsigned numOfElement, void* value)
{
    LIST_CHECK_VALID
    
    if (value == NULL || numOfElement >= list->size)
        return;

    if (list->size == 0)
        return;

    ListElement* element = ListGetElementByNumber(list, numOfElement);
    if (element == NULL)
        return;

    element->value = value;
}

bool ListChangeValue(List* list, const void* oldValue, void* newValue)
{
    LIST_CHECK_VALID
    
    if (oldValue == NULL || newValue == NULL)
        return false;

    if (list->size == 0)
        return false;

    ListElement* element = ListGetElementByValue(list, oldValue);
    if (element == NULL)
        return false;

    element->value = newValue;

    return true;
}



/*  TESTS!!! */
#ifdef _DEBUG
#define MAX_LIST_SIZE 6000
void ListTest()
{
    printf ("List's tests started!!!\n");

    int* value1 = malloc(sizeof(int));
    int* value2 = malloc(sizeof(int));
    int* value3 = malloc(sizeof(int));
    int* value4 = malloc(sizeof(int));
    int* value5 = malloc(sizeof(int));
    int* value6 = NULL;
    *value1 = 1;
    *value2 = 2;
    *value3 = 3;
    *value4 = 4;
    *value5 = 5;

    void* ptr = NULL;
    int num = -1;
    bool result;

    List* list = ListCreate();

    /* test1 : check size */
    printf ("--------test1--------\n");
    ListAddElement(list, value1);
    ListAddElement(list, value2);
    ListAddElement(list, value3);
    ListAddElement(list, value4);
    assert (ListGetSize(list) == 4);
    printf ("size of list : %d\n", ListGetSize(list));

    /* test2 : Get element by number */
    printf ("--------test2--------\n");
    // from the head
    ptr = ListGetValueByNumber(list, 1);
    assert (ptr != NULL);
    assert ((int*)ptr == value2);
    printf ("get value by number 1 - %d\n", *(int*)ptr);
    // from the tail
    ptr = ListGetValueByNumber(list, 3);
    assert (ptr != NULL);
    assert ((int*)ptr == value4);
    printf ("get value by number 3 - %d\n", *(int*)ptr);

    /* test3 : Get element by number over size of list */
    printf ("--------test3--------\n");
    ptr = ListGetValueByNumber(list, 5);
    assert (ptr == NULL);
    printf ("number over size of list!!\n");

    /* test4 : delete element by number */
    printf ("--------test4--------\n");
    ListDeleteElementByNumber (list, 0);
    assert (ListGetSize(list) == 3);
    printf ("new size of list : %d\n", ListGetSize(list));

    /* test5 : Get new element by number */
    printf ("--------test5--------\n");
    ptr = ListGetValueByNumber(list, 2);
    assert (ptr != NULL);
    printf ("get new value by number 2 : %d\n", *(int*)ptr);

    /* test6 : Get element by number over new size of list */
    printf ("--------test6--------\n");
    ptr = ListGetValueByNumber(list, 4);
    assert (ptr == NULL);
    printf ("number over size of list!!\n");

    /* test7 : delete element by deleted value early */
    printf ("--------test7--------\n");
    ListDeleteElementByValue(list, value1);
    assert (ListGetSize(list) == 3);
    printf ("size of list : %d\n", ListGetSize(list));

    /* test8 : delete element by exists value */
    printf ("--------test8--------\n");
    ListDeleteElementByValue(list, value2);
    assert (ListGetSize(list) == 2);
    printf ("new size of list : %d\n", ListGetSize(list));

    /* test9 : Get number of value */
    printf ("--------test9--------\n");
    num = ListGetNumberByValue(list, value3);
    assert (num != -1);
    printf ("number of value is %d\n", num);

    /* test10 : Get number of deleted value */
    printf ("--------test10--------\n");
    num = ListGetNumberByValue(list, value2);
    assert (num == -1);
    printf ("value not exists!\n");

    /* test11 : clear list */
    printf ("--------test11--------\n");
    ListClear(list);
    assert (ListGetSize(list) == 0);
    printf ("now size of list : %d\n", ListGetSize(list));

    /* test12 : delete list and try to get value */
    printf ("--------test12--------\n");
    ListDestroy(&list);
    assert (list == NULL);
    ptr = ListGetValueByNumber(list, 1);
    assert (ptr == NULL);
    printf ("nothing to do this!\n");

    /* prepare for new tests */
    free(value1);
    free(value2);
    free(value3);
    free(value4);
    free(value5);
    free(value6);
    value1 = malloc(sizeof(int));
    value2 = malloc(sizeof(int));
    value3 = malloc(sizeof(int));
    value4 = malloc(sizeof(int));
    value5 = malloc(sizeof(int));
    value6 = malloc(sizeof(int));
    *value1 = 1;
    *value2 = 2;
    *value3 = 3;
    *value4 = 4;
    *value5 = 5;
    *value6 = 6;
    list = ListCreate();
    ListAddElement(list, value1);
    ListAddElement(list, value2);
    ListAddElement(list, value3);

    /* test13 : set new value by number */
    printf ("--------test13--------\n");
    ListSetValueByNumber(list, 0, value5);
    assert(ListGetValueByNumber(list, 0) == value5);
    printf ("passed!\n");

    /* test14 : change exists value */
    printf ("--------test14--------\n");
    result = ListChangeValue(list, value5, value4);
    assert(result == true);
    assert(ListGetValueByNumber(list, 0) == value4);
    printf ("passed!\n");

    // prepare long list...
    ListClear(list);
    int* array = malloc(MAX_LIST_SIZE*sizeof(int));
    for (unsigned i = 0; i < MAX_LIST_SIZE; ++i)
    {
        array[i] = i;
        ListAddElement(list, &array[i]);
    }

    /* test15 : get element by value */
    printf ("--------test15--------\n");
    ListElement* element = NULL;
    #ifdef __USE_SDL_THREADS
    unsigned startT = SDL_GetTicks();
    #endif // __USE_SDL_THREADS
    for (unsigned i = 0; i < MAX_LIST_SIZE; ++i)
    {
        //printf("value%d = ", i);
        element = ListGetElementByValue(list, &array[i]);
        assert(element != NULL);
        assert(element->value == &array[i]);
        //printf("%d\n", i, *(int*)element->value);
    }
    #ifdef __USE_SDL_THREADS
    unsigned endT = SDL_GetTicks();
    printf ("time elapsed - %d ms\n", endT - startT);
    #endif // __USE_SDL_THREADS
    printf ("passed!\n");

    ListDestroy(&list);
    free(array);
    free(value1);
    free(value2);
    free(value3);
    free(value4);
    free(value5);
    free(value6);
    free(ptr);

    /* passed */
    printf ("--------result-------\n");
    printf ("all list's tests are passed!\n");
}
#endif // _DEBUG
