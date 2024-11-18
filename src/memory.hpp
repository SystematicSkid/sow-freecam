#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

namespace memory
{
    uintptr_t sigscan( const char* pattern );
    std::string hex_to_bytes( std::string hex );
}