// GPU performance-measurement subsystem

#pragma once

#include <d3d11.h>
#include "timer.h"

struct ID3D11DeviceContext;
struct ID3D11Query;

#define BUF_SIZE 20

// Enum of GPU timestamps to record
enum GTS {
    GTS_BeginFrame,
    GTS_DrawToTexture ,
    GTS_ToneMapping,
    GTS_EndFrame,

    GTS_Max
};

class CGpuProfiler {
  public:
    CGpuProfiler();

    bool Init(ID3D11Device* device);
    void Shutdown();

    void BeginFrame(ID3D11DeviceContext* context);
    void Timestamp(ID3D11DeviceContext* context, GTS gts);
    void EndFrame(ID3D11DeviceContext* context);

    // Wait on GPU for last frame's data (not this frame's) to be available
    void WaitForDataAndUpdate(ID3D11DeviceContext* context);

    float Time();

    float Dt(GTS gts) { return m_adT[gts]; }
    float DtAvg( GTS gts) { return m_adTAvg[gts]; }
  private:
    int m_iFrameQuery;                          // Which of the two sets of queries are we currently issuing?
    int m_iFrameCollect;                        // Which of the two did we last collect?
    ID3D11Query* m_apQueryTsDisjoint[BUF_SIZE];        // "Timestamp disjoint" query; records whether timestamps are valid
    ID3D11Query* m_apQueryTs[GTS_Max][BUF_SIZE];       // Individual timestamp queries for each relevant point in the frame

    float m_adT[GTS_Max];                       // Last frame's timings (each relative to previous GTS)
    float m_adTAvg[GTS_Max];                    // Timings averaged over 0.5 second

    float m_adTTotalAvg[GTS_Max];               // Total timings thus far within this averaging period
    int m_frameCountAvg;                        // Frames rendered in current averaging period
    float m_tBeginAvg;                          // Time at which current averaging period started'
    LARGE_INTEGER fr, t1, t2;
};
