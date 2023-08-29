#ifndef UNICODE_CONVERTER_H
#define UNICODE_CONVERTER_H

#include <array>
#include <string>

#if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L) && (_MSC_VER >= 1920))
namespace UniConv
{
template <typename T, typename E>
concept UnicodeConcept = requires
{
    requires std::integral<T>;
    requires(sizeof(T) == sizeof(E));
    requires std::is_convertible_v<T, E>;
};
template <typename T>
concept UTF8Concept = requires()
{
    UnicodeConcept<T, char8_t>;
};
template <typename T>
concept UTF16Concept = requires()
{
    UnicodeConcept<T, char16_t>;
};
template <typename T>
concept UTF32Concept = requires()
{
    UnicodeConcept<T, char32_t>;
};

class Char
{
    Char() = delete;

public:
    static constexpr bool U8ToU16(const std::array<char8_t, 4>& u8ch, std::array<char16_t, 2>& u16ch);
    static constexpr bool U8ToU32(const std::array<char8_t, 4>& u8ch, char32_t& u32ch);
    static constexpr bool U16ToU8(const std::array<char16_t, 2>& u16ch, std::array<char8_t, 4>& u8ch);
    static constexpr bool U16ToU32(const std::array<char16_t, 2>& u16ch, char32_t& u32ch);
    static constexpr bool U32ToU8(const char32_t u32ch, std::array<char8_t, 4>& u8ch);
    static constexpr bool U32ToU16(const char32_t u32ch, std::array<char16_t, 2>& u16ch);
    static constexpr uint_fast8_t GetU8ByteCount(char8_t ch);
    static constexpr bool IsU8LaterByte(char8_t ch);
    static constexpr bool IsU16HighSurrogate(char16_t ch);
    static constexpr bool IsU16LowSurrogate(char16_t ch);
};

class String
{
    String() = delete;

public:
    template <UTF8Concept S, UTF16Concept D>
    static constexpr bool U8ToU16(std::basic_string_view<S> src, std::basic_string<D>& dst) noexcept;
    template <UTF8Concept S, UTF32Concept D>
    static constexpr bool U8ToU32(std::basic_string_view<S> src, std::basic_string<D>& dst) noexcept;

    template <UTF16Concept S, UTF8Concept D>
    static constexpr bool U16ToU8(std::basic_string_view<S> src, std::basic_string<D>& dst) noexcept;
    template <UTF16Concept S, UTF32Concept D>
    static constexpr bool U16ToU32(std::basic_string_view<S> src, std::basic_string<D>& dst) noexcept;

    template <UTF32Concept S, UTF8Concept D>
    static constexpr bool U32ToU8(std::basic_string_view<S> src, std::basic_string<D>& dst) noexcept;
    template <UTF32Concept S, UTF16Concept D>
    static constexpr bool U32ToU16(std::basic_string_view<S> src, std::basic_string<D>& dst) noexcept;

    template <UTF8Concept S, UTF16Concept D>
    static constexpr bool U8ToU16(const std::basic_string<S>& src, std::basic_string<D>& dst) noexcept;
    template <UTF8Concept S, UTF32Concept D>
    static constexpr bool U8ToU32(const std::basic_string<S>& src, std::basic_string<D>& dst) noexcept;

    template <UTF16Concept S, UTF8Concept D>
    static constexpr bool U16ToU8(const std::basic_string<S>& src, std::basic_string<D>& dst) noexcept;
    template <UTF16Concept S, UTF32Concept D>
    static constexpr bool U16ToU32(const std::basic_string<S>& src, std::basic_string<D>& dst) noexcept;

