#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

unsigned int gdtinc = 0;

struct gdt_entry {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

struct gdt_pointer {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct gdt_entry gdt[3];
struct gdt_pointer gdtp;

extern void gdt_flush();

void gdt_encoder(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran) {
/*  if (base > 0xFFFFFFFF) {
        fatal("The GDT cannot be encoded above 0xFFFF.");
        __asm__("cli; hlt");
    } 
    commented out temporarily*/
    
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0x0FF;

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_setup() {
    gdtp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gdtp.base = (unsigned int)&gdt;

    gdt_encoder(0, 0, 0, 0, 0);
    // null descriptor
   
    // code segment
    gdt_encoder(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // data segment
    gdt_encoder(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    gdt_flush();

    gdtinc = 1;
}