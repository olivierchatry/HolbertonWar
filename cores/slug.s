; Slug want to live

.name Slug

sti r1, :live, 2
:live
live 1
ldi :live, 2,  r2
sti r2, :live2, 2
:live2
live 1

jmp :live
