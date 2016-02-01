#include "memory_access.h"

memory_read32_t memory_read32;
memory_read16_t memory_read16;
memory_read8_t  memory_read8;

memory_write32_t memory_write32;
memory_write16_t memory_write16;
memory_write8_t  memory_write8;



#include "../common/utils.h"

int memory_bound(int offset, bound_t* bound) {
	if (bound) {
		int end = bound->start + bound->size;
		while (offset < bound->start) offset = (bound->size + offset);
		while (offset > end) offset -= bound->size;
	}
	return offset;
}

static char 	read8(char* mem, int offset, bound_t* bound) {
	return mem[memory_bound(offset, bound)];
}

static void  write8(char val, char* mem, int offset, bound_t* bound) {
	mem[memory_bound(offset, bound)] = val;
}


static int		invert_read32(char* mem, int offset, bound_t* bound) {
	return (((int)(read8(mem, offset + 0, bound)) & 0xff) << 24)
		| (((int)(read8(mem, offset + 1, bound)) & 0xff) << 16)
		| (((int)(read8(mem, offset + 2, bound)) & 0xff) << 8)
		| (((int)(read8(mem, offset + 3, bound)) & 0xff) << 0);
}

static short	invert_read16(char* mem, int offset, bound_t* bound) {
	return (((short)(read8(mem, offset + 0, bound)) & 0xff) << 8)
		| (((short)(read8(mem, offset + 1, bound)) & 0xff) << 0);
}

static void  invert_write32(int val, char* mem, int offset, bound_t* bound) {
	write8((val >> 24) & 0xff, mem, offset + 0, bound);
	write8((val >> 16) & 0xff, mem, offset + 1, bound);
	write8((val >> 8) & 0xff, mem, offset + 2, bound);
	write8((val >> 0) & 0xff, mem, offset + 3, bound);
}
static void  invert_write16(short val, char* mem, int offset, bound_t* bound) {
	write8((val >> 8) & 0xff, mem, offset + 0, bound);
	write8((val >> 0) & 0xff, mem, offset + 1, bound);
}

static int		read32(char* mem, int offset, bound_t* bound) {
	return (((int)(read8(mem, offset + 3, bound)) & 0xff) << 24)
		| (((int)(read8(mem, offset + 2, bound)) & 0xff) << 16)
		| (((int)(read8(mem, offset + 1, bound)) & 0xff) << 8)
		| (((int)(read8(mem, offset + 0, bound)) & 0xff) << 0);
}

static short	read16(char* mem, int offset, bound_t* bound) {
	return (((short)(read8(mem, offset + 1, bound)) & 0xff) << 8)
		| (((short)(read8(mem, offset + 0, bound)) & 0xff) << 0);
}


static void  write32(int val, char* mem, int offset, bound_t* bound) {
	write8((val >> 24) & 0xff, mem, offset + 3, bound);
	write8((val >> 16) & 0xff, mem, offset + 2, bound);
	write8((val >> 8)  & 0xff, mem, offset + 1, bound);
	write8((val >> 0)  & 0xff, mem, offset + 0, bound);
}
static void  write16(short val, char* mem, int offset, bound_t* bound) {
	write8((val >> 8) & 0xff, mem, offset + 1, bound);
	write8((val >> 0) & 0xff, mem, offset + 0, bound);
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
