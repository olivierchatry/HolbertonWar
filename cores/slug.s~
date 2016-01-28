; Simple fork bomb, do nothing.

.name Slug

ld %0b0c0d0e,r2
:start
st :start, -64
st r2, 64
fork 0 ; simple fork bomb
live r1

jmp start
