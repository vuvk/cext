/*
    CEXT.H

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

    (RU) Внимание! Этот заголовок использует встроенные возможности компилятора GCC 3.4.5. Вам нужен компилятор GNU C версии 3.4.5 или выше, чтобы это работало!
    (EN) Attention! This header uses the built-in capabilities of the GCC compiler 3.4.5. You need the GNU C compiler version 3.4.5 or higher to make it work!


    (RU) Этот заголовок служит дополнением к заголовку для работы с движком WorldSim3D на языке C/C++.
         Используется он только на чистом Си для имитации высокоуровневых операций создания и удаления
         объектов с конструктором структур движка WS3D.

         --------------------------
         Создание объектов осуществляется через макрос
            new (Object, count)
         Причем второй параметр не является обязательным. Если его опустить, то будет выделен один объект.
         Если аргументы отсутствуют или указано количество 0, то макрос вернет NULL.
         Для создания массива можно использовать две конструкции:
            int* array = new(int, 10);
         или
            int* array = new(int[10]);

         Вы можете создавать таким образом, как базовые типы, так и собственные:
            typedef struct
            {
                int x, y;
            } example_s;

            example_s* ex = new(example_s);
            
         Создание двусвязного списка:
            List* list = new(List);
            list->push_back(list, some_value);
            list->back(list);  // == some_value
          

         --------------------------
         Удаление объектов осуществляется через макрос
            delete(Object)
         При этом указатель на объект станет равен NULL!

         Например,
            example_s* ex = new(example_s);
            ...
            delete(ex);
            assert(ex == NULL);
             
         Список при удалении будет очищен. 
            delete(list);
            assert(list == NULL);

         --------------------------
         Также в этом заголовке определены два типа строк:
            String  = string_t
            WString = wstring_t

         Примеры:
            // создадим строку, выведем, а потом удалим
            char* str0 = new(String);
            sprintf(str0, "Привет, мир!\n");
            printf(str0);
            delete(str0);

            // хм, а что насчет широкой строки?
            wchar_t* str1 = new(WString);
            swprintf(str1, L"Привет, широкий мир!\n");
            wprintf(str1);
            delete(str1);

         --------------------------

         --------------------------

    (EN) This header complements the header for working with the WorldSim3D engine in C / C ++.
         It is used only on pure C to simulate high-level creation and removal operations
         objects with the constructor of the WS3D engine structures.

         --------------------------
         Creating objects is done through a macro
            new (Object, count)
         And the second parameter is not mandatory. If you omit it, then one object will be selected.
         If the arguments are missing or the number 0 is specified, the macro will return NULL.
         To create an array, you can use two constructs:
            int* array = new(int, 10);
         or
            int* array = new(int[10]);

         You can create in this way both basic types and our own:
            typedef struct
            {
                int x, y;
            } example_s;

            example_s* ex = new(example_s);
            
         Creating of doubly-linked list:
            List* list = new(List);
            list->push_back(list, some_value);
            list->back(list);  // == some_value

         --------------------------
         Deleting objects is done through a macro
            delete(Object)
         In this case, the pointer to the object will be equal to NULL!

         For example,
            example_s* ex = new(example_s);
            ...
            delete(ex); 
             
         List when deleted will be cleared.
            delete(list);
            assert(list == NULL);

         --------------------------
         Also in this header, there are two types of strings:
            String  = string_t
            WString = wstring_t

         Examples:
            // create a string, print it, and then delete it
            char* str0 = new(String);
            sprintf(str0, "Hello world!\n");
            printf(str0);
            delete(str0);

            // hmm, what about the wide string?
            wchar_t* str1 = new(WString);
            swprintf(str1, L"Hello, Wide World!\n");
            wprintf(str1);
            delete(str1);

         --------------------------
*/


#ifndef __CEXT_H
#define __CEXT_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/* doubly-linked list */
#include "list.h"

