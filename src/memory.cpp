#include "memory.hpp"
#include <Windows.h>
#include <Psapi.h>


uintptr_t memory::sigscan(const char *pattern)
{
    u_char *base = (u_char *)GetModuleHandle(nullptr);
    std::string signature = hex_to_bytes(pattern);
    u_char first = (u_char)signature.at(0);
    /* Get module size */
	MODULEINFO info;
	GetModuleInformation(GetCurrentProcess(), (HMODULE)base, &info, sizeof(info));
    size_t memory_size = info.SizeOfImage;
    /* Scan for signature */
    u_char *end = (base + memory_size) - signature.length();
    for (; base < end; ++base)
    {
        if (*base != first)
            continue;
        u_char *bytes = base;
        u_char *sig = (u_char *)signature.c_str();
        for (; *sig; ++sig, ++bytes)
        {
            if (*sig == '?')
                continue;
            if (*bytes != *sig)
                goto end;
        }
        return (uintptr_t)base;
    end:;
    }
    return NULL;
}

std::string memory::hex_to_bytes(std::string hex)
{
    std::string bytes;
    hex.erase(std::remove_if(hex.begin(), hex.end(), isspace), hex.end());
    for (uint32_t i = 0; i < hex.length(); i += 2)
    {
        if ((u_char)hex[i] == '?')
        {
            bytes += '?';
            i -= 1;
            continue;
        }
        u_char byte = (u_char)std::strtol(hex.substr(i, 2).c_str(), nullptr, 16);
        bytes += byte;
    }
    return bytes;
}