4-0
Заданы описание структуры и прототип функции:

#include <stdint.h>

typedef struct { uint8_t u8; uint16_t u16; uint32_t u32; } values_t;

extern uint64_t calculate(const values_t *values);

Реализуйте на языке ассемблера ARM-32 эту функцию, которая вычисляет значение values->u8 + values->u16 + values->u32.

Запрещается выделять дополнительную память на стеке, для решения этой задачи достаточно только четырех регистров.

Процессор работает в режиме Little-Endian, используются стандартные соглашения о вызовах gnueabi и стандартные Си-соглашения о выравнивании.


4-1
Реализуйте функцию с прототипом:

typedef enum { PlusZero = 0x00, MinusZero = 0x01, PlusInf = 0xF0, MinusInf = 0xF1, PlusRegular = 0x10, MinusRegular = 0x11, PlusDenormal = 0x20, MinusDenormal = 0x21, SignalingNaN = 0x30, QuietNaN = 0x31 } float_class_t;

extern float_class_t classify(double *value_ptr);

которая классифицирует специальные значения вещественного числа, на которое указывает value_ptr, в соответствии со стандартом IEEE 754.

При решении допускается использовать только побитовые операции, включая сдвиги.


4-2
Реализуйте функцию с прототипом:

extern int check_int(uint32_t u32); /* retval: 0 - false, 1 - true */ которая проверяет, может ли число u32 быть представлено типом float:

float f = u32; uint64_t y = f; assert(u32==y);

Функция должна вернуть 1, если число может быть представлено типом float, или 0, - если не может.

Допускается исользовать только побитовые операции.


4-3
Заданы описание структуры и прототип функции:

typedef struct Person { uint32_t id; uint8_t age; char first_name[20]; char last_name[20]; } person_t;

extern void sort_by_age(int N, person_t *persons); Реализуйте функию sort_by_age, которая сортирует массив persons из N элементов в порядке возрастания поля age.

Можно использовать любой алгоритм сортировки, в том числе имеющий сложность O(N2).
