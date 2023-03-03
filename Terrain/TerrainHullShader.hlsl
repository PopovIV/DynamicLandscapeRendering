#define NUM_CONTROL_POINTS 3

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float3 cameraPos;
};

struct HS_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 tex2 : TEXCOORD1;
};

struct DS_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 tex2 : TEXCOORD1;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeFactors[3]         : SV_TessFactor;
    float InsideFactor : SV_InsideTessFactor;
    float3 bezierPoints[10] : BEZIERPOS;
};

float calculateEdgeFactor(float4 p0, float4 p1)
{
    p0 = mul(p0, worldMatrix);
    p1 = mul(p1, worldMatrix);
    float edgeLength = distance(p0, p1);
    float4 edgeCenter = (p0 + p1) * 0.5;
    float viewDistance = distance(edgeCenter, cameraPos);
    return (edgeLength * 1080) / (10.0 * viewDistance);
}

float3 CalculateBezierControlPoint(float3 p0PositionWS, float3 aNormalWS, float3 p1PositionWS, float3 bNormalWS) {
    float w = dot(p1PositionWS - p0PositionWS, aNormalWS);
    return (p0PositionWS * 2 + p1PositionWS - w * aNormalWS) / 3.0;
}

float3 CalculateBezierControlNormal(float3 p0PositionWS, float3 aNormalWS, float3 p1PositionWS, float3 bNormalWS) {
    float3 d = p1PositionWS - p0PositionWS;
    float v = 2 * dot(d, aNormalWS + bNormalWS) / dot(d, d);
    return normalize(aNormalWS + bNormalWS - v * d);
}

HS_CONSTANT_DATA_OUTPUT constantsHullShader(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> patch, uint patchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;

    // Calculate tessFactor
    float p0factor = calculateEdgeFactor(patch[1].position, patch[2].position);
    float p1factor = calculateEdgeFactor(patch[2].position, patch[0].position);
    float p2factor = calculateEdgeFactor(patch[0].position, patch[1].position);

    // Assign tessellation levels (constant for now)
    output.EdgeFactors[0] = p0factor;
    output.EdgeFactors[1] = p1factor;
    output.EdgeFactors[2] = p2factor;
    output.InsideFactor = (output.EdgeFactors[0] + output.EdgeFactors[1] + output.EdgeFactors[2]) / 3.0f;

    output.bezierPoints[0] = CalculateBezierControlPoint(patch[0].position, patch[0].normal, patch[1].position, patch[1].normal);
    output.bezierPoints[1] = CalculateBezierControlPoint(patch[1].position, patch[1].normal, patch[0].position, patch[0].normal);
    output.bezierPoints[2] = CalculateBezierControlPoint(patch[1].position, patch[1].normal, patch[2].position, patch[2].normal);
    output.bezierPoints[3] = CalculateBezierControlPoint(patch[2].position, patch[2].normal, patch[1].position, patch[1].normal);
    output.bezierPoints[4] = CalculateBezierControlPoint(patch[2].position, patch[2].normal, patch[0].position, patch[0].normal);
    output.bezierPoints[5] = CalculateBezierControlPoint(patch[0].position, patch[0].normal, patch[2].position, patch[2].normal);
    float3 avgBezier = 0;
    [unroll] for (int i = 0; i < 6; i++) {
        avgBezier += output.bezierPoints[i];
    }
    avgBezier /= 6.0;
    float3 avgControl = (patch[0].position + patch[1].position + patch[2].position) / 3.0;
    output.bezierPoints[6] = avgBezier + (avgBezier - avgControl) / 2.0;

    output.bezierPoints[7] = CalculateBezierControlNormal(patch[0].position, patch[0].normal, patch[1].position, patch[1].normal);
    output.bezierPoints[8] = CalculateBezierControlNormal(patch[1].position, patch[1].normal, patch[2].position, patch[2].normal);
    output.bezierPoints[9] = CalculateBezierControlNormal(patch[2].position, patch[2].normal, patch[0].position, patch[0].normal);

    return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_CONTROL_POINTS)]
[patchconstantfunc("constantsHullShader")]
[maxtessfactor(64.0)]
DS_INPUT main(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> patch, uint patchID : SV_OutputControlPointID)
{
    DS_INPUT output;

    // Copy inputs to outputs
    output.position = patch[patchID].position;
    output.normal = patch[patchID].normal;
    output.tangent = patch[patchID].tangent;
    output.binormal = patch[patchID].binormal;
    output.tex = patch[patchID].tex;
    output.tex2 = patch[patchID].tex2;

    return output;
}
