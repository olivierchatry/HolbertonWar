; Simple fork bomb, do nothing.

.name Slug

sti r1, :live, 2
ld %:live,r2
ldi %:live, 3,r3

:start
st r2, :write_here
sti r3, 4, :write_here

fork :write_here ; simple fork bomb
:live
live 0xff00ff00
jmp :start

:write_here
; well
