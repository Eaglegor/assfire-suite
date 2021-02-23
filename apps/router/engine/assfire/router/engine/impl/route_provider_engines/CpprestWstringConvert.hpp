#pragma once

#include <codecvt>

namespace {
#ifdef WIN32

    std::string convert(const utility::string_t &from) {
        return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(from);
    }

    utility::string_t convert(const std::string &from) {
        return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(from);
    }

#else
    std::string convert(const std::string& from)
    {
        return from;
    }
#endif
}