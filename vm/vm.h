#ifndef __VM_H__
#define __VM_H__

#include "../common/holberton-core.h"
#include "core.h"

#define TYPE(v, a) ((v >> (a * 2)) & 3)

#define VM_ERROR_ENCODING	-1
#define VM_ERROR_OPCODE		-2
#define VM_ERROR_REGISTER	-3

#define VM_OK			1
#define VM_MAX_JUMP	512
#define VM_PROCESS_MAX_IO 16
typedef struct process_s
{

	int32			pc;
	int32			next_pc;
	int32			reg[CORE_REGISTER_COUNT];
	int32			zero;

	int32			cycle_wait;
	int32			cycle_live;
	opcode_t*	current_opcode;

	int32			free;
	int32			internal_id;

	core_t*		core;

	int16			memory_read_op[VM_PROCESS_MAX_IO];
	int16 		memory_read_op_count;
	int16			memory_write_op[VM_PROCESS_MAX_IO];
	int16 		memory_write_op_count;

	int32			jump;
	int16			jump_from;
	int16			jump_to;

	int32			cycle_create;

	memory_callback_t memory_callback;

} process_t;


typedef struct vm_s
{
	int8*		memory;
	int8*		shadow;
	int32		memory_size;
	int32		memory_modulo;
	int32		live_count;
	int32		live_total;

	int32		cycle_current;
	int32		cycle_to_die;
	int32		cycle_delta;
	int32		cycle_total;
	int32		cycle_barrier;

	process_t**		processes;
	int32					process_count;

	process_t**		processes_pool;
	int32					process_pool_count;
	int32					process_counter;

	struct core_s**	cores;
	int32						core_count;

	int32					dump_to_cycle;
	int32					full_screen;
} vm_t;



process_t*	vm_create_process(vm_t* vm, process_t* parent, int32 pc);
process_t*	vm_add_core(vm_t* vm, core_t* core, int32 offset);
void				vm_reset_process_io_op(process_t* process);
vm_t*				vm_initialize();
void				vm_destroy(vm_t* vm);
void				vm_kill_process_if_no_live(vm_t* vm);
opcode_t* 	vm_get_opcode(vm_t* vm, process_t* process);
void 				vm_clean_dead_process(vm_t* vm);
int 				vm_execute(vm_t* vm, process_t* process);

#define VM_MEMORY_BOUND(vm, offset) {\
while (offset < 0) offset += vm->memory_size;\
offset %= vm->memory_size;\
}


#endif
