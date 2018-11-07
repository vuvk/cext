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

#ifndef __LIST_H
#define __LIST_H

//#define __MULTITHREADS
//#define __USE_SDL_THREADS

#define __LIST_ID 1953720652   /* 'L' 'i' 's' 't' */

typedef struct ListElement_tag
{
    void* value;

    struct ListElement_tag* prev;
    struct ListElement_tag* next;  
} ListElement;

typedef struct
{
    unsigned __id;
    
    unsigned size;

    ListElement* first;        /* head */
    ListElement* last;         /* tail */
    
    void* (*front)(void* this);
    void* (*back) (void* this);
    bool  (*push_back)(void* this, void* value);
    void  (*pop_back) (void* this);
    bool  (*empty)(void* this);
    void  (*clear)(void* this);
    void* (*at)(void* this, unsigned position);
} List;

/** init memory as list */
void ListInit(void* mem);
/** create list and return pointer to list */
List* ListCreate ();
/** delete all elements in list */
void ListClear (List* list);
/** clear and destroy list */
void ListDestroy (List** list);

/** add value to exists list, return false if not */
bool ListAddElement (List* list, void* value);
/** delete element from list */
bool ListDeleteElement (List* list, ListElement* element);
/** delete value from list */
void ListDeleteElementByValue (List* list, void* value);
/** delete value from list by position in list */
void ListDeleteElementByNumber (List* list, unsigned numOfElement);
/** delete last value from list */
void ListPopBack(List* list);

/* GETTERS */
/** get first value from list */
void* ListGetFirstValue(List* list);
/** get last value from list */
void* ListGetLastValue(List* list);
/** return element by value */
ListElement* ListGetElementByValue (List* list, const void* value);
/** return element by number in list */
ListElement* ListGetElementByNumber (List* list, unsigned numOfElement);
/** return value by number in list */
void* ListGetValueByNumber (List* list, unsigned numOfElement);
/** return number of value in list (if exists), else return -1 */
int ListGetNumberByValue (List* list, void* value);

/** get count of elements in list */
unsigned ListGetSize (List* list);
/** check empty list */
bool ListIsEmpty(List* list);

/* SETTERS */
/** set value of element by position in dictionary */
void ListSetValueByNumber(List* list, unsigned numOfElement, void* value);
/** change value of element. Return false if value not changed */
bool ListChangeValue(List* list, const void* oldValue, void* newValue);

/* tests */
#ifdef _DEBUG
#include <assert.h>
#include "SDL2/SDL.h"
void ListTest();
#endif // _DEBUG

#endif // __LIST_H
