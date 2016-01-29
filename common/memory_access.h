#ifndef __MEMORY_ACCESS__
#define __MEMORY_ACCESS__

void memory_access_initialize(int invert);

typedef int		(*memory_read32_t)(char*, int, int);
typedef short	(*memory_read16_t)(char*, int, int);
typedef char 	(*memory_read8_t)(char*, int, int);

typedef void  (*memory_write32_t)(int, char*, int, int);
typedef void  (*memory_write16_t)(short, char*, int, int);
typedef void  (*memory_write8_t)(char, char*, int, int);

extern memory_read32_t memory_read32;
extern memory_read16_t memory_read16;
extern memory_read8_t  memory_read8;

extern memory_write32_t memory_write32;
extern memory_write16_t memory_write16;
extern memory_write8_t  memory_write8;

#endif
