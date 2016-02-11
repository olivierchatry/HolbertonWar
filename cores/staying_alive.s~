.name And we're stayin' alive, stayin' alive

ld 0xcacacaca, r3 ; barrier value
xor r2, r2, r2		; set the default offset for the barrier

:start
sub r3, 1, r3			; make the barrier change a bit, more fun to watch, each new process will get a new value.
ld  4 , r4 					; number of fork per process, make sure it is reset after each fork.

add r2, 4, r2			; make the barrier move, each  new process will get a new value.
sub r2, 128, r9		;
jnz :write_live
xor r2, r2, r2
:write_live
st  r1, 9 				; write id to next live
:alive
live 0xffffffff		; live, real value will be written at runtime.

sti r3, r2, 300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, -300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, 300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, -300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, 300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, -300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, 300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, -300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, 300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, -300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, 300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, -300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, 300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, -300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, 300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, -300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, 300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, -300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, 300		; write barrier, r2 ( per process offset ) + 64
sti r3, r2, -300		; write barrier, r2 ( per process offset ) + 64

sub r4, 1, r4			; remove one to the per-process fork counter.
jnz :fork					; if not zero, let'fork.

ld 1, r4 					; make sure we do not fork again
jmp :alive				; and do the barrier thing again

:fork							; fork !
fork :start
jmp :alive
