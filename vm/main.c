#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core.h"
#include "vm.h"

#include "display/gl/display_gl.h"
#include "debugger/debugger.h"

// #define RENDER_NCURSES
#define RENDER_GL

#ifdef RENDER_NCURSES
	#include <ncurses.h>
#endif


#include "../common/utils.h"
#include "../common/memory_access.h"

struct s_core_load_informations {
	int32	address;
	int32	have_address;
	int32	id;
	char*	file_name;
	core_t*	core;
};


int parse_arguments(vm_t* vm, int ac, char** av) {
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
		else if (strcmp("-bc", av[i]) == 0)
			vm->cycle_barrier = strtol(av[++i], &end, 0);
		else if (strcmp("-dbg", av[i]) == 0)
			vm->step = 1;
		else if (strcmp("-dbg-same", av[i]) == 0)
			vm->dbg_same_window = 1;
		else if (strcmp("-fs", av[i]) == 0)
			vm->full_screen = 1;
		else {
			core_t* core = core_load_from_file(av[i]);
			if (core) {
				core->bound.start = 0;
				core->bound.size = vm->memory_size;
				infos[current_core++].core = core;
			}
		}
	}

	for (i = 0; i < current_core; ++i) {
		if (!infos[i].have_address) {
			infos[i].address = (vm->memory_size/ current_core) * i;
		}
		VM_MEMORY_BOUND(vm, infos[i].address);
		infos[i].core->start_address = infos[i].address;
		infos[i].core->id = infos[i].id;

		vm_add_core(vm, infos[i].core, infos[i].address);
	}

	return current_core;
}

int	 check_core_endianess(vm_t* vm) {
	int i;
	if (vm->core_count > 1) {
		int big_endian = vm->cores[1]->is_big_endian;
		for (i = 2; i < vm->core_count; ++i) {
			if (big_endian != vm->cores[i]->is_big_endian) {
				fprintf(stderr, "cores endianess mismatch, all cores should use same endianess.\n");
				for (i = 1; i < vm->core_count; ++i) {
					fprintf(stderr, "core '%s' is %s endian\n", vm->cores[i]->header->name, vm->cores[i]->is_big_endian ? "BIG" : "little");
				}
				return VM_ERROR_ENDIANESS;
			}
		}
		vm->big_endian = big_endian;
	}
	return VM_OK;
}

void print_winning_core(vm_t* vm) {
	core_t*		winning = NULL;
	int32			count = 0;
	int32			i;

	for (i = 1; i < vm->core_count; ++i) {
		if ((winning == NULL) || (vm->cores[i]->live_last_cycle > winning->live_last_cycle)) {
			winning = vm->cores[i];
		}
	}
	// tie detection
	for (i = 0; i < vm->core_count; ++i) {
		count += winning == vm->cores[i];
	}
	if (count > 1 || winning == NULL || winning == vm->cores[0]) {
		printf("it's a tie !\n");
	}
	else {
		printf("%s win \n", winning->header->name);
	}
}

int main(int ac, char** av) {
	vm_t*					vm;
	display_gl_t*	display = NULL;
	int						bound;
	int						i;
	debugger_t*		debugger = NULL;

	vm = vm_initialize();

	if ( (parse_arguments(vm, ac, av) <= 0) || (check_core_endianess(vm) < 0)) {
		return -1;
	}

	bound = VM_MEMORY_SIZE / (vm->core_count - 1);
	for (i = 0; i < vm->core_count; ++i) {
		vm->cores[i]->bound.start = vm->cores[i]->start_address;
		vm->cores[i]->bound.size = bound;
	}

	memory_access_initialize(is_cpu_big_endian() != vm->big_endian);

#if defined(_DEBUG)
		vm->full_screen = 0;
#endif

#ifdef RENDER_GL
	display = display_gl_initialize(1980, 1080, vm->full_screen);
	if (vm->step != -1) {
		debugger = debugger_init(vm->dbg_same_window ? display_gl_get_window(display) : NULL);
	}
#endif

	while (vm->process_count && !display_gl_should_exit(display)) {
		int32 i;
		int update_display = 0;


		if (vm->cycle_barrier == vm->cycle_total) {
			for (i = 0; i < vm->core_count; ++i) {
				vm->cores[i]->bound.start = 0;
				vm->cores[i]->bound.size = vm->memory_size;
			}
		}

		if (vm->step != 0) {
			vm->cycle_current++;
			vm->cycle_total++;

			int32 process_count = vm->process_count;
			for (i = process_count; i > 0; --i) {
				process_t* process = vm->processes[i - 1];
				process->cycle_wait--;
				if (process->cycle_wait <= 0) {
					vm_reset_process_io_op(process);
					vm_execute(vm, process);
					update_display = 1;
					if (vm->step > 0 && (vm->step_process == NULL || vm->step_process == process) ) {
						vm->step --;
					}
				}
			}

			if (vm->cycle_current > vm->cycle_to_die) {
				vm->cycle_current = 0;
				vm_kill_process_if_no_live(vm);
				vm_clean_dead_process(vm);
			}
		} else {
			update_display = 1;
		}

	#ifdef RENDER_GL
		update_display |= display_gl_update_input(display);
		if (update_display)
		{
			float y = 1;
			y = display_gl_text(display, 0, y, 0xffffffff, "cycle to die   %d", vm->cycle_to_die);
			y = display_gl_text(display, 0, y, 0xffffffff, "live count     %d ", vm->live_count);
			y = display_gl_text(display, 0, y, 0xffffffff, "process count  %d ", vm->process_count);
			y = display_gl_text(display, 0, y, 0xffffffff, "cycle          %d ", vm->cycle_total);
			y = display_gl_text(display, 0, y, 0xffffffff, "barrier        %d ", vm->cycle_barrier);
			for (i = 0; i < vm->core_count; ++i) {
				core_t* core = vm->cores[i];
				char* name = core->header ? core->header->name : "Unknow";
				y = display_gl_text(display, 0, y, 0xffffffff, "%s %d", name, core->live_count);
			}
			display_gl_step(vm, display);
			display_gl_swap(display);
		}

		if (debugger && vm->step != 1) {
			debugger_render(debugger, vm);
		}
	#endif
	}

	print_winning_core(vm);
	if (debugger) {
		debugger_destroy(debugger);
	}
#ifdef RENDER_GL
	display_gl_destroy(display);
#endif
	vm_destroy(vm);
}
