vs.1.1
dcl_position v0
dcl_normal v3
dcl_texcoord v7
mov r1, c0 /* initialize register r1 */
mov r0, c0 /* initialize register r0 */
mov oD0, c0 /* initialize output register oD0 */
mov oT0, c0 /* initialize output register oT0 */
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3
mov oT0, v7
mov r0, v0
add r0.xyz, c25.xyzz, -r0.xyzz
dp3 r1.x, r0, r0
rsq r1.x, r1.x
mul r0.xyz, r0.xyzz, r1.x
mul r1.x, r1.x, c25.w
rcp r1.x, r1.x
add r1.x, c5.y, -r1.x
max r1.x, r1.x, c5.x
dp3 r1.y, r0, v3
max r1.y, r1.y, c5.x
mov r0.xyz, r1.x
mul oD0.xyz, r0.xyzz, c26.xyzz
mov oD0.w, c5.w
mov oFog /* removed swizzle */, c5.y