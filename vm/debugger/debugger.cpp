#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include "../vm.h"
}

#include "debugger.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

struct debugger_s {
  GLFWwindow*	window;
	int					window_owner;
  int					current_core;
  int					current_process;
	char*				disasm;
  char*				processes_name_buffer;
  char**			processes_names;
	process_t**	processes;
  int					processes_count;
};

// process name will be it's 32bits id in hex
#define MAX_PROCESS_NAME_SIZE 11

debugger_t *debugger_init(void* window) {
  debugger_t *debugger = (debugger_t *)malloc(sizeof(debugger_t));
  memset(debugger, 0, sizeof(debugger_t));

	if (!window) {
		debugger->window_owner = 1;
		window = (void*) glfwCreateWindow(640, 480, "Debugger", NULL, NULL);
	}
  debugger->window = (GLFWwindow *) window;
  debugger->processes_name_buffer =(char *)malloc(MAX_PROCESS_NAME_SIZE * VM_MAX_PROCESSES);
  memset(debugger->processes_name_buffer, 0, MAX_PROCESS_NAME_SIZE * VM_MAX_PROCESSES);
  debugger->processes_names = (char **)malloc(sizeof(char **) * VM_MAX_PROCESSES);
	debugger->processes = (process_t**)malloc(sizeof(process_t**) * VM_MAX_PROCESSES);
	debugger->disasm = (char*) malloc(10000);
  for (int i = 0; i < VM_MAX_PROCESSES; ++i) {
    debugger->processes_names[i] = debugger->processes_name_buffer + i * MAX_PROCESS_NAME_SIZE;
  }

  // glfwMakeContextCurrent(debugger->window);
  gl3wInit();
  ImGuiIO &io = ImGui::GetIO();

  int widthMM, heightMM;
  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);
  const float dpi = (float)mode->width / ((float)widthMM / 25.4f);
  io.FontGlobalScale = (dpi / 96.0f) * 0.75f;

  ImGui_ImplGlfwGL3_Init(debugger->window, true);
  glfwSwapInterval(0);

  return debugger;
}

void debugger_update_processes(debugger_t *debugger, vm_t *vm) {
  int current_core = debugger->current_core + 1;
  int processes_count = 0;
  if (current_core > 0 && current_core < vm->core_count) {
    core_t *core = vm->cores[current_core];
    for (int i = 0; i < vm->process_count; ++i) {
      process_t *process = vm->processes[i];
      if (process->core == core) {
        sprintf(debugger->processes_names[processes_count], "0x%0.8x", process->internal_id);
				debugger->processes[processes_count] = process;
				processes_count++;
      }
    }
  }
  debugger->processes_count = processes_count;
}

void disasm(debugger_t* debugger, vm_t* vm, process_t* process, int instructions_count) {
	int pc = process->next_pc;
	int i, j;
	bound_t bound;

	bound.start = 0;
	bound.size = VM_MEMORY_SIZE;


	*debugger->disasm = 0;
	for (i = 0; i < instructions_count; ++i) {
		int8			opcode;
		int8			types;
		long			val;
		char			number[32];
		opcode_t* instruction;

		opcode = vm->memory[pc++ % VM_MEMORY_SIZE];
		instruction = holberton_core_get_opcodes();
		while ((instruction->opcode != 0) && (instruction->opcode != opcode))
			instruction++;
		if (instruction->mnemonic) {
			strcat(debugger->disasm, instruction->mnemonic);
			strcat(debugger->disasm, " ");
			if ((instruction->processing_flags & ASM_PROCESSING_NO_TYPES)) {
				types = instruction->arg_types[0] << 6;
			} else {
				types = vm->memory[pc++ % VM_MEMORY_SIZE];
			}

			for (j = 0; j < instruction->arg_count; ++j) {
				if (j > 0) {
					strcat(debugger->disasm, ", ");
				}

				int8 type = TYPE(types, j);
				if (type == CORE_ARG_TYPE_REG) {
					val = vm->memory[pc++ % VM_MEMORY_SIZE] + 1;
					strcat(debugger->disasm, "r");

				} else if (type == CORE_ARG_TYPE_IMM) {
					strcat(debugger->disasm, "%");
					if (instruction->processing_flags & ASM_PROCESSING_IMM_AS_ADD) {
						type = CORE_ARG_TYPE_ADD;
					} else {
						val = memory_read32(vm->memory, pc, &bound, NULL);
						pc += 4;
					}
				}

				if (type == CORE_ARG_TYPE_ADD) {
					val = memory_read16(vm->memory, pc, &bound, NULL);
					pc += 2;
				}

				sprintf(number, "%ld", val);
				strcat(debugger->disasm, number);
			}
		} else {
			strcat(debugger->disasm, "nop");
		}

		strcat(debugger->disasm, "\n");
	}
}

