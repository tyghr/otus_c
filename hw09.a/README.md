Задание:
- Обработать все потенциальные ошибки

Описание:
- Данная программа генерирует заданное пользователем количество случайных чисел, записывает их в файл и выводит на консоль их арифметическое среднее.
- В процессе устранения недостатков допустимо менять интерфейсы.

```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

int div_int(int a, int b) {
    return a / b;
}

double div_dbl(double a, double b) {
    return a / b;
}

int calc_mean(const int * buf, int len) {
    int mean = 0;
    for (int i = 0; i < len; i++) {
        mean += buf[i];
    }
    mean = div_int(mean, len);
    return mean;
}

void rand_save(const int* buf, int length, const char fname[]) {
    FILE* f = fopen(fname, "w");
    for (int i = 0; i < length; i++) {
        fprintf(f, "%1d, ", buf[i]);
    }
}

void rand_gen(int** buf, int length) {
    *buf = (int*)malloc(length * sizeof(int));
    for (int i = 0; i < length; i++) {
        (*buf)[i] = rand() % 10;
    }

    rand_save(*buf, length, "D:\\rand.txt");
}

int main() {
    int length;
    printf("Enter the length: ");
    scanf("%d", & length);
    //while (getchar() != '\n');
    int* buf;
    rand_gen(&buf, length);
    int mean = calc_mean(buf, length);
    printf("Mean = %d \n", mean);
}
```
