### Introduction

В рамках данного проекта реализация библиотеки с основными стандартными контейнерными классами языка С++: `list` (список), `map` (словарь), `queue` (очередь), `set` (множество), `stack` (стек) и `vector` (вектор). Реализации должны предоставлять весь набор стандартных методов и атрибутов для работы с элементами, проверкой заполненности контейнера и итерирования. 

## Chapter I

- Программа разработана на языке C++ стандарта C++17 с использованием компилятора gcc
- Код программы находится в develop/includes
- Google Style
- Полное покрытие unit-тестами методов контейнерных классов c помощью библиотеки GTest

### Part 1. Реализация библиотеки s21_containers.h

Реализация классов библиотеки `s21_containers.h` 
Список классов: `list` (список), `map` (словарь), `queue` (очередь), `set` (множество), `stack` (стек), `vector` (вектор).
- Решение в виде заголовочного файла `s21_containers.h`, который включает в себя другие заголовочные файлы с реализациями необходимых контейнеров (`s21_list.h`, `s21_map.h` и т.д.)
- Makefile для тестов написанной библиотеки (с целями clean, test)

### Part 2. Дополнительно. Реализация библиотеки s21_containersplus.h

Список доп.классов: `array` (массив), `multiset` (мультимножество).

### Part 3. Дополнительно. Реализация методов `insert_many`

 Классы дополнены соответствующими методами, согласно таблице:

| Modifiers      | Definition                                      | Containers |
|----------------|-------------------------------------------------| -------------------------------------------|
| `iterator insert_many(const_iterator pos, Args&&... args)`          | inserts new elements into the container directly before `pos`  | List, Vector |
| `void insert_many_back(Args&&... args)`          | appends new elements to the end of the container  | List, Vector, Queue |
| `void insert_many_front(Args&&... args)`          | appends new elements to the top of the container  | List, Stack |
| `vector<std::pair<iterator,bool>> insert_many(Args&&... args)`          | inserts new elements into the container  | Map, Set, Multiset |

