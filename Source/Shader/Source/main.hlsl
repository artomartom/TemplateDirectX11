#include "Func.hlsl"


cbuffer ViewPortBuffer : register(b0){ float2 viewPortSize; float2  resolution; };

//  t - time since application start 
//  t milisec from start (1.f = 1 sec)
//  deltaT - time since last frame  
cbuffer FrameBuffer : register(b1){ float4 FrameTime; }; //   (  t  , (t % (1000 * 60 * 10))  , deltaT,deltaT)
static float2 quadPos[6] = 
{
    {-.5f, +.5f},
    {+.5f, +.5f},
    {+.5f, -.5f},
    {-.5f, +.5f},
    {+.5f, -.5f},
    {-.5f, -.5f},
};

float4 VSMain(uint VertID:SV_VertexID, uint InstID:SV_InstanceID) : SV_Position
{
    float2 pos =rotate2(quadPos[VertID],FrameTime.x%PI);
    pos*=  resolution;
    return float4(pos *0.4  , 1.0, 1.0) ;  
};

float4 PSMain(float4 pos :SV_Position ) : SV_Target
{
    return    float4(  pos.xy,.0, 1.0) ;  
};

 



  