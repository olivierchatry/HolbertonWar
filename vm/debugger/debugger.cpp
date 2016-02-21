#include <stdlib.h>
#include <stdio.h>

#include "debugger.h"
#include "../vm.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

struct debugger_s {
	GLFWwindow*	window;
	int					current_core;
	int					current_process;

	char*				processes_name_buffer;
	char**			processes_names;
	int					processes_count;

};

// process name will be it's 32bits id in hex
#define MAX_PROCESS_NAME_SIZE 11

debugger_t*   debugger_init() {
	debugger_t* debugger = (debugger_t*) malloc(sizeof(debugger_t));
	memset(debugger, 0, sizeof(debugger_t));

	debugger->window = glfwCreateWindow(640, 480, "Debugger", NULL, NULL);
	debugger->processes_name_buffer = (char*) malloc(MAX_PROCESS_NAME_SIZE * VM_MAX_PROCESSES);
	memset(debugger->processes_name_buffer, 0, MAX_PROCESS_NAME_SIZE * VM_MAX_PROCESSES);
	debugger->processes_names = (char**)malloc(sizeof(char**) * VM_MAX_PROCESSES);
	
	for (int i = 0; i < VM_MAX_PROCESSES; ++i) {
		debugger->processes_names[i] = debugger->processes_name_buffer + i * MAX_PROCESS_NAME_SIZE;
	}

	
	glfwMakeContextCurrent(debugger->window);
	gl3wInit();
	ImGuiIO& io = ImGui::GetIO();

	int widthMM, heightMM;
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	const float dpi = (float)mode->width / ((float)widthMM / 25.4f);
	io.FontGlobalScale = dpi / 96.0f;

	ImGui_ImplGlfwGL3_Init(debugger->window, true);
	glfwSwapInterval(0);

	return debugger;
}


void					debugger_update_processes(debugger_t* debugger, vm_t* vm) {
	int current_core = debugger->current_core + 1;
	int process_count = 0;
	if (current_core > 0 && current_core < vm->core_count) {
		core_t* core = vm->cores[current_core];
		for (int i = 0; i < vm->process_count; ++i) {
			process_t* process = vm->processes[i];
			if (process->core == core) {
				sprintf(debugger->processes_names[process_count++], "0x%0.8x", process->internal_id);
			}
		}
	}
	debugger->processes_count = process_count;

}

void          debugger_render(debugger_t* debugger, vm_t* vm) {
	bool show_test_window = true;

	glfwMakeContextCurrent(debugger->window);
	ImVec4			clear_color = ImColor(0, 0, 0);

	char*				cores_name[VM_MAX_CORES];
	for (int i = 1; i < vm->core_count; ++i) {
		cores_name[i - 1] = vm->cores[i]->header->name;
	}
	debugger_update_processes(debugger, vm);
	ImGui_ImplGlfwGL3_NewFrame();
	{
		static float f = 0.0f;
		ImGui::BeginChild("Cores");
		ImGui::ListBox("cores", &debugger->current_core, (const char**) cores_name, vm->core_count - 1, -1);
		ImGui::ListBox("processes", &debugger->current_process, (const char**) debugger->processes_names, debugger->processes_count, -1);

		// ImGui::ListBox("processes", &debugger->current_process, [](void* data, int idx, const char** out) -> bool {}, vm->cores[debugger->current_core]- 1, -1);
		ImGui::EndChild();

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	int display_w, display_h;
	glfwGetFramebufferSize(debugger->window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui::Render();
	glfwSwapBuffers(debugger->window);
}

void          debugger_destroy(debugger_t* debugger) {

	glfwDestroyWindow(debugger->window);
	free(debugger);
}
