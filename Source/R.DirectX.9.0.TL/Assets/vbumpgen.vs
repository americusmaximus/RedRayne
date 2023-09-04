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
mov oD1, c0 /* initialize output register oD1 */
mov oD0, c0 /* initialize output register oD0 */
mov oT2, c0 /* initialize output register oT2 */
mov oT1, c0 /* initialize output register oT1 */
mov oT0, c0 /* initialize output register oT0 */
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3
mov oT0, v7
dp4 r0.x, v0, c15
dp4 r0.y, v0, c16
dp4 r0.z, v0, c17
dp4 r0.w, v0, c18
mov r1, r0
mov oT1, r1
mov oT2, r1
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
dp3 r1.x, c4, v1
dp3 r1.y, c4, v2
dp3 r1.z, c4, r0
dp3 r2.x, v3, r0
sge r3.x, r2.x, c5.x
add r3.x, r3.x, r3.x
add r3.x, r3.x, -c5.y
mul r1.z, r1.z, r3.x
mul r1, r1, c5.z
add oD0, r1, c5.z
mov oD0.w, c5.w
mov oD1, c5.y