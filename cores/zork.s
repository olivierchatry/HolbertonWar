#
# ex.s for corewar
#
# Alexandre David
# Sat Nov 10 22:24:30 2201
#
.name "zork"
.comment "just a basic living prog"
l2:
sti r1,%:live,%1
and r1,%0,r1
live: live %1
zjmp %:live
l3:

# compiled executable :
#
# 0x0b,0x68,0x01,0x00,0x0f,0x00,0x01
# 0x06,0x64,0x01,0x00,0x00,0x00,0x00,0x01
# 0x01,0x00,0x00,0x00,0x01
# 0x09,0xff,0xfb