/*
    ---------------------------------------
    Cloak
    A mini-preprocessor library to demonstrate the recursive capabilities of the preprocessor

        https://github.com/pfultz2/Cloak
    ---------------------------------------
*/
#include "cloak.h"


/*
    ---------------------------------------
    (RU) определение количества аргументов функции
    (EN) calculate count of arguments in function

        http://www.cplusplus.com/forum/lounge/108294/
    ---------------------------------------
*/

#define NUM_ARGS1(_20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1, n, ...) n
#define NUM_ARGS0(...) NUM_ARGS1(__VA_ARGS__,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define NUM_ARGS(...) IF(DEC(NUM_ARGS0(__VA_ARGS__)))(NUM_ARGS0(__VA_ARGS__),IF(IS_PAREN(__VA_ARGS__ ()))(0,1))



/*
    ---------------------------------------
    (RU) тип строк - это массив в N символов
    (EN) string-type is array of chars
    ---------------------------------------
*/

#define MAX_STRING_LENGTH 1024
typedef    char string_t [MAX_STRING_LENGTH];
typedef wchar_t wstring_t[MAX_STRING_LENGTH];

/*
    ---------------------------------------
    (RU) синонимы строковых типов
    (EN) synonims of string-types
    ---------------------------------------
*/
#define  String string_t
#define WString wstring_t


/*
    ---------------------------------------
    (RU) предустановленные значения для "конструкторов" структур WS3D
    (EN) constant WS3D-structs for "constructor" with default values
    ---------------------------------------
*/
/*
const wEAXReverbParameters wEAXReverbParameters_c =
{
    .Density = 1.0f,
    .Diffusion = 1.0f,
    .Gain = 0.32f,
    .GainHF = 0.89f,
    .GainLF = 0.0f,
    .DecayTime = 1.49f,
    .DecayHFRatio = 0.83f,
    .DecayLFRatio = 1.0f,
    .ReflectionsGain = 0.05f,
    .ReflectionsDelay = 0.007f,
    .ReflectionsPanX = 0.0f,
    .ReflectionsPanY = 0.0f,
    .ReflectionsPanZ = 0.0f,
    .LateReverbGain = 1.26f,
    .LateReverbDelay = 0.011f,
    .LateReverbPanX = 0.0f,
    .LateReverbPanY = 0.0f,
    .LateReverbPanZ = 0.0f,
    .EchoTime = 0.25f,
    .EchoDepth = 0.0f,
    .ModulationTime = 0.25f,
    .ModulationDepth = 0.0f,
    .AirAbsorptionGainHF = 0.994f,
    .HFReference = 5000.0f,
    .LFReference = 250.0f,
    .RoomRolloffFactor = 0.0f,
    .DecayHFLimit = true
};

const wReverbParameters wReverbParameters_c =
{
    .Density = 1.0f,
    .Diffusion = 1.0f,
    .Gain = 0.32f,
    .GainHF = 0.89f,
    .DecayTime = 1.49f,
    .DecayHFRatio = 0.83f,
    .ReflectionsGain = 0.05f,
    .ReflectionsDelay = 0.007f,
    .LateReverbGain = 1.26f,
    .LateReverbDelay = 0.011f,
    .AirAbsorptionGainHF = 0.994f,
    .RoomRolloffFactor = 0.0f,
    .DecayHFLimit = true
};

const wChorusParameters wChorusParameters_c =
{
    .Waveform = ECW_TRIANGLE,
    .Phase = 90,
    .Rate = 1.1f,
    .Depth = 0.1f,
    .Feedback = 0.25f,
    .Delay = 0.016f
};

const wDistortionParameters wDistortionParameters_c =
{
    .Edge = 0.2f,
    .Gain = 0.05f,
    .LowpassCutoff = 8000.0f,
    .EqCenter = 3600.0f,
    .EqBandwidth = 3600.0f
};

const wEchoParameters wEchoParameters_c =
{
    .Delay = 0.1f,
    .LRDelay = 0.1f,
    .Damping = 0.5f,
    .Feedback = 0.5f,
    .Spread = -1.0f
};

const wFlangerParameters wFlangerParameters_c =
{
    .Waveform = EFW_TRIANGLE,
    .Phase = 0,
    .Rate = 0.27f,
    .Depth = 1.0f,
    .Feedback = -0.5f,
    .Delay = 0.002f
};

const wFrequencyShiftParameters wFrequencyShiftParameters_c =
{
    .Frequency = 0.0f,
    .Left  = ESD_DOWN,
    .Right = ESD_DOWN
};

const wVocalMorpherParameters wVocalMorpherParameters_c =
{
    .PhonemeA = EMP_A,
    .PhonemeB = EMP_ER,
    .PhonemeACoarseTune = 0,
    .PhonemeBCoarseTune = 0,
    .Waveform = EMW_SINUSOID,
    .Rate = 1.41f
};

const wPitchShifterParameters wPitchShifterParameters_c =
{
    .CoarseTune = 12,
    .FineTune = 0
};

const wRingModulatorParameters wRingModulatorParameters_c =
{
    .Frequency = 440.0f,
    .HighPassCutoff = 800.0f,
    .Waveform = EMW_SINUSOID2
};

const wAutowahParameters wAutowahParameters_c =
{
    .AttackTime = 0.06f,
    .ReleaseTime = 0.06f,
    .Resonance = 1000.0f,
    .PeakGain = 11.22f
};

const wCompressorParameters wCompressorParameters_c =
{
    .Active = true
};

const wEqualizerParameters wEqualizerParameters_c =
{
    .LowGain = 1.0f,
    .LowCutoff = 200.0f,
    .Mid1Gain = 1.0f,
    .Mid1Center = 500.0f,
    .Mid1Width = 1.0f,
    .Mid2Gain = 1.0f,
    .Mid2Center = 3000.0f,
    .Mid2Width = 1.0f,
    .HighGain = 1.0f,
    .HighCutoff = 6000.0f
};
*/


