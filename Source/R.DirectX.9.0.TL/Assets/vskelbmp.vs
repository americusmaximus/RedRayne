vs.1.1
dcl_position1 v15
dcl_position v0
dcl_blendweight v1
dcl_blendindices v2
dcl_normal v3
dcl_psize v4
dcl_color v5
dcl_color1 v6
dcl_texcoord v7
dcl_texcoord1 v8
dcl_texcoord2 v9
mov a0.x, v6.x
dp4 r0.x, v0, c0[a0.x]
dp4 r0.y, v0, c1[a0.x]
dp4 r0.z, v0, c2[a0.x]
dp3 r1.x, v5, c0[a0.x]
dp3 r1.y, v5, c1[a0.x]
dp3 r1.z, v5, c2[a0.x]
dp3 r2.x, v8, c0[a0.x]
dp3 r2.y, v8, c1[a0.x]
dp3 r2.z, v8, c2[a0.x]
dp3 r3.x, v9, c0[a0.x]
dp3 r3.y, v9, c1[a0.x]
dp3 r3.z, v9, c2[a0.x]
mov a0.x, v6.y
dp4 r4.x, v1, c0[a0.x]
dp4 r4.y, v1, c1[a0.x]
dp4 r4.z, v1, c2[a0.x]
dp3 r5.x, v5, c0[a0.x]
dp3 r5.y, v5, c1[a0.x]
dp3 r5.z, v5, c2[a0.x]
dp3 r6.x, v8, c0[a0.x]
dp3 r6.y, v8, c1[a0.x]
dp3 r6.z, v8, c2[a0.x]
dp3 r7.x, v9, c0[a0.x]
dp3 r7.y, v9, c1[a0.x]
dp3 r7.z, v9, c2[a0.x]
mul r0.xyz, r0.xyzz, v4.x
mul r1.xyz, r1.xyzz, v4.x
mul r2.xyz, r2.xyzz, v4.x
mul r3.xyz, r3.xyzz, v4.x
mul r4.xyz, r4.xyzz, v4.y
mul r5.xyz, r5.xyzz, v4.y
mul r6.xyz, r6.xyzz, v4.y
mul r7.xyz, r7.xyzz, v4.y
add r0.xyz, r0.xyzz, r4.xyzz
add r1.xyz, r1.xyzz, r5.xyzz
add r2.xyz, r2.xyzz, r6.xyzz
add r3.xyz, r3.xyzz, r7.xyzz
mov a0.x, v6.z
dp4 r4.x, v2, c0[a0.x]
dp4 r4.y, v2, c1[a0.x]
dp4 r4.z, v2, c2[a0.x]
dp3 r5.x, v5, c0[a0.x]
dp3 r5.y, v5, c1[a0.x]
dp3 r5.z, v5, c2[a0.x]
dp3 r6.x, v8, c0[a0.x]
dp3 r6.y, v8, c1[a0.x]
dp3 r6.z, v8, c2[a0.x]
dp3 r7.x, v9, c0[a0.x]
dp3 r7.y, v9, c1[a0.x]
dp3 r7.z, v9, c2[a0.x]
mul r4.xyz, r4.xyzz, v4.z
mul r5.xyz, r5.xyzz, v4.z
mul r6.xyz, r6.xyzz, v4.z
mul r7.xyz, r7.xyzz, v4.z
add r0.xyz, r0.xyzz, r4.xyzz
add r1.xyz, r1.xyzz, r5.xyzz
add r2.xyz, r2.xyzz, r6.xyzz
add r3.xyz, r3.xyzz, r7.xyzz
mov a0.x, v6.w
dp4 r4.x, v3, c0[a0.x]
dp4 r4.y, v3, c1[a0.x]
dp4 r4.z, v3, c2[a0.x]
dp3 r5.x, v5, c0[a0.x]
dp3 r5.y, v5, c1[a0.x]
dp3 r5.z, v5, c2[a0.x]
dp3 r6.x, v8, c0[a0.x]
dp3 r6.y, v8, c1[a0.x]
dp3 r6.z, v8, c2[a0.x]
dp3 r7.x, v9, c0[a0.x]
dp3 r7.y, v9, c1[a0.x]
dp3 r7.z, v9, c2[a0.x]
mul r4.xyz, r4.xyzz, v4.w
mul r5.xyz, r5.xyzz, v4.w
mul r6.xyz, r6.xyzz, v4.w
mul r7.xyz, r7.xyzz, v4.w
add r0.xyz, r0.xyzz, r4.xyzz
add r8.xyz, r2.xyzz, r6.xyzz
add r9.xyz, r3.xyzz, r7.xyzz
add r7.xyz, r1.xyzz, r5.xyzz
mov r0.w, c5.y
dp4 oPos.x, r0, c0
dp4 oPos.y, r0, c1
dp4 oPos.z, r0, c2
dp4 oPos.w, r0, c3
dp4 r0.z, v0, c2
add r0.z, r0.z, -c6.x
mul r0.z, r0.z, c6.y
max r0.z, r0.z, c5.x
min r0.z, r0.z, c5.y
add oFog /* removed swizzle */, c5.y, -r0.z
mov oT0, v7
mov oT1, v7
dp3 r7.w, r7, r7
rsq r7.w, r7.w
mul r7, r7, r7.w
dp3 r8.w, r8, r8
rsq r8.w, r8.w
mul r8, r8, r8.w
dp3 r9.w, r9, r9
rsq r9.w, r9.w
mul r9, r9, r9.w
mov r1, r9
mul r0.xyz, r8.yzxx, r1.zxyy
mad r0.xyz, -r8.zxyy, r1.yzxx, r0.xyzz
dp3 r0.w, r0, r0
rsq r0.w, r0.w
mul r0, r0, r0.w
dp3 r1.x, c4, r8
dp3 r1.y, c4, r9
dp3 r1.z, c4, r0
dp3 r2.x, r7, r0
sge r3.x, r2.x, c5.x
add r3.x, r3.x, r3.x
add r3.x, r3.x, -c5.y
mul r1.z, r1.z, r3.x
mul r1, r1, c5.z
add oD0, r1, c5.z
mov oD0.w, c5.w
mov r2.x, c5.x
dp3 r2.y, r7, c20
dp3 r2.z, r7, c22
mov r2.w, c7
max r2, r2, c24
dp3 oD1.x, r2, c8
dp3 oD1.y, r2, c21
dp3 oD1.z, r2, c23
mov oD1.w, c5.w