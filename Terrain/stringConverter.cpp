#include"stringConverter.h"

// Function to convert wchar_t* to std string
std::string StringConverter::wstr2str(const wchar_t* ws) {

    std::string curLocale = setlocale(LC_ALL, NULL);
    setlocale(LC_ALL, "chs");

    int wsLen = wcslen(ws);
    int bufLen = sizeof(wchar_t) * wsLen + 1;
    char* pRes = new char[bufLen];
    memset(pRes, '\0', bufLen);
    //wcstombs_s(&convertedChars,dest,dByteNum,wcs,_TRUNCATE)
    size_t convertLen = 0;
    wcstombs_s(&convertLen, pRes, bufLen, ws, _TRUNCATE);
    std::string resStr = pRes;
    delete[] pRes;

    setlocale(LC_ALL, curLocale.c_str());

    return resStr;

}
