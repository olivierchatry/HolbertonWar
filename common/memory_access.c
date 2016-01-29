#include "memory_access.h"

memory_read32_t memory_read32;
memory_read16_t memory_read16;
memory_read8_t  memory_read8;

memory_write32_t memory_write32;
memory_write16_t memory_write16;
memory_write8_t  memory_write8;



#include "../common/utils.h"


static int		invert_read32(char* mem, int offset, int modulo) {
	return ((int)(mem[(offset + 0) % modulo] & 0xff) << 24)
	 	   | ((int)(mem[(offset + 1) % modulo] & 0xff) << 16)
			 | ((int)(mem[(offset + 2) % modulo] & 0xff) << 8)
			 | ((int)(mem[(offset + 3) % modulo] & 0xff) << 0) ;
}

static short	invert_read16(char* mem, int offset, int modulo) {
	return ((short)(mem[(offset + 0) % modulo] & 0xff) << 8)
	 	   | ((short)(mem[(offset + 1) % modulo] & 0xff) << 0);
}

static void  invert_write32(int val, char* mem, int offset, int modulo) {
	mem[(offset + 0) % modulo] = (val >> 24) & 0xff;
	mem[(offset + 1) % modulo] = (val >> 16) & 0xff;
	mem[(offset + 2) % modulo] = (val >> 8)  & 0xff;
	mem[(offset + 3) % modulo] = (val >> 0)  & 0xff;
}
static void  invert_write16(short val, char* mem, int offset, int modulo) {
	mem[(offset + 0) % modulo] = (val >> 8)  & 0xff;
	mem[(offset + 1) % modulo] = (val >> 0)  & 0xff;
}

static int		read32(char* mem, int offset, int modulo) {
	return ((int)(mem[(offset + 3) % modulo] & 0xff) << 24)
	 	   | ((int)(mem[(offset + 2) % modulo] & 0xff) << 16)
			 | ((int)(mem[(offset + 1) % modulo] & 0xff) << 8)
			 | ((int)(mem[(offset + 0) % modulo] & 0xff) << 0) ;
}

static short	read16(char* mem, int offset, int modulo) {
	return ((short)(mem[(offset + 1) % modulo] & 0xff) << 8)
	 	   | ((short)(mem[(offset + 0) % modulo] & 0xff) << 0);
}

static char 	read8(char* mem, int offset, int modulo) {
	return mem[(offset + 0) % modulo];
}

static void  write32(int val, char* mem, int offset, int modulo) {
	mem[(offset + 3) % modulo] = (val >> 24) & 0xff;
	mem[(offset + 2) % modulo] = (val >> 16) & 0xff;
	mem[(offset + 1) % modulo] = (val >> 8)  & 0xff;
	mem[(offset + 0) % modulo] = (val >> 0)  & 0xff;
}
static void  write16(short val, char* mem, int offset, int modulo) {
	mem[(offset + 1) % modulo] = (val >> 8)  & 0xff;
	mem[(offset + 0) % modulo] = (val >> 0)  & 0xff;
}

static void  write8(char val, char* mem, int offset, int modulo) {
	mem[(offset + 0) % modulo] = val;
}

void memory_access_initialize(int invert) {
	memory_read8  = read8;
	memory_write8  = write8;
	if (invert) {
		memory_read32 = invert_read32;
		memory_read16 = invert_read16;
		memory_write32 = invert_write32;
		memory_write16 = invert_write16;
	} else {
		memory_read32 = read32;
		memory_read16 = read16;
		memory_write32 = write32;
		memory_write16 = write16;
	}
}
