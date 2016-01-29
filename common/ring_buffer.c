#include <stdlib.h>
#include <string.h>
#include "ring_buffer.h"

void			ring_buffer_add(ring_buffer_t* ring, void* data)
{
	if (ring->write_index >= ring->size)
	{
		ring->free_element_fct(ring->data[ring->read_index % ring->size]);
		ring->read_index++;
	}

	ring->data[ring->write_index % ring->size] = data;
	ring->write_index++;
}

void			ring_buffer_destroy(ring_buffer_t* ring)
{
	int count = ring->size;
	int start = ring->read_index;
	while (count--)
	{
		ring->free_element_fct(ring->data[start % ring->size]);
		start++;
	}

	free(ring->data);
	free(ring);
}

ring_buffer_t* ring_buffer_initialize(int size, t_ring_fct_free free_element_fct)
{
	ring_buffer_t* ring = (ring_buffer_t*)malloc(sizeof(ring_buffer_t));
	ring->size = size;
	ring->read_index = 0;
	ring->write_index = 0;
	ring->data = malloc(sizeof(void*) * size);
	memset(ring->data, 0, sizeof(void*) * size);
	ring->free_element_fct = free_element_fct;
	return ring;
}

void**			ring_buffer_get_last(ring_buffer_t* ring)
{
	int index;
	if (ring->write_index == 0)
		return NULL;
	index = (ring->write_index - 1);
	if (index < 0)
		index = ring->size - 1;
	return &ring->data[index % ring->size];
}
