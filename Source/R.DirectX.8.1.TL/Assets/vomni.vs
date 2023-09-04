vs.1.1
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3
mov oT0, v7
mov r0, v0
add r0.xyz, c25.xyzz, -r0
dp3 r0.x, r0, r0
rsq r0.x, r0.x
mul r0.x, r0.x, c25.w
rcp r0.x, r0.x
add r0.xyz, c5.y, -r0.x
max r0.xyz, r0.xyzz, c24
mul oD0.xyz, r0.xyzz, c26.xyzz
mov oD0.w, c5.w
mov oFog.x, c5.y