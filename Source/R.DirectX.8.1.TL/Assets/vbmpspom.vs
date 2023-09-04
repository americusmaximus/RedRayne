vs.1.1
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3
mov oT0, v7
mov oT1, v7
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
mov r4, v0
add r4, c25, -r4
dp3 r5.x, r4, r4
rsq r5.x, r5.x
mul r4, r4, r5.x
add r2, r4, r1
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
dp4 r0.z, v0, c2
add r0.z, r0.z, -c6.x
mul r0.z, r0.z, c6.y
max r0.z, r0.z, c5.x
min r0.w, r0.z, c5.y
add r0.w, c5.y, -r0.w
mul oD0.w, r0.w, c5.w
mov oFog.x, c5.y
mov r0, v0
add r0.xyz, c25.xyzz, -r0.xyzz
dp3 r0.x, r0, r0
rsq r0.x, r0.x
mul r0.x, r0.x, c25.w
rcp r0.x, r0.x
add r0.xyz, c5.y, -r0.x
max r0.xyz, r0.xyzz, c24
mul oD1.xyz, r0.xyzz, c26.xyzz