struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

PS_INPUT main(uint input : SV_VERTEXID) {
    PS_INPUT output = (PS_INPUT)0;
    output.tex = float2(input & 1, input >> 1);
    output.pos = float4((output.tex.x - 0.5f) * 2, -(output.tex.y - 0.5f) * 2, 0, 1);
    return output;
}
