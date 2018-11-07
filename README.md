# cext
C-Extended. Small project for using pure C with High-Level features


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
