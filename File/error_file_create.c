#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
    int cnt = 0;
    while (1) {
    char name[64];
    snprintf(name, sizeof(name),"%d.txt", cnt);
    int fd = creat(name, 644);
    sleep(10);
    ++cnt;
    }
    return 0;
}