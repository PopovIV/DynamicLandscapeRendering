#include "defines.h"

struct GeomBuffer {
    matrix worldMatrix;
};

cbuffer GeomBufferInst : register (b0)
{
    GeomBuffer geomBuffer[TERRAIN_CHUNK_COUNT_WIDTH * TERRAIN_CHUNK_COUNT_HEIGHT];
};

cbuffer SceneMatrixBuffer : register (b1)
{
    matrix viewProjectionMatrix;
    float3 cameraPos;
};
