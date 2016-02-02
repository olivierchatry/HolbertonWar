#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

typedef void(*t_ring_fct_free)(void*);

struct ring_buffer_s
{
	int								size;
	int								read_index;
	int								write_index;
	void**						data;
	t_ring_fct_free		free_element_fct;
};

typedef struct ring_buffer_s ring_buffer_t;

ring_buffer_t*	ring_buffer_initialize(int size, t_ring_fct_free free_element_fct);
void						ring_buffer_destroy(ring_buffer_t* ring);
void						ring_buffer_add(ring_buffer_t* ring, void* data);
void**					ring_buffer_get_last(ring_buffer_t* ring);


#endif