/*
    ---------------------------------------
    (RU) шаблон конструкторов
    (EN) template for constructors
    ---------------------------------------
*/

#define __create(X, N)                                                      \
        ({                                                                  \
            int __i_create;                                                 \
            __typeof__(X)* __p_create = NULL;                               \
            if (N > 0)                                                      \
            {                                                               \
                __p_create = calloc(sizeof(X), N);                          \
                if (__p_create != NULL)                                     \
                {                                                           \
                    for (__i_create = 0; __i_create < N; ++__i_create)      \
                    {                                                       \
                        __p_create[__i_create] = X ## _c;                   \
                    }                                                       \
                }                                                           \
            }                                                               \
            __p_create;                                                     \
         })


/*
    ---------------------------------------
    (RU) "конструкторы" структур WS3D
    (EN) "constructors" of WS3D-structs
    ---------------------------------------
*/
/*
void* __new_wEAXReverbParameters(size_t cnt)
{
    return __create(wEAXReverbParameters, cnt);
}

void* __new_wReverbParameters(size_t cnt)
{
    return __create(wReverbParameters, cnt);
}

void* __new_wChorusParameters(size_t cnt)
{
    return __create(wChorusParameters, cnt);
}

void* __new_wDistortionParameters(size_t cnt)
{
    return __create(wDistortionParameters, cnt);
}

void* __new_wEchoParameters(size_t cnt)
{
    return __create(wEchoParameters, cnt);
}

void* __new_wFlangerParameters(size_t cnt)
{
    return __create(wFlangerParameters, cnt);
}

void* __new_wFrequencyShiftParameters(size_t cnt)
{
    return __create(wFrequencyShiftParameters, cnt);
}

void* __new_wVocalMorpherParameters(size_t cnt)
{
    return __create(wVocalMorpherParameters, cnt);
}

void* __new_wPitchShifterParameters(size_t cnt)
{
    return __create(wPitchShifterParameters, cnt);
}

void* __new_wRingModulatorParameters(size_t cnt)
{
    return __create(wRingModulatorParameters, cnt);
}

void* __new_wAutowahParameters(size_t cnt)
{
    return __create(wAutowahParameters, cnt);
}

void* __new_wCompressorParameters(size_t cnt)
{
    return __create(wCompressorParameters, cnt);
}

void* __new_wEqualizerParameters(size_t cnt)
{
    return __create(wEqualizerParameters, cnt);
}
*/

