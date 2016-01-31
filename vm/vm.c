#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vm.h"
#include "core.h"
#include "../common/memory_access.h"

vm_t* vm_initialize() {
	vm_t* vm = (vm_t*) malloc(sizeof(vm_t));

	vm->memory = (int8*) malloc(VM_MEMORY_SIZE);
	vm->memory_size = VM_MEMORY_SIZE;

	vm->cycle_current = 0;
	vm->cycle_to_die = VM_CYCLE_TO_DIE;
	vm->cycle_delta = VM_CYCLE_DELTA;
	vm->cycle_total = 0;
	vm->live_count = 0;
	vm->live_total = 0;
	vm->process_count = 0;
	vm->process_counter = 0;

	vm->processes = (process_t**) malloc(sizeof(process_t*) * VM_MAX_PROCESSES);
	vm->process_count = 0;

	vm->processes_pool = (process_t**)malloc(sizeof(process_t*) * VM_MAX_PROCESSES);
	vm->process_pool_count = 0;

	vm->core_count = 0;
	vm->cores = (core_t**)malloc(sizeof(core_t*) * VM_MAX_CORES + 1);

	// core[0] is "unknown" core, used when player "live" with a unknown id.
	vm->cores[vm->core_count] = malloc(sizeof(core_t));
	vm->cores[vm->core_count++]->live_count = 0;

	return vm;
}

void vm_destroy(vm_t* vm) {
	int32 i = 0;
	free(vm->memory);

	for (; i < vm->process_count; ++i) {
		if (!vm->processes[i]->free) {
			free(vm->processes[i]);
		}
	}

	for (i = 0; i < vm->process_pool_count; ++i) {
		free(vm->processes_pool[i]);
	}

	for (i = 0; i < vm->core_count; ++i) {
		core_destroy(vm->cores[i]);
	}

	free(vm->cores);
	free(vm->processes);
	free(vm->processes_pool);

	free(vm);
}

process_t*	vm_create_process(vm_t* vm, process_t* parent, int32 pc) {
	process_t* process = NULL;

	if (vm->process_count >= VM_MAX_PROCESSES) {
		return NULL;
	}

	if (vm->process_pool_count > 0) {
		process = vm->processes_pool[--vm->process_pool_count];
	}
	else {
		process = malloc(sizeof(process_t));
	}


	if (parent) {
		memcpy(process, parent, sizeof(process_t));
		memory_bound(pc, &process->core->bound);
	}
	else {
		memset(process, 0, sizeof(process_t));
	}


	process->pc = pc;
	process->internal_id = vm->process_counter++;
	process->cycle_live = vm->cycle_total;
	process->memory_write_op = 0;
	process->memory_read_op = 0;
	process->jump = 0;
	// new process always wait 1 more cycles.
	process->cycle_wait ++;

	process->cycle_create = vm->cycle_total;

	vm->processes[vm->process_count++] = process;

	return process;
}

void vm_destroy_process(vm_t* vm, process_t* process) {
	vm->processes_pool[vm->process_pool_count++] = process;
	process->free = 1;
}

process_t*	vm_add_core(vm_t* vm, core_t* core, int32 offset) {
	int8* core_code = core_get_code_ptr(core);
	process_t* process = NULL;

	if (vm->core_count < VM_MAX_CORES) {
		memcpy(vm->memory + offset, core_code, core->header->code_size);
		process = vm_create_process(vm, NULL, offset);
		process->reg[0] = core->id;
		process->core = core;
		vm->cores[vm->core_count++] = core;
	}

	return process;
}

void	vm_reset_process_io_op(process_t* process) {
	process->memory_read_op = 0;
	process->memory_write_op = 0;
	process->jump = 0;
}

void vm_clean_dead_process(vm_t* vm) {
	int32 current = 0;
	int32 previous = 0;

	for (; previous < vm->process_count; ++previous) {
		if (vm->processes[previous]->free == 0) {
			vm->processes[current++] = vm->processes[previous];
		}
	}
	vm->process_count = current;
}

void	vm_kill_process_if_no_live(vm_t* vm) {
	int32 i = 0;
	for (; i < vm->process_count; ++i) {
		if (vm->processes[i]->cycle_live == 0) {
			vm_destroy_process(vm, vm->processes[i]);
		}
		else {
			vm->processes[i]->cycle_live = 0;
		}
	}
}


opcode_t* vm_get_opcode(vm_t* vm, process_t* process) {
	opcode_t* temp = holberton_core_get_opcodes();
	int8 opcode = vm->memory[process->pc % vm->memory_size];

	while ((temp->opcode != 0) && (temp->opcode != opcode))
		temp++;

	process->current_opcode = temp;
	process->cycle_wait = temp->cycles;

	return temp;
}


