#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core.h"
#include "vm.h"

#include "../common/utils.h"
#include "../common/memory_access.h"

struct s_core_load_informations {
	int32	address;
	int32	have_address;
	int32	id;
	char*	file_name;
	core_t*	core;
};


int load_cores(vm_t* vm, int ac, char** av) {
	struct s_core_load_informations infos[VM_MAX_CORES];
	int32	current_core = 0;
	int32	i;

	for (i = 0; i < VM_MAX_CORES; ++i) {
		infos[i].id = 0xcacacaca + i;
		infos[i].address = -1;
		infos[i].have_address = 0;
		infos[i].file_name = NULL;
	}

	for (i = 1; i < ac; ++i) {
		char* end = NULL;

		if (strcmp("-dump", av[i]) == 0)
			vm->dump_to_cycle = strtol(av[++i], &end, 0);
		else if (strcmp("-n", av[i]) == 0)
			infos[current_core].id = strtol(av[++i], &end, 0);
		else if (strcmp("-a", av[i]) == 0)
			infos[current_core].address = strtol(av[++i], &end, 0), infos[i].have_address = 1;
		else {
			core_t* core = core_load_from_file(av[i]);
			if (core) {
				infos[current_core++].core = core;
			}
		}
	}

	for (i = 0; i < current_core; ++i) {
		if (!infos[i].have_address) {
			infos[i].address = (VM_MEMORY_SIZE / current_core) * i;
		}
		VM_MEMORY_BOUND(infos[i].address);
		vm_add_core(vm, infos[i].core, infos[i].id, infos[i].address);
	}

	return current_core;
}

int main(int ac, char** av) {
	vm_t*	vm		= vm_initialize();

	memory_access_initialize(0);

	if (load_cores(vm, ac, av) <= 0) {
		return -1;
	}

	while (vm->process_count) {
		int32 i;

		vm->cycle_current++;
		vm->cycle_total++;

		int32 process_count = vm->process_count;
		for (i = 0; i < process_count; ++i) {
			process_t* process = vm->processes[i];
			process->cycle_wait--;
			if (process->cycle_wait <= 0) {
				vm_reset_process_io_op(process);
				vm_execute(vm, process);
			}
		}

		if (vm->cycle_current > vm->cycle_to_die) {
			vm->cycle_current = 0;
			vm_kill_process_if_no_live(vm);
		}

		vm_clean_dead_process(vm);
	}

	vm_destroy(vm);
}
