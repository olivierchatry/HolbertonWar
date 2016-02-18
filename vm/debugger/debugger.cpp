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
};


debugger_t*   debugger_init() {
	debugger_t* debugger = (debugger_t*) malloc(sizeof(debugger_t));
	debugger->window = glfwCreateWindow(640, 480, "Debugger", NULL, NULL);
	glfwMakeContextCurrent(debugger->window);
	gl3wInit();
	ImGui_ImplGlfwGL3_Init(debugger->window, true);
	glfwSwapInterval(0);
	return debugger;
}

void          debugger_render(debugger_t* debugger, vm_t* vm) {
	bool show_test_window = true;

	glfwMakeContextCurrent(debugger->window);
	ImVec4 clear_color = ImColor(114, 144, 154);
	ImGui_ImplGlfwGL3_NewFrame();
	{
		static float f = 0.0f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&clear_color);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::Render();
	glfwSwapBuffers(debugger->window);
}

void          debugger_destroy(debugger_t* debugger) {

	glfwDestroyWindow(debugger->window);
	free(debugger);
}