    template <UTF32Concept S, UTF8Concept D>
    static constexpr bool U32ToU8(const std::basic_string<S>& src, std::basic_string<D>& dst) noexcept;
    template <UTF32Concept S, UTF16Concept D>
    static constexpr bool U32ToU16(const std::basic_string<S>& src, std::basic_string<D>& dst) noexcept;

};

constexpr uint_fast8_t Char::GetU8ByteCount(char8_t ch)
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
constexpr bool Char::IsU8LaterByte(char8_t ch)
{
    return 0x80 <= uint8_t(ch) && uint8_t(ch) < 0xC0;
}
constexpr bool Char::IsU16HighSurrogate(char16_t ch)
{
    return 0xD800 <= ch && ch < 0xDC00;
}
constexpr bool Char::IsU16LowSurrogate(char16_t ch)
{
    return 0xDC00 <= ch && ch < 0xE000;
}
constexpr bool Char::U8ToU16(const std::array<char8_t, 4>& u8ch, std::array<char16_t, 2>& u16ch)
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
constexpr bool Char::U8ToU32(const std::array<char8_t, 4>& u8ch, char32_t& u32ch)
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
constexpr bool Char::U16ToU8(const std::array<char16_t, 2>& u16ch, std::array<char8_t, 4>& u8ch)
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
constexpr bool Char::U16ToU32(const std::array<char16_t, 2>& u16ch, char32_t& u32ch)
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
constexpr bool Char::U32ToU8(const char32_t u32ch, std::array<char8_t, 4>& u8ch)
{
    if (u32ch > 0x10FFFF)
    {
        return false;
    }

    if (u32ch < 128)
    {
        u8ch[0] = char8_t(u32ch);
        u8ch[1] = 0;
        u8ch[2] = 0;
        u8ch[3] = 0;
    }
    else if (u32ch < 2048)
    {
        u8ch[0] = 0xC0 | char8_t(u32ch >> 6);
        u8ch[1] = 0x80 | (char8_t(u32ch) & 0x3F);
        u8ch[2] = 0;
        u8ch[3] = 0;
    }
    else if (u32ch < 65536)
    {
        u8ch[0] = 0xE0 | char8_t(u32ch >> 12);
        u8ch[1] = 0x80 | (char8_t(u32ch >> 6) & 0x3F);
        u8ch[2] = 0x80 | (char8_t(u32ch) & 0x3F);
        u8ch[3] = 0;
    }
    else
    {
        u8ch[0] = 0xF0 | char8_t(u32ch >> 18);
        u8ch[1] = 0x80 | (char8_t(u32ch >> 12) & 0x3F);
        u8ch[2] = 0x80 | (char8_t(u32ch >> 6) & 0x3F);
        u8ch[3] = 0x80 | (char8_t(u32ch) & 0x3F);
    }

    return true;
}
constexpr bool Char::U32ToU16(const char32_t u32ch, std::array<char16_t, 2>& u16ch)
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

template <UTF8Concept S, UTF16Concept D>
constexpr bool String::U8ToU16(std::basic_string_view<S> src, std::basic_string<D>& dst) noexcept
{
    for (auto iter = src.cbegin(); iter != src.cend(); iter++)
    {
        auto num_bytes = Char::GetU8ByteCount((*iter));
        if (num_bytes == 0)
        {
            return false;
        }

        std::array<char8_t, 4> u8ch;
        u8ch[0] = (*iter);
        for (uint_fast8_t i = 1; i < num_bytes; i++)
        {
            iter++;
            if (iter == src.cend())
            {
                return false;
            }
            u8ch[i] = (*iter);
        }

        std::array<char16_t, 2> u16ch;
        if (!Char::U8ToU16(u8ch, u16ch))
        {
            return false;
        }

        dst.push_back(u16ch[0]);
        if (u16ch[1] != 0)
        {
            dst.push_back(u16ch[1]);
        }
    }

    return true;
}
template <UTF8Concept S, UTF32Concept D>
constexpr bool String::U8ToU32(std::basic_string_view<S> src, std::basic_string<D>& dst) noexcept
{
    for (auto iter = src.cbegin(); iter != src.cend(); iter++)
    {
        auto num_bytes = Char::GetU8ByteCount((*iter));
        if (num_bytes == 0)
        {
            return false;
        }

        std::array<char8_t, 4> u8ch;
        u8ch[0] = (*iter);
        for (uint_fast8_t i = 1; i < num_bytes; i++)
        {
            iter++;
            if (iter == src.cend())
            {
                return false;
            }
            u8ch[i] = (*iter);
        }

        char32_t u32ch;
        if (!Char::U8ToU32(u8ch, u32ch))
        {
            return false;
        }

        dst.push_back(u32ch);
    }
    return true;
}
template <UTF16Concept S, UTF8Concept D>
constexpr bool String::U16ToU8(std::basic_string_view<S> src, std::basic_string<D>& dst) noexcept
{
    for (auto iter = src.cbegin(); iter != src.cend(); iter++)
    {
        std::array<char16_t, 2> u16ch;
        if (Char::IsU16HighSurrogate((*iter)))
        {
            u16ch[0] = (*iter);
            iter++;
            if (iter == src.cend())
            {
                return false;
            }
            u16ch[1] = (*iter);
        }
        else
        {
            u16ch[0] = (*iter);
            u16ch[1] = 0;
        }

        std::array<char8_t, 4> u8ch;
        if (!Char::U16ToU8(u16ch, u8ch))
        {
            return false;
        }
        if (u8ch[0] != 0)
        {
            dst.push_back(u8ch[0]);
        }
        if (u8ch[1] != 0)
        {
            dst.push_back(u8ch[1]);
        }
        if (u8ch[2] != 0)
        {
            dst.push_back(u8ch[2]);
        }
        if (u8ch[3] != 0)
        {
            dst.push_back(u8ch[3]);
        }
    }
    return true;
}
template <UTF16Concept S, UTF32Concept D>
constexpr bool String::U16ToU32(std::basic_string_view<S> src, std::basic_string<D>& dst) noexcept
{
    for (auto u16iter = src.cbegin(); u16iter != src.cend(); u16iter++)
    {
        std::array<char16_t, 2> u16ch;
        if (Char::IsU16HighSurrogate((*u16iter)))
        {
            u16ch[0] = (*u16iter);
            u16iter++;
            if (u16iter == src.cend())
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
        if (!Char::U16ToU32(u16ch, u32ch))
        {
            return false;
        }
        dst.push_back(u32ch);
    }
    return true;
}
template <UTF32Concept S, UTF8Concept D>
constexpr bool String::U32ToU8(std::basic_string_view<S> src, std::basic_string<D>& dst) noexcept
{
    for (auto iter = src.cbegin(); iter != src.cend(); iter++)
    {
        std::array<char8_t, 4> u8ch;
        if (!U32ToU8((*iter), u8ch))
        {
            return false;
        }

        if (u8ch[0] != 0)
        {
            dst.push_back(u8ch[0]);
        }
        if (u8ch[1] != 0)
        {
            dst.push_back(u8ch[1]);
        }

        if (u8ch[2] != 0)
        {
            dst.push_back(u8ch[2]);
        }
        if (u8ch[3] != 0)
        {
            dst.push_back(u8ch[3]);
        }
    }
    return true;
}
template <UTF32Concept S, UTF16Concept D>
constexpr bool String::U32ToU16(std::basic_string_view<S> src, std::basic_string<D>& dst) noexcept
{
    for (auto iter = src.cbegin(); iter != src.cend(); iter++)
    {
        std::array<char16_t, 2> u16ch;
        if (!Char::U32ToU16((*iter), u16ch))
        {
            return false;
        }

        if (u16ch[0] != 0)
        {
            dst.push_back(u16ch[0]);
        }
        if (u16ch[1] != 0)
        {
            dst.push_back(u16ch[1]);
        }
    }
    return true;
}

template <UTF8Concept S, UTF16Concept D>
constexpr bool String::U8ToU16(const std::basic_string<S>& src, std::basic_string<D>& dst) noexcept
{
    return U8ToU16(std::basic_string_view<S>(src), dst);
}
template <UTF8Concept S, UTF32Concept D>
constexpr bool String::U8ToU32(const std::basic_string<S>& src, std::basic_string<D>& dst) noexcept
{
    return U8ToU32(std::basic_string_view<S>(src), dst);
}
template <UTF16Concept S, UTF8Concept D>
constexpr bool String::U16ToU8(const std::basic_string<S>& src, std::basic_string<D>& dst) noexcept
{
    return U16ToU8(std::basic_string_view<S>(src), dst);
}
template <UTF16Concept S, UTF32Concept D>
constexpr bool String::U16ToU32(const std::basic_string<S>& src, std::basic_string<D>& dst) noexcept
{
    return U16ToU32(std::basic_string_view<S>(src), dst);
}
template <UTF32Concept S, UTF8Concept D>
constexpr bool String::U32ToU8(const std::basic_string<S>& src, std::basic_string<D>& dst) noexcept
{
    return U32ToU8(std::basic_string_view<S>(src), dst);
}
template <UTF32Concept S, UTF16Concept D>
constexpr bool String::U32ToU16(const std::basic_string<S>& src, std::basic_string<D>& dst) noexcept
{
    return U32ToU16(std::basic_string_view<S>(src), dst);
}
}
#endif
#endif // UNICODE_CONVERTER_H
