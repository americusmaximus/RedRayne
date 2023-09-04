vs.1.1
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
mov oFog.x, c5.y
