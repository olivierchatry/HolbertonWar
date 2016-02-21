.name "Beghaiiiiiiiiiii"
.comment "Str_to_wordtab c'est pour concatene deux chaines ?? Dixit beghaiiiiiiiii. Champion par delign_b et choizi_d"

	ld	%0,r15
	zjmp	%:st

#================================== bouclier ===================
bh1:	ld	%0,r15
bh1_:	live	%42
	sti	r9,r10,r14
	sti	r9,r10,r14
	sti	r9,r10,r14
	sti	r9,r10,r14
	sti	r9,r10,r14
	sti	r9,r10,r14
	zjmp	%:bh1_
st_bc2:	st	r1,6
	live    %42
	ld	%589824,r9
	st	r1,6
nop:	live	%42
rpc1:	ld	%-510,r10
	ld	%-25,r14
	fork	%:bh1
	ld	%0,r15
	zjmp	%:nop

st_bc:	sti	r1,%:bh1_,%1
	st	r1,6
st_bc1:	live    %42
	ld	%589824,r9
	ld	%-510,r10
	ld	%30,r7
	st	r1,6
st_bc_:	live    %42
	add	r10,r7,r10
	zjmp	%:st_bc1
	sti	r10,%:rpc1,%2
	ld	%0,r15
	zjmp	%:st_bc_
#===============================================================	

st:	st	r1,6
	live	%42
	fork	%:st_bc
	st	r1,6
	live	%42
	fork	%:st1
	st	r1,6
	live	%42
	fork	%:st2
st1:	live	%42
	fork	%:st1
	st	r1,6
	live	%42
	fork	%:st_bc2
	ld	%0,r15
	zjmp	%:st1
	st	r1,6
st2:	ld	%0,r15
	live	%42
	fork	%:st_tp2
	live	%42
	fork	%:live1
	live	%42
	fork	%:st2
	zjmp	%:st2

	st	r1,6
st_tp:	live	%42
	fork	%:st_bc2
	st	r1,6
	live	%42
	fork	%:st_tp
	ld	%0,r15
	zjmp	%:st_tp

live1:	ld	%0,r15
	st	r1,6	
	live	%42
	fork	%:live11
	zjmp	%:live1
live11:	st	r1,11
	st	r1,11
live12:	live	%42
	live	%42
	zjmp	%:live12

st_tp2:	ld	%0,r15
	st	r1,6
	live	%42
	fork	%:st2tp1
	st	r1,6
	live	%42
	fork	%:st_tp2
	zjmp	%:st_tp2
	
#====================================== TORP 2 =============================
	
st2tp1:	sti	r1, %:tp2_l, %1
	ldi	%:tp2_l, %0,r10
	ld	%0, r11
	live	%42
	fork	%:st2tp2
	live	%42
	fork	%:st2tp4
	lldi	0,%0,r15
	zjmp	%3
	ld	%0,r15
	zjmp	%:tp2_l

st2tp2:	ldi	%:tp2_l, %4, r10
	ld	%4, r11
	live	%42
	fork    %:st2tp3
	ldi	0,%0,r15
	zjmp	%3
	zjmp    %:tp2_l

st2tp3:	ldi	%:tp2_l,%12,r10
	ld	%12,r11
	ld	%0,r15
	zjmp	%0

st2tp4:	ldi	%:tp2_l,%8,r10
	ld	%8,r11
	ldi	0,%0,r15
	zjmp	%3
	zjmp	%:tp2_l
	
tp2_l:	live	%42
	sti	r10,r11,%6

#==========================================================