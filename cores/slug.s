; Some kind of torpedo

.name Slug

sti r1, :live, 2

ld %:live,r2
ldi :live, 4,r3
ldi :live, 8,r4

:start
st  r2, :write_here
sti r3, :write_here, 4
sti r4, :write_here, 8

fork :write_here
:live
live 0xff00ff00
jmp :start

:write_here
