//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
// Parameters:
//
//   float4x4 gWorldViewProj;
//
//
// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   gWorldViewProj c0       4
//

    vs_2_0
    def c4, 1, 0, 0, 0
    dcl_position v0  // vin<0,1,2>
    dcl_color v1  // vin<3,4,5,6>

#line 29 "F:\d3d12\d3d12-chapter6-box\d3d12-chapter6-box\Shaders\color.hlsl"
    mul r0, v0.xyzx, c4.xxxy
    add r0, r0, c4.yyyx
    dp4 r0.x, r0, c0  // ::vout<0>
    mul r1, v0.xyzx, c4.xxxy
    add r1, r1, c4.yyyx
    dp4 r0.y, r1, c1  // ::vout<1>
    mul r1, v0.xyzx, c4.xxxy
    add r1, r1, c4.yyyx
    dp4 r0.z, r1, c2  // ::vout<2>
    mul r1, v0.xyzx, c4.xxxy
    add r1, r1, c4.yyyx
    dp4 r0.w, r1, c3  // ::vout<3>

#line 32
    mov r1, v1  // ::vout<4,5,6,7>
    mov oPos, r0  // ::VS<0,1,2,3>
    mov oD0, r1  // ::VS<4,5,6,7>

// approximately 15 instruction slots used
