vs.1.1
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3
dp4 r0.x, v0, c15
dp4 r0.y, v0, c16
dp4 r0.z, v0, c17
dp4 r0.w, v0, c18
mov r1, r0
mov oT0, r1
mov oT1, r1
mul r0.x, r1.w, c6.y
min r0.x, r0.x, c5.y
max r0.x, r0.x, c5.x
add oD0.w, c5.y, -r0.x
mov oD0.xyz, c5.y