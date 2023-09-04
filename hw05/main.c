#include <stdio.h>
#include <stdlib.h>

char *empty_str = "";
long data[] = {4, 8, 15, 16, 23, 42};
int data_length = sizeof(data) / sizeof(data[0]);
char *int_format = "%ld ";

void print_int(long i) {
    printf(int_format, i);
    fflush(stdout);
}

long p(long i) {
    return i & 1;
}

struct element {
    long value;
    struct element *next;
};

struct element *add_element(long l, struct element *next) {
    struct element *ep = malloc(sizeof(struct element));
    if (!ep) abort();

    ep->value = l;
    ep->next = next;

    return ep;
}

void m(struct element *ep, void (*fp)(long)) {
    if (!ep) return;
    fp(ep->value);
    m(ep->next, fp);
}

struct element *f(struct element *filtered, struct element *base, long (*fp)(long)) {
    if (!base) goto outf;

    if (!p(base->value))
        filtered = filtered;
    else
        filtered = add_element(base->value, filtered);

    filtered = f(filtered, base->next, fp);

outf:
    return filtered;
}

int main(void) {
    struct element *ep = NULL;

    for (int i = data_length; i != 0; --i)
        ep = add_element(data[i-1], ep);

    m(ep, print_int);
    puts(empty_str);

    struct element *filtered = NULL;
    filtered = f(filtered, ep, p);

    m(filtered, print_int);
    puts(empty_str);

    return 0;
}
