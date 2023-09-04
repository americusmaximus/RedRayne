vs.1.1
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3
dp4 r0.z, v0, c2
add r0.z, r0.z, -c6.x
mul r0.z, r0.z, c6.y
max r0.z, r0.z, c5.x
min r0.z, r0.z, c5.y
add oFog.x, c5.y, -r0.z
mov oT0, v7
mov oT1, v7
mov oT2, v7
mov oT3, v8
mov r1, v2
mul r0.xyz, v1.yzxx, r1.zxyy
mad r0.xyz, -v1.zxyy, r1.yzxx, r0.xyzz
dp3 r0.w, r0, r0
rsq r0.w, r0.w
mul r0, r0, r0.w
mov r1, v0
add r1, c9, -r1
dp3 r1.w, r1, r1
rsq r1.w, r1.w
mul r1, r1, r1.w
add r2, v4, r1
dp3 r2.w, r2, r2
rsq r2.w, r2.w
mul r2, r2, r2.w
dp3 r1.x, r2, v1
dp3 r1.y, r2, v2
dp3 r1.z, r2, r0
dp3 r2.x, v3, r0
sge r3.x, r2.x, c5.x
add r3.x, r3.x, r3.x
add r3.x, r3.x, -c5.y
mul r1.z, r1.z, r3.x
mad oD0.xyz, r1, c5.z, c5.z
mov oD0.w, c5.w