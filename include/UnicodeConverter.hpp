#ifndef UNICODE_CONVERTER_H
#define UNICODE_CONVERTER_H

#include <array>
#include <string>

namespace UnicodeConverter
{
namespace Internal
{
bool U8ToU16(const std::array<char, 4>& u8ch, std::array<char16_t, 2>& u16ch);

bool U8ToU32(const std::array<char, 4>& u8ch, char32_t& u32ch);

bool U16ToU8(const std::array<char16_t, 2>& u16ch, std::array<char, 4>& u8ch);

bool U16ToU32(const std::array<char16_t, 2>& u16ch, char32_t& u32ch);

bool U32ToU8(const char32_t u32ch, std::array<char, 4>& u8ch);

bool U32ToU16(const char32_t u32ch, std::array<char16_t, 2>& u16ch);
}

#if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L) && (_MSC_VER >= 1920))
#if (__cpp_lib_string_view >= 201606L)
bool Convert(std::u8string_view u8str, std::u16string& u16str) noexcept;
#else
bool Convert(const std::u8string& u8str, std::u16string& u16str) noexcept;
#endif
#else
#if (__cpp_lib_string_view >= 201606L)
bool Convert(std::string_view u8str, std::u16string& u16str) noexcept;
#else
bool Convert(const std::string& u8str, std::u16string& u16str) noexcept;
#endif
#endif

#if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L) && (_MSC_VER >= 1920))
#if (__cpp_lib_string_view >= 201606L)
bool Convert(const std::u8string_view& u8str, std::u32string& u32str) noexcept;
#else
bool Convert(const std::u8string& u8str, std::u32string& u32str) noexcept;
#endif
#else
#if (__cpp_lib_string_view >= 201606L)
bool Convert(std::string_view u8str, std::u32string& u32str) noexcept;
#else
bool Convert(const std::string& u8str, std::u32string& u32str) noexcept;
#endif
#endif

#if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L) && (_MSC_VER >= 1920))
#if (__cpp_lib_string_view >= 201606L)
bool Convert(std::u16string_view u16str, std::u8string& u8str);
#else
bool Convert(const std::u16string& u16str, std::u8string& u8str);
#endif
#else
#if (__cpp_lib_string_view >= 201606L)
bool Convert(std::u16string_view u16str, std::string& u8str);
#else
bool Convert(const std::u16string& u16str, std::string& u8str);
#endif
#endif

#if (__cpp_lib_string_view >= 201606L)
bool Convert(std::u16string_view u16str, std::u32string& u32str);
#else
bool Convert(const std::u16string& u16str, std::u32string& u32str);
#endif

#if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L) && (_MSC_VER >= 1920))
bool U32ToU8(const std::u32string& u32str, std::u8string& u8str);
#else
bool U32ToU8(const std::u32string& u32str, std::string& u8str);
#endif

bool U32ToU16(const std::u32string& u32str, std::u16string& u16str);

}
#endif // UNICODE_CONVERTER_H