void debugger_render(debugger_t *debugger, vm_t *vm) {
  bool show_test_window = true;

  glfwMakeContextCurrent(debugger->window);
  ImVec4 clear_color = ImColor(0, 0, 0);

  char *cores_name[VM_MAX_CORES];
  for (int i = 1; i < vm->core_count; ++i) {
    cores_name[i - 1] = vm->cores[i]->header->name;
  }
  debugger_update_processes(debugger, vm);
  ImGui_ImplGlfwGL3_NewFrame();
  {
		bool	opened = true;
		int width, height;
		int flag = 0;
		if (debugger->window_owner) {
			glfwGetFramebufferSize(debugger->window, &width, &height);
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2((float)width, (float)height));
			flag = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		}
		ImGui::Begin("HolbertonWar", &opened, flag);

			ImGui::ListBox("cores", &debugger->current_core,
                       (const char **)cores_name, vm->core_count - 1, 8);
			ImGui::SameLine();
			ImGui::ListBox("processes", &debugger->current_process,
											 (const char **)debugger->processes_names,
											 debugger->processes_count, 8);

			if (ImGui::Button("step")) {
				vm->step = 1;
			}
			ImGui::SameLine();
			if (ImGui::Button("continue")) {
				vm->step = -1;
			}
			ImGui::SameLine();
			if (ImGui::Button("pause")) {
				vm->step = 0;
			}
			if (debugger->current_process < debugger->processes_count && debugger->current_process >= 0) {
				bool pin = false;
				process_t* process = debugger->processes[debugger->current_process];
				disasm(debugger, vm, process, 14);
				ImGui::BeginChild("proc", ImVec2(700, 400), true);
					ImGui::BeginChild("Proc", ImVec2(400, 0));
						ImGui::Checkbox("Pin", &pin);
						ImGui::Text(" PC %0.8X ", process->pc);
						ImGui::SameLine();
						ImGui::Text("ZF %d", process->zero);
						ImGui::Text(" WT %d", process->cycle_wait);
						for (int i = 0; i < 16; ++i) {
							if ((i) % 2) {
								ImGui::SameLine();
							}
							ImGui::Text("R%0.2d %0.8X ", i + 1, process->reg[i]);
						}
					ImGui::EndChild();
					ImGui::SameLine();
					ImGui::BeginChild("asm", ImVec2(300, 0));
						ImGui::InputTextMultiline("", debugger->disasm, 10000, ImVec2(-1.0f, ImGui::GetTextLineHeight() * 15), ImGuiInputTextFlags_ReadOnly);
						vm->step_process = process;
					ImGui::EndChild();
				ImGui::EndChild();
			}
    // ImGui::ListBox("processes", &debugger->current_process, [](void* data,
    // int idx, const char** out) -> bool {}, vm->cores[debugger->current_core]-
    // 1, -1);


    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
  }
  int display_w, display_h;
  glfwGetFramebufferSize(debugger->window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
	if (debugger->window_owner) {
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
	}
  ImGui::Render();
	if (debugger->window_owner) {
		glfwSwapBuffers(debugger->window);
	}
}

void debugger_destroy(debugger_t *debugger) {
	if (debugger->window_owner) {
		glfwDestroyWindow(debugger->window);
	}
	free(debugger->processes_name_buffer);
	free(debugger->disasm);
	free(debugger->processes);
  free(debugger);
}