#define swap(a,b) { __auto_type t = *(a); *(a) = *(b); *(b) = t; }

// test version of GNUC
#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)
/*
#if GCC_VERSION >= 30450
    #define __new_2(X, N)                                                               \
        ({                                                                          \
            void* __tmp_new_2 = NULL;                                               \
            if (__builtin_types_compatible_p (X, wEAXReverbParameters))             \
                __tmp_new_2 = __new_wEAXReverbParameters(N);                        \
            else if (__builtin_types_compatible_p (X, wReverbParameters))           \
                __tmp_new_2 = __new_wReverbParameters(N);                           \
            else if (__builtin_types_compatible_p (X, wChorusParameters))           \
                __tmp_new_2 = __new_wChorusParameters(N);                           \
            else if (__builtin_types_compatible_p (X, wDistortionParameters))       \
                __tmp_new_2 = __new_wDistortionParameters(N);                       \
            else if (__builtin_types_compatible_p (X, wEchoParameters))             \
                __tmp_new_2 = __new_wEchoParameters(N);                             \
            else if (__builtin_types_compatible_p (X, wFlangerParameters))          \
                __tmp_new_2 = __new_wFlangerParameters(N);                          \
            else if (__builtin_types_compatible_p (X, wFrequencyShiftParameters))   \
                __tmp_new_2 = __new_wFrequencyShiftParameters(N);                   \
            else if (__builtin_types_compatible_p (X, wPitchShifterParameters))     \
                __tmp_new_2 = __new_wPitchShifterParameters(N);                     \
            else if (__builtin_types_compatible_p (X, wRingModulatorParameters))    \
                __tmp_new_2 = __new_wRingModulatorParameters(N);                    \
            else if (__builtin_types_compatible_p (X, wAutowahParameters))          \
                __tmp_new_2 = __new_wAutowahParameters(N);                          \
            else if (__builtin_types_compatible_p (X, wCompressorParameters))       \
                __tmp_new_2 = __new_wCompressorParameters(N);                       \
            else if (__builtin_types_compatible_p (X, wEqualizerParameters))        \
                __tmp_new_2 = __new_wEqualizerParameters(N);                        \
            else                                                                    \
                __tmp_new_2 = calloc(sizeof(X), N);                                 \
            __tmp_new_2;                                                            \
         })
#else*/
    #define __new_2(X, N) calloc(sizeof(X), N)
/*#endif // _GCC_VERSION
*/

                
#define __new_1(X)                                                  \
            ({  void* __tmp_new_1 = NULL;                           \
                if (__builtin_types_compatible_p (X, List))         \
                    __tmp_new_1 = ListCreate();                     \
                else                                                \
                    __tmp_new_1 = __new_2(X, 1);                    \
                __tmp_new_1;                                        \
            })
#define __new_0()   NULL

#define new(...)                                                                \
        ({                                                                      \
            void* __tmp_new = NULL;                                             \
            if (NUM_ARGS(__VA_ARGS__) == 1 || NUM_ARGS(__VA_ARGS__) == 2)       \
                __tmp_new = CAT(__new_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__);    \
            __tmp_new;                                                          \
         })
         
#define delete(X)                                                               \
        ({  unsigned id = *(unsigned*)(X);                                      \
            if (id == __LIST_ID)                                                \
                ListDestroy(&(X));                                                \
            else                                                                \
            {                                                                    \
                free(X);                                                        \
                (X) = NULL;                                                     \
            }                                                                   \
         })
         

#endif // __CEXT_H