int 				vm_check_opcode(vm_t* vm, process_t* process, int* args, int* regs, int modulo) {
	int i = 0;
	int map[4] = {0, OP_ARG_TYPE_REG, OP_ARG_TYPE_IMM, OP_ARG_TYPE_ADD};
	if (process->current_opcode->opcode) {
		int32 pc = process->pc + 1;
		int8 encoding;

		encoding = vm->memory[pc++];

		for (; i < process->current_opcode->arg_count; ++i) {
			int encode = TYPE(encoding, i);

			int type = map[ encode ];
			if ( (type & process->current_opcode->arg_types[i]) == 0 )
				return VM_ERROR_ENCODING;
			if ( type == OP_ARG_TYPE_IMM ) {
				args[i] = memory_read32(vm->memory, pc, &process->core->bound);
				regs[i] = -1;
				pc += 4;
			}
			else if ( type == OP_ARG_TYPE_ADD ) {
				regs[i] = memory_read16(vm->memory, pc, &process->core->bound);
				args[i] = process->pc + regs[i] % modulo;
				if (args[i] < 0) args[i] += vm->memory_size;
				args[i] = memory_read32(vm->memory, args[i], &process->core->bound);
				pc += 2;
			}
			else {
				int8 reg = vm->memory[pc++];
				if (reg <= 0 || reg > CORE_REGISTER_COUNT)
					return VM_ERROR_REGISTER;
				args[i] = process->reg[reg - 1];
				regs[i] = reg - 1;
			}
		}
		process->pc = pc;
		memory_bound(process->pc, &process->core->bound);
		return VM_OK;
	}
	return VM_ERROR_OPCODE;
}

void	vm_live(vm_t* vm, int32 id)
{
	int32 i;

	vm->live_count++;

	if ((vm->live_count % VM_LIVE_COUNT) == 0)
	{
		vm->cycle_to_die -= vm->cycle_delta;
	}

	for (i = 1; i < vm->core_count; ++i)
	{
		if (vm->cores[i]->id == id)
		{
			vm->cores[i]->live_count++;
			vm->cores[i]->live_last_cycle = vm->cycle_total;
			return;
		}
	}

	vm->cores[0]->live_count++;
	vm->cores[0]->live_last_cycle = vm->cycle_total;
}

int 				vm_execute(vm_t* vm, process_t* process) {
	int32				pc;
	int32   		offset = 2;
	int32				args[4], regs[4];
	int32				addr;
	/*
	for (int i =0; i < 16; ++i) {
		printf("r%d=%.8X ", i + 1, process->reg[i]);
	}
	printf("\n");
	for (int i =0; i < 80; ++i) {
		printf("%.2X ", (unsigned char) vm->memory[i]);
	}
	printf("\n\n");
	*/

	process->pc = memory_bound(process->pc, &process->core->bound);
	pc = process->pc;
	opcode_t* op = vm_get_opcode(vm, process);
	int32	ret = vm_check_opcode(vm, process, args, regs, VM_MEMORY_MODULO);

	if (ret == VM_OK) {
		switch(op->opcode) {
		case 0x01:
			process->reg[regs[2]] = args[0] + args[1];
			process->zero = process->reg[regs[2]] == 0;
			break;
		case 0x02:
			process->reg[regs[2]] = args[0] - args[1];
			process->zero = process->reg[regs[2]] == 0;
			break;
		case 0x03:
			process->reg[regs[2]] = args[0] & args[1];
			process->zero = process->reg[regs[2]] == 0;
			break;
		case 0x04:
			process->reg[regs[2]] = args[0] | args[1];
			process->zero = process->reg[regs[2]] == 0;
			break;
		case 0x05:
			process->reg[regs[2]] = args[0] ^ args[1];
			process->zero = process->reg[regs[2]] == 0;
			break;
		case 0x06:
			if (process->zero) {
				break;
			}
		case 0x07:
			addr = pc + args[0] % VM_MEMORY_MODULO;
			process->pc = memory_bound(addr, &process->core->bound);
			break;
		case 0x08:
			process->reg[regs[1]] = args[0];
			process->zero = args[0] == 0;
			break;
		case 0x09:
			addr = pc + args[1] % VM_MEMORY_MODULO;
			memory_write32(args[0], vm->memory, addr, &process->core->bound);
			process->zero = args[0] == 0;
			break;
		case 0x0a:
			vm_create_process(vm, process, pc + args[0] % VM_MEMORY_MODULO);
			break;
		case 0x0b:
			break;
		case 0x0c:
			vm_live(vm, args[0]);
			break;
		case 0x0d:
			addr = pc + (args[0] + args[1]) % VM_MEMORY_MODULO;
			process->reg[regs[2]] = memory_read32(vm->memory, addr, &process->core->bound);
			process->zero = process->reg[regs[2]] == 0;
			break;
		case 0x0e:
			addr = pc + (args[1] + args[2]) % VM_MEMORY_MODULO;
			memory_write32(args[0], vm->memory, addr, &process->core->bound);
			process->zero = args[0] == 0;
			break;
		case 0x0f:
			process->reg[regs[0]] = vm->cycles_barrier;
			process->zero = vm->cycles_barrier == 0;
			break;
		default:
			// ret = VM_ERROR_OPCODE;
			break;
		}
		// process->pc = process->pc + 1;
	}

	if (ret != VM_OK) {
		exit(0);
		process->pc = process->pc + 1;
	}

	return ret;
}
