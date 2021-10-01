#pragma once

#include "CommonCRT.h"


namespace Utils
{

    struct MemFrag
    {
        uintptr_t _start;
        uintptr_t _end;

        inline size_t GetSize()
        {
            return _end - _start;
        }

        MemFrag(PVOID start, SIZE_T end)
        {
            _start = (uintptr_t)start;
            _end = (uintptr_t)end;
        }

    };

    void get_base_module_bounds(uintptr_t *start, uintptr_t *end);
    void get_module_bounds(const char *name, uintptr_t *start, uintptr_t *end);

    bool IsBadReadPtrEx(void* p);

    bool CompareData(const BYTE* pData, const BYTE* bMask, const char* szMask);

    uintptr_t FindPatternSafe(uintptr_t module_ptr, _In_ const char* Pattern, const char* PatternMask);
    uintptr_t ResolveX64_REL(uintptr_t ptr_to_asm_location, uint32_t off1, uint32_t off2);

    char* GetDirectoryFile(const char* filename);
    void WriteFile(const char* filepath, PVOID data, size_t len);
    void WriteFileLocalDirectory(const char* fileName, PVOID data, size_t len);

};