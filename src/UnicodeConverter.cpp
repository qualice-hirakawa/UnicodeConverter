#include "../include/UnicodeConverter.hpp"

#include <cstdint>

#if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L) && (_MSC_VER >= 1920))
#else
namespace
{
using char8_t = char;
}
#endif

#if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L) && (_MSC_VER >= 1920))
#else
namespace std
{
using u8string = std::string;
}
#endif

namespace
{
#if (__cpp_lib_string_view >= 201606L)
template <class T>
using string_args = std::basic_string_view<T>;
#else
template <class T>
using string_args = const std::basic_string<T, std::char_traits<T>, std::allocator<T>>&;
#endif
}

namespace
{
uint_fast8_t GetU8ByteCount(char ch)
{
    if (0 <= uint8_t(ch) && uint8_t(ch) < 0x80)
    {
        return 1;
    }
    if (0xC2 <= uint8_t(ch) && uint8_t(ch) < 0xE0)
    {
        return 2;
    }
    if (0xE0 <= uint8_t(ch) && uint8_t(ch) < 0xF0)
    {
        return 3;
    }
    if (0xF0 <= uint8_t(ch) && uint8_t(ch) < 0xF8)
    {
        return 4;
    }
    return 0;
}

bool IsU8LaterByte(char ch)
{
    return 0x80 <= uint8_t(ch) && uint8_t(ch) < 0xC0;
}

bool IsU16HighSurrogate(char16_t ch)
{
    return 0xD800 <= ch && ch < 0xDC00;
}

bool IsU16LowSurrogate(char16_t ch)
{
    return 0xDC00 <= ch && ch < 0xE000;
}
}

namespace UnicodeConverter
{
namespace Internal
{
bool U8ToU16(const std::array<char, 4>& u8ch, std::array<char16_t, 2>& u16ch)
{
    char32_t u32ch;
    if (!U8ToU32(u8ch, u32ch))
    {
        return false;
    }
    if (!U32ToU16(u32ch, u16ch))
    {
        return false;
    }
    return true;
}

bool U8ToU32(const std::array<char, 4>& u8ch, char32_t& u32ch)
{
    auto num_bytes = GetU8ByteCount(u8ch[0]);
    if (num_bytes == 0)
    {
        return false;
    }
    switch (num_bytes)
    {
    case 1:
        u32ch = char32_t(uint8_t(u8ch[0]));
        break;
    case 2:
        if (!IsU8LaterByte(u8ch[1]))
        {
            return false;
        }
        if ((uint8_t(u8ch[0]) & 0x1E) == 0)
        {
            return false;
        }

        u32ch = char32_t(u8ch[0] & 0x1F) << 6;
        u32ch |= char32_t(u8ch[1] & 0x3F);
        break;
    case 3:
        if (!IsU8LaterByte(u8ch[1]) || !IsU8LaterByte(u8ch[2]))
        {
            return false;
        }
        if ((uint8_t(u8ch[0]) & 0x0F) == 0 && (uint8_t(u8ch[1]) & 0x20) == 0)
        {
            return false;
        }

        u32ch = char32_t(u8ch[0] & 0x0F) << 12;
        u32ch |= char32_t(u8ch[1] & 0x3F) << 6;
        u32ch |= char32_t(u8ch[2] & 0x3F);
        break;
    case 4:
        if (!IsU8LaterByte(u8ch[1]) || !IsU8LaterByte(u8ch[2]) || !IsU8LaterByte(u8ch[3]))
        {
            return false;
        }
        if ((uint8_t(u8ch[0]) & 0x07) == 0 && (uint8_t(u8ch[1]) & 0x30) == 0)
        {
            return false;
        }

        u32ch = char32_t(u8ch[0] & 0x07) << 18;
        u32ch |= char32_t(u8ch[1] & 0x3F) << 12;
        u32ch |= char32_t(u8ch[2] & 0x3F) << 6;
        u32ch |= char32_t(u8ch[3] & 0x3F);
        break;
    default:
        return false;
    }

    return true;
}

bool U16ToU8(const std::array<char16_t, 2>& u16ch, std::array<char, 4>& u8ch)
{
    char32_t u32ch;
    if (!U16ToU32(u16ch, u32ch))
    {
        return false;
    }
    if (!U32ToU8(u32ch, u8ch))
    {
        return false;
    }
    return true;
}

bool U16ToU32(const std::array<char16_t, 2>& u16ch, char32_t& u32ch)
{
    if (IsU16HighSurrogate(u16ch[0]))
    {
        if (IsU16LowSurrogate(u16ch[1]))
        {
            u32ch = 0x10000 + (char32_t(u16ch[0]) - 0xD800) * 0x400 + (char32_t(u16ch[1]) - 0xDC00);
        }
        else if (u16ch[1] == 0)
        {
            u32ch = u16ch[0];
        }
        else
        {
            return false;
        }
    }
    else if (IsU16LowSurrogate(u16ch[0]))
    {
        if (u16ch[1] == 0)
        {
            u32ch = u16ch[0];
        }
        else
        {
            return false;
        }
    }
    else
    {
        u32ch = u16ch[0];
    }

    return true;
}

bool U32ToU8(const char32_t u32ch, std::array<char, 4>& u8ch)
{
    if (u32ch > 0x10FFFF)
    {
        return false;
    }

    if (u32ch < 128)
    {
        u8ch[0] = char(u32ch);
        u8ch[1] = 0;
        u8ch[2] = 0;
        u8ch[3] = 0;
    }
    else if (u32ch < 2048)
    {
        u8ch[0] = 0xC0 | char(u32ch >> 6);
        u8ch[1] = 0x80 | (char(u32ch) & 0x3F);
        u8ch[2] = 0;
        u8ch[3] = 0;
    }
    else if (u32ch < 65536)
    {
        u8ch[0] = 0xE0 | char(u32ch >> 12);
        u8ch[1] = 0x80 | (char(u32ch >> 6) & 0x3F);
        u8ch[2] = 0x80 | (char(u32ch) & 0x3F);
        u8ch[3] = 0;
    }
    else
    {
        u8ch[0] = 0xF0 | char(u32ch >> 18);
        u8ch[1] = 0x80 | (char(u32ch >> 12) & 0x3F);
        u8ch[2] = 0x80 | (char(u32ch >> 6) & 0x3F);
        u8ch[3] = 0x80 | (char(u32ch) & 0x3F);
    }

    return true;
}

bool U32ToU16(const char32_t u32ch, std::array<char16_t, 2>& u16ch)
{
    if (u32ch > 0x10FFFF)
    {
        return false;
    }

    if (u32ch < 0x10000)
    {
        u16ch[0] = char16_t(u32ch);
        u16ch[1] = 0;
    }
    else
    {
        u16ch[0] = char16_t((u32ch - 0x10000) / 0x400 + 0xD800);
        u16ch[1] = char16_t((u32ch - 0x10000) % 0x400 + 0xDC00);
    }

    return true;
}
}

bool Convert(string_args<char8_t> u8str, std::u16string& u16str) noexcept
{
    for (auto u8iter = u8str.cbegin(); u8iter != u8str.cend(); u8iter++)
    {
        auto num_bytes = GetU8ByteCount((*u8iter));
        if (num_bytes == 0)
        {
            return false;
        }

        std::array<char, 4> u8ch;
        u8ch[0] = (*u8iter);
        for (uint_fast8_t i = 1; i < num_bytes; i++)
        {
            u8iter++;
            if (u8iter == u8str.cend())
            {
                return false;
            }
            u8ch[i] = (*u8iter);
        }

        std::array<char16_t, 2> u16ch;
        if (!Internal::U8ToU16(u8ch, u16ch))
        {
            return false;
        }

        u16str.push_back(u16ch[0]);
        if (u16ch[1] != 0)
        {
            u16str.push_back(u16ch[1]);
        }
    }

    return true;
}

bool Convert(string_args<char8_t> u8str, std::u32string& u32str) noexcept
{
    for (auto u8iter = u8str.cbegin(); u8iter != u8str.cend(); u8iter++)
    {
        auto num_bytes = GetU8ByteCount((*u8iter));
        if (num_bytes == 0)
        {
            return false;
        }

        std::array<char, 4> u8ch;
        u8ch[0] = (*u8iter);
        for (uint_fast8_t i = 1; i < num_bytes; i++)
        {
            u8iter++;
            if (u8iter == u8str.cend())
            {
                return false;
            }
            u8ch[i] = (*u8iter);
        }

        char32_t u32ch;
        if (!Internal::U8ToU32(u8ch, u32ch))
        {
            return false;
        }

        u32str.push_back(u32ch);
    }
    return true;
}

bool Convert(string_args<char16_t> u16str, std::u8string& u8str)
{
    for (auto u16iter = u16str.cbegin(); u16iter != u16str.cend(); u16iter++)
    {
        std::array<char16_t, 2> u16ch;
        if (IsU16HighSurrogate((*u16iter)))
        {
            u16ch[0] = (*u16iter);
            u16iter++;
            if (u16iter == u16str.cend())
            {
                return false;
            }
            u16ch[1] = (*u16iter);
        }
        else
        {
            u16ch[0] = (*u16iter);
            u16ch[1] = 0;
        }

        std::array<char, 4> u8ch;
        if (!Internal::U16ToU8(u16ch, u8ch))
        {
            return false;
        }
        if (u8ch[0] != 0)
        {
            u8str.push_back(u8ch[0]);
        }
        if (u8ch[1] != 0)
        {
            u8str.push_back(u8ch[1]);
        }
        if (u8ch[2] != 0)
        {
            u8str.push_back(u8ch[2]);
        }
        if (u8ch[3] != 0)
        {
            u8str.push_back(u8ch[3]);
        }
    }
    return true;
}

bool Convert(string_args<char16_t> u16str, std::u32string& u32str)
{
    for (auto u16iter = u16str.cbegin(); u16iter != u16str.cend(); u16iter++)
    {
        std::array<char16_t, 2> u16ch;
        if (IsU16HighSurrogate((*u16iter)))
        {
            u16ch[0] = (*u16iter);
            u16iter++;
            if (u16iter == u16str.cend())
            {
                return false;
            }
            u16ch[1] = (*u16iter);
        }
        else
        {
            u16ch[0] = (*u16iter);
            u16ch[1] = 0;
        }

        char32_t u32ch;
        if (!Internal::U16ToU32(u16ch, u32ch))
        {
            return false;
        }
        u32str.push_back(u32ch);
    }
    return true;
}

bool Convert(const std::u32string& u32str, std::u8string& u8str)
{
    for (auto u32iter = u32str.cbegin(); u32iter != u32str.cend(); u32iter++)
    {
        std::array<char, 4> u8ch;
        if (!Internal::U32ToU8((*u32iter), u8ch))
        {
            return false;
        }

        if (u8ch[0] != 0)
        {
            u8str.push_back(u8ch[0]);
        }
        if (u8ch[1] != 0)
        {
            u8str.push_back(u8ch[1]);
        }

        if (u8ch[2] != 0)
        {
            u8str.push_back(u8ch[2]);
        }
        if (u8ch[3] != 0)
        {
            u8str.push_back(u8ch[3]);
        }
    }
    return true;
}

bool Convert(const std::u32string& u32str, std::u16string& u16str)
{
    for (auto u32iter = u32str.cbegin(); u32iter != u32str.cend(); u32iter++)
    {
        std::array<char16_t, 2> u16ch;
        if (!Internal::U32ToU16((*u32iter), u16ch))
        {
            return false;
        }

        if (u16ch[0] != 0)
        {
            u16str.push_back(u16ch[0]);
        }
        if (u16ch[1] != 0)
        {
            u16str.push_back(u16ch[1]);
        }
    }
    return true;
}
}
