.name "Beghaiiiiiiiiiii"
.comment "Champion par delign_b et choizi_d"

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
bh2_:	live	%42
	zjmp	%:bh1_

st_bc2:	st	r1,6
	live    %42
	ld	%589824,r9
	st	r1,6
stbc_:	live	%42
rpc1:	ld	%-510,r10
	ld	%-25,r14
	fork	%:bh1
	ld	%0,r15
	zjmp	%:stbc_
st_bc:	sti	r1,%:bh1_,%1
	sti	r1,%:bh2_,%1
	st	r1,6
st_bc1:	live    %42
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
	fork	%:new_st
	st	r1,6
	live	%42
	fork	%:live
	st	r1,6
	live	%42
	fork	%:st2
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
	fork	%:st2
	live	%42
	fork	%:st_tp2
	ld	%0,r15
	zjmp	%:st2

live:	ld	%0,r15
	st	r1,6	
	live	%42
	fork	%:live1
	st	r1,6	
	live	%42
	fork	%:live2
	zjmp	%:live
	
new_st:	st	r1,6
	live	%42
	fork	%:spe_tp
	st	r1,6
st_tp:	live	%42
	fork	%:go_tp
	st	r1,6
	live	%42
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
	fork	%:st_tp2
	st	r1,6
	live	%42
	fork	%:st2tp1
	zjmp	%:st_tp2
	
#====================================== TORP 1 =============================
		
gogo:	st	r1,6
	live	%42
	ld	%1703936,r2
	ld	%393216,r3
	sti     r1,%:tp1,%1
	sti     r1,%:tp_i,%1
	sti	r2,%:rplc,%1
	sti	r3,%:rplc2,%1

spe_tp:	ld	%8,r5
	ld	%1,r6
	ld	%0,r15
	zjmp	%:go_tp1

go_tp:	ld	%-1,r5
	ld	%1,r6
go_tp1:	ld	%33554432,r14
	ld	%655360,r8
	ld	%517,r12
	ld	%427,r9		# Revoir CA Possibilite to improve
	ld	%10,r7
	st	r1,6
tp:	live	%42
	sub	r5,r6,r5
	zjmp    %:gogo
	sub	r14,r8,r14
	sub	r12,r7,r12
	xor	r12,r9,r15
	zjmp	%:go_tp1
	fork	%:go_tp2
	ld	%0,r11
rplc:	zjmp	%:tp		#a remplace par 23
	zjmp	%:tp1		# merde pour pas qu il ecrase nimporte quoi

go_tp2:	st	r1,6
	live	%42
	fork	%:tp		#ou go_tp1
	ld	%0,r15
rplc2:	zjmp	%:go_tp2	#a remplace par 3
	zjmp	%:tp1		# merde pour pas qu il ecrase nimporte quoi

tp1:	live	%42
	fork	%:tp1
	sti 	r1,%:tp_i,%1
	live	%42
	fork	%:tp2
	sti	r14,%:tp_l,%6
	ldi	%:tp_l,%0,r10
	ld	%0, r11
	live	%42
	zjmp	%:tp_i

tp2:	sti	r14,%:tp_l,%6
	ldi	%:tp_l,%4,r10
	ld	%4, r11
	ld	%0, r15
	zjmp    %:tp_i

tp_i:	live	%42
	ldi	%0, %0, r15
	lld	%0, r15
	sti	r14, %:tp_l, %6		

tp_l:	sti	r10,r11,r12
	zjmp	%502
	zjmp	%:tp1		# merde pour pas qu il ecrase nimporte quoi

#===========================================================================

live2:	ld	%0,r15
	st	r1,6	
	live	%42
	fork	%:live21
	zjmp	%:live2
live21:	st	r1,11
	st	r1,11
live22:	live	%42
	live	%42
	zjmp	%:live22

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