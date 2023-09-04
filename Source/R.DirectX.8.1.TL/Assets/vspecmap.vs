vs.1.1
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3
add r8, v0, -c9
dp3 r8.w, r8, r8
rsq r8.w, r8.w
mul r8, r8, r8.w
add r8, r8, -c4
dp3 r8.w, r8, r8
rsq r8.w, r8.w
mul r8, r8, r8.w
dp3 r8.x, v3, r8
mul r8.x, r8.x, -c5.y
max r8.x, r8.x, c24
mul r8.x, r8.x, r8.x
mul r8.x, r8.x, r8.x
mul r8.x, r8.x, r8.x
mul r8.x, r8.x, r8.x
mul r8.x, r8.x, r8.x
mul oD0.x, r8.x, c8.x
mul oD0.y, r8.x, c21.x
mul oD0.z, r8.x, c23.x
mov oT0, v7
dp4 r0.z, v0, c2
add r0.z, r0.z, -c6.x
mul r0.z, r0.z, c6.y
max r0.z, r0.z, c5.x
min r0.w, r0.z, c5.y
add r0.w, c5.y, -r0.w
mul oD0.w, r0.w, c5.w
mov oFog.x, c5.y