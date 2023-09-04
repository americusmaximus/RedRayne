vs.1.1
mov a0.x, v6.x
dp4 r0.x, v0, c0[a0.x]
dp4 r0.y, v0, c1[a0.x]
dp4 r0.z, v0, c2[a0.x]
dp3 r4.x, v5, c0[a0.x]
dp3 r4.y, v5, c1[a0.x]
dp3 r4.z, v5, c2[a0.x]
mov a0.x, v6.y
dp4 r1.x, v1, c0[a0.x]
dp4 r1.y, v1, c1[a0.x]
dp4 r1.z, v1, c2[a0.x]
dp3 r5.x, v5, c0[a0.x]
dp3 r5.y, v5, c1[a0.x]
dp3 r5.z, v5, c2[a0.x]
mov a0.x, v6.z
dp4 r2.x, v2, c0[a0.x]
dp4 r2.y, v2, c1[a0.x]
dp4 r2.z, v2, c2[a0.x]
dp3 r6.x, v5, c0[a0.x]
dp3 r6.y, v5, c1[a0.x]
dp3 r6.z, v5, c2[a0.x]
mov a0.x, v6.w
dp4 r3.x, v3, c0[a0.x]
dp4 r3.y, v3, c1[a0.x]
dp4 r3.z, v3, c2[a0.x]
dp3 r7.x, v5, c0[a0.x]
dp3 r7.y, v5, c1[a0.x]
dp3 r7.z, v5, c2[a0.x]
mul r0.xyz, r0.xyzz, v4.x
mul r1.xyz, r1.xyzz, v4.y
mul r2.xyz, r2.xyzz, v4.z
mul r3.xyz, r3.xyzz, v4.w
mul r4.xyz, r4.xyzz, v4.x
mul r5.xyz, r5.xyzz, v4.y
mul r6.xyz, r6.xyzz, v4.z
mul r7.xyz, r7.xyzz, v4.w
add r0.xyz, r0.xyzz, r1.xyzz
add r0.xyz, r0.xyzz, r2.xyzz
add r0.xyz, r0.xyzz, r3.xyzz
mov r0.w, c5.y
add r4.xyz, r4.xyzz, r5.xyzz
add r4.xyz, r4.xyzz, r6.xyzz
add r4.xyz, r4.xyzz, r7.xyzz
dp4 oPos.x, r0, c0
dp4 oPos.y, r0, c1
dp4 oPos.z, r0, c2
dp4 oPos.w, r0, c3
dp3 r2.x, r4, c4
dp3 r2.y, r4, c20
dp3 r2.z, r4, c22
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