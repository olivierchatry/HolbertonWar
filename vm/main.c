#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core.h"
#include "vm.h"
#include "display/display.h"
#include "display/display_text.h"
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
		else if (strcmp("-bc", av[i]) == 0)
			vm->cycles_barrier = strtol(av[++i], &end, 0);
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

// #define RENDER_NCURSES
#define RENDER_GL

#ifdef RENDER_NCURSES
	#include <ncurses.h>
#endif

int main(int ac, char** av) {
	vm_t*	vm		= vm_initialize();
	display_t*	display;
	memory_access_initialize(0);

	if (load_cores(vm, ac, av) <= 0) {
		return -1;
	}
	
	int bound = VM_MEMORY_SIZE / (vm->core_count - 1);

	for (int i = 0; i < vm->core_count; ++i) {
		vm->cores[i]->bound.start = vm->cores[i]->start_address;
		vm->cores[i]->bound.size = bound;
	}

	#ifdef RENDER_NCURSES
		initscr();
		raw();
	#endif

	#ifdef RENDER_GL
		display = display_initialize(1980, 1080);
	#endif

	while (vm->process_count && !display_should_exit(display)) {
		int32 i;
		int update_display = 0;

		vm->cycle_current++;
		vm->cycle_total++;

		int32 process_count = vm->process_count;
		for (i = 0; i < process_count; ++i) {
			process_t* process = vm->processes[i];
			process->cycle_wait--;
			if (process->cycle_wait <= 0) {
				vm_reset_process_io_op(process);
				vm_execute(vm, process);
				update_display = 1;
			}
		}

		// printf("cycle %d, process count %d, live count %d, cycle to die %d\n", vm->cycle_total, vm->process_count, vm->live_count, vm->cycle_to_die);
		/*clear_console();*/
	#ifdef RENDER_NCURSES
		{
			int row, col, mem;
			getmaxyx(stdscr,row,col);
	 		mem = 0;
			for (int y = 0; y < row; ++y) {
				for (int x = 0; x < col; x+=3) {
					if (mem < vm->memory_size) {
						mvprintw(y, x, "%.2X ", (unsigned char)vm->memory[mem ++]);
					}
				}
			}
			refresh();
		}
	#endif
		if (vm->cycle_current > vm->cycle_to_die) {
			vm->cycle_current = 0;
			vm_kill_process_if_no_live(vm);
		}

		vm_clean_dead_process(vm);

	#ifdef RENDER_GL
		if (display_update_input(display) || update_display)
		{
			float y = 1;
			y = display_text_add(display_get_text(display), 0, y, 0xffffffff, "live to die    %d", vm->cycle_to_die);
			y = display_text_add(display_get_text(display), 0, y, 0xffffffff, "live count     %d ", vm->live_count);
			y = display_text_add(display_get_text(display), 0, y, 0xffffffff, "process count  %d ", vm->process_count);
			for (int i = 0; i < vm->core_count; ++i) {
				core_t* core = vm->cores[i];
				char* name = core->header ? core->header->name : "Unknow";
				y = display_text_add(display_get_text(display), 0, y, 0xffffffff, "%s %d", name, core->live_count);
			}
			display_step(vm, display);
		}
	#endif
		
	}

#ifdef RENDER_NCURSES
	endwin();
#endif

#ifdef RENDER_GL
	display_destroy(display);
#endif
	vm_destroy(vm);
}
