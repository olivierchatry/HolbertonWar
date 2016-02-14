#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

struct debugger_s;
typedef struct debugger_s debugger_t;


debugger_t*   debugger_init();
void          debugger_render(vm_t* vm);
void          debugger_destroy(debugger_t* debugger);

#endif
