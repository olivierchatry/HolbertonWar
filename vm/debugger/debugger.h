#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

struct debugger_s;
typedef struct debugger_s debugger_t;

struct vm_s;
typedef struct vm_s vm_t;

#ifdef __cplusplus
extern "C" {
#endif

	debugger_t*   debugger_init(void* window);
	void          debugger_render(debugger_t* debugger, vm_t* vm);
	void          debugger_destroy(debugger_t* debugger);
#ifdef __cplusplus
}
#endif

#endif
