vs.1.1
dcl_position v0
dcl_normal v3
dcl_texcoord v7
dcl_blendweight v1
dcl_blendindices v2
dcl_texcoord1 v8
dcl_psize v4
mov r3, c0 /* initialize register r3 */
mov r2, c0 /* initialize register r2 */
mov r1, c0 /* initialize register r1 */
mov r0, c0 /* initialize register r0 */
mov oD0, c0 /* initialize output register oD0 */
mov oT3, c0 /* initialize output register oT3 */
mov oT2, c0 /* initialize output register oT2 */
mov oT1, c0 /* initialize output register oT1 */
mov oT0, c0 /* initialize output register oT0 */
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
dp4 r0.z, v0, c2
add r0.z, r0.z, -c6.x
mul r0.z, r0.z, c6.y
max r0.z, r0.z, c5.x
min r0.z, r0.z, c5.y
add oFog /* removed swizzle */, c5.y, -r0.z
mov r1, v2
mul r0.xyz, v1.yzxx, r1.zxyy
mad r0.xyz, -v1.zxyy, r1.yzxx, r0.xyzz
dp3 r0.w, r0, r0
rsq r0.w, r0.w
mul r0, r0, r0.w
dp3 r2.x, v3, r0
sge r3.x, r2.x, c5.x
add r3.x, r3.x, r3.x
add r3.x, r3.x, -c5.y
mul r0, r0, r3.x
mov r2, v0
add r2, c9, -r2
dp3 r1.x, r2, c12
dp3 r1.y, r2, c13
dp3 r1.z, r2, c14
mov oT0, v7
dp3 oT1.x, v1, c12
dp3 oT1.y, v2, c12
dp3 oT1.z, r0, c12
mov oT1.w, r1.x
dp3 oT2.x, v1, c13
dp3 oT2.y, v2, c13
dp3 oT2.z, r0, c13
mov oT2.w, r1.y
dp3 oT3.x, v1, c14
dp3 oT3.y, v2, c14
dp3 oT3.z, r0, c14
mov oT3.w, r1.z