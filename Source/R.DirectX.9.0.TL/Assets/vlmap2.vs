vs.1.1
dcl_position v0
dcl_normal v3
dcl_texcoord v7
mov r0, c0 /* initialize register r0 */
mov oD0, c0 /* initialize output register oD0 */
mov oT1, c0 /* initialize output register oT1 */
mov oT0, c0 /* initialize output register oT0 */
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3
mov oD0.xyz, c7
mov oD0.w, c5.w
mov oT0, v7
mov oT1, v3
dp4 r0.z, v0, c2
add r0.z, r0.z, -c6.x
mul r0.z, r0.z, c6.y
max r0.z, r0.z, c5.x
min r0.z, r0.z, c5.y
add oFog /* removed swizzle */, c5.y, -r0.z