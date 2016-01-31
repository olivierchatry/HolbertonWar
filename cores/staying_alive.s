.name And we're stayin' alive, stayin' alive

ld 0xcacacaca, r3
xor r2, r2, r2
:start
sub r3, 1, r3
add r2, 4, r2
st  r1, 9 ; write id to next live
:alive
live 0xffffffff
fork :start
sti r3, r2, 64
jmp :alive
