//--------------------------------------------------------------------------------------
// StringConverter  - utility class to help with encoding problems
//--------------------------------------------------------------------------------------
#ifndef _STRINGCONVERTER_H_
#define _STRINGCONVERTER_H_

#include<clocale>
#include<string>

class StringConverter {
public:
    // Function to convert wchar_t* to std string
    static std::string wstr2str(const wchar_t* ws);
private:
    // Disallow creating an instance of this object
    StringConverter() {};
};

#endif
