vs.1.1
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3
dp3 r2.x, v3, c4
dp3 r2.y, v3, c20
dp3 r2.z, v3, c22
mov r2.w, c7
max r2, r2, c24
dp4 r1.x, r2, c8
dp4 r1.y, r2, c21
dp4 r1.z, r2, c23
mov r1.w, c5.w
mov oD0, r1
dp4 r0.x, v0, c15
dp4 r0.y, v0, c16
dp4 r0.z, v0, c17
dp4 r0.w, v0, c18
mov r1, r0
mov oT0, r1
mov oT1, r1
dp4 r0.z, v0, c2
add r0.z, r0.z, -c6.x
mul r0.z, r0.z, c6.y
max r0.z, r0.z, c5.x
min r0.z, r0.z, c5.y
add oFog.x, c5.y, -r0.z