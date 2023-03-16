#include "gpuprofiler.h"

CGpuProfiler::CGpuProfiler() : m_iFrameQuery(0), m_iFrameCollect(-1), m_frameCountAvg(0), m_tBeginAvg(0.0f) {
    memset(m_apQueryTsDisjoint, 0, sizeof(m_apQueryTsDisjoint));
    memset(m_apQueryTs, 0, sizeof(m_apQueryTs));
    memset(m_adT, 0, sizeof(m_adT));
    memset(m_adTAvg, 0, sizeof(m_adT));
    memset(m_adTTotalAvg, 0, sizeof(m_adT));
}

bool CGpuProfiler::Init(ID3D11Device* device) {

    QueryPerformanceCounter(&t1);
    // Create all the queries we'll need
    D3D11_QUERY_DESC queryDesc = { D3D11_QUERY_TIMESTAMP_DISJOINT, 0 };

    for (int i = 0; i < BUF_SIZE; i++) {
        if (FAILED(device->CreateQuery(&queryDesc, &m_apQueryTsDisjoint[i]))) {
            return false;
        }
    }

    queryDesc.Query = D3D11_QUERY_TIMESTAMP;

    for (GTS gts = GTS_BeginFrame; gts < GTS_Max; gts = GTS(gts + 1)) {
        for (int i = 0; i < BUF_SIZE; i++) {
            if (FAILED(device->CreateQuery(&queryDesc, &m_apQueryTs[gts][i]))) {
                return false;
            }
        }
    }

    return true;
}

void CGpuProfiler::Shutdown() {
    for (int i = 0; i < BUF_SIZE; i++) {
        if (m_apQueryTsDisjoint[i]) {
            m_apQueryTsDisjoint[i]->Release();
        }
    }

    for (GTS gts = GTS_BeginFrame; gts < GTS_Max; gts = GTS(gts + 1)) {
        for (int i = 0; i < BUF_SIZE; i++) {
            if (m_apQueryTs[gts][i]) {
                m_apQueryTs[gts][i]->Release();
            }
        }
    }
}

void CGpuProfiler::BeginFrame(ID3D11DeviceContext* context) {
    context->Begin(m_apQueryTsDisjoint[m_iFrameQuery]);
    Timestamp(context, GTS_BeginFrame);
}

void CGpuProfiler::Timestamp(ID3D11DeviceContext* context, GTS gts) {
    context->End(m_apQueryTs[gts][m_iFrameQuery]);
}

void CGpuProfiler::EndFrame(ID3D11DeviceContext* context) {
    Timestamp(context, GTS_EndFrame);
    context->End(m_apQueryTsDisjoint[m_iFrameQuery]);

    ++m_iFrameQuery %= BUF_SIZE;
}

float CGpuProfiler::Time() {
    QueryPerformanceCounter(&t2);

    QueryPerformanceFrequency(&fr);

    float time = (t2.QuadPart - t1.QuadPart) / (float)fr.QuadPart;
    t1 = t2;

    return time * 1000.0f;
}

void CGpuProfiler::WaitForDataAndUpdate(ID3D11DeviceContext* context) {
    if (m_iFrameCollect < 0) {
        // Haven't run enough frames yet to have data
        m_iFrameCollect = 0;
        return;
    }

    // Wait for data
    while (context->GetData(m_apQueryTsDisjoint[m_iFrameCollect], NULL, 0, 0) == S_FALSE) {
        Sleep(1);
    }

    int iFrame = m_iFrameCollect;
    ++m_iFrameCollect %= BUF_SIZE;

    D3D11_QUERY_DATA_TIMESTAMP_DISJOINT timestampDisjoint;
    if (context->GetData(m_apQueryTsDisjoint[iFrame], &timestampDisjoint, sizeof(timestampDisjoint), 0) != S_OK) {
        return;
    }

    if (timestampDisjoint.Disjoint) {
        // Throw out this frame's data
        return;
    }

    UINT64 timestampPrev;
    if (context->GetData(m_apQueryTs[GTS_BeginFrame][iFrame], &timestampPrev, sizeof(UINT64), 0) != S_OK) {
        return;
    }

    for (GTS gts = GTS(GTS_BeginFrame + 1); gts < GTS_Max; gts = GTS(gts + 1)) {
        UINT64 timestamp;
        if (context->GetData(m_apQueryTs[gts][iFrame], &timestamp, sizeof(UINT64), 0) != S_OK) {
            return;
        }
        m_adT[gts] = float(timestamp - timestampPrev) / float(timestampDisjoint.Frequency);
        timestampPrev = timestamp;

        m_adTTotalAvg[gts] += m_adT[gts];
    }

    ++m_frameCountAvg;
    if (Time() > m_tBeginAvg + 0.5f) {
        for (GTS gts = GTS_BeginFrame; gts < GTS_Max; gts = GTS(gts + 1)) {
            m_adTAvg[gts] = m_adTTotalAvg[gts] / m_frameCountAvg;
            m_adTTotalAvg[gts] = 0.0f;
        }

        m_frameCountAvg = 0;
        m_tBeginAvg = Time();
    }
}
