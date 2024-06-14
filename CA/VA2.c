#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define OFFSET_MASK 0x00000FFF

int make_offsetmask(int a) {
    int mask = 1;
    for (int i = 1; i < a; i++) {
        mask = (mask << 1) | 1;
    }

    return mask;
}

void bit_print(int a)
{
    int i;
    int n = sizeof(int) * CHAR_BIT;  /* in limits.h */
    int mask = 1 << (n - 1);         /* mask = 100...0 */

    for (i = 1; i <= n; ++i) {
        putchar(((a & mask) == 0) ? '0' : '1');
        a <<= 1;
        if (i % CHAR_BIT == 0 && i < n)
            putchar(' ');
    }
    printf("\n");
}

int main(int argc, char* argv[])
{
    int vpn, ppn, offset;
    int va, pa;
    int shift, mask;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <virtual address> <shift>\n", argv[0]);
        return 1;
    }

    va = atoi(argv[1]);
    shift = atoi(argv[2]);
    mask = make_offsetmask(shift);

    vpn = (va & ~mask) >> shift;
    offset = va & mask;

    printf("----------------------------------------\n");
    // calculate vpn and offset
    printf("Virtual Address: %d\n", va);
    bit_print(va);
    printf("Virtual Page Number: %d\n", vpn);
    bit_print(vpn);
    printf("Offset: %d\n", offset);
    bit_print(offset);

    printf("----------------------------------------\n");
    // calculate address
    ppn = vpn % 100;
    printf("Physical Page Number: %d\n", ppn);
    bit_print(ppn);
    pa = (ppn << shift) | offset;
    printf("Physical Address: %d\n", pa);
    bit_print(pa);
    printf("----------------------------------------\n");

    return 0;
}