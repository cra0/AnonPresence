#include "Utils.h"

namespace Utils
{
    static UINT32 mempage_mask = (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);

    static char dlldir[MAX_PATH] = {};

    bool IsBadReadPtrEx(void* p)
    {
        MEMORY_BASIC_INFORMATION mbi = { 0 };
        if (::VirtualQuery(p, &mbi, sizeof(mbi)))
        {
            bool b = !(mbi.Protect & mempage_mask);
            // check the page is not a guard page
            if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) b = true;
            return b;
        }
        return true; //Unable to Query
    }

    bool CompareData(const BYTE* pData, const BYTE* bMask, const char* szMask)
    {
        for (; *szMask; ++szMask, ++pData, ++bMask)
            if (*szMask == 'x' && *pData != *bMask)   return 0;
        return (*szMask) == NULL;
    }

    uintptr_t _FindPatternInternal(uintptr_t dwAddress, size_t dwLen, BYTE *bMask, const char * szMask)
    {
        for (size_t i = 0; i < dwLen; i++)
            if (CompareData((BYTE*)(dwAddress + i), bMask, szMask))  return (uintptr_t)(dwAddress + i);
        return 0;
    }

    uintptr_t _FindPatternInternalSafe(std::vector<MemFrag> memFrags, BYTE* bMask, const char* szMask)
    {
        for (size_t i = 0; i < memFrags.size(); i++)
        {
            MemFrag frag = memFrags[i];
            for (size_t j = 0; j < frag.GetSize(); j++)
            {
                if (CompareData((BYTE*)(frag._start + j), bMask, szMask))
                {
                    return (uintptr_t)(frag._start + j);
                }
            }
        }  
        return 0;
    }

    DWORD GetModuleSize(HMODULE hModule)
    {
        PIMAGE_DOS_HEADER dosHeader = nullptr;
        PIMAGE_NT_HEADERS ntHeaders = nullptr;
        PIMAGE_OPTIONAL_HEADER optional_header = nullptr;

        //get dos_header
        dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        {
            return 0;
        }

        //get nt_header
        ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>((uintptr_t)hModule + dosHeader->e_lfanew);

        //get optional header
        optional_header = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(&ntHeaders->OptionalHeader);

        //return size of image
        return optional_header->SizeOfImage;
    }


    void get_base_module_bounds(uintptr_t *start, uintptr_t *end)
    {
        const auto module = ::GetModuleHandleA(NULL);
        *start = (uintptr_t)(module);
        *end = *start + GetModuleSize(module);
    }

    void get_module_bounds(const char *name, uintptr_t *start, uintptr_t *end)
    {
        const auto module = ::GetModuleHandleA(name);
        if (module == nullptr)
            return;

        *start = (uintptr_t)(module);
        *end = *start + GetModuleSize(module);
    }

    bool _PopulateMemFrags(std::vector<MemFrag> &frags, uintptr_t start, uintptr_t end)
    {
        MEMORY_BASIC_INFORMATION mem_basic_info = { 0 };
        uintptr_t current_address = start;
        while (::VirtualQuery((void*)current_address, &mem_basic_info, sizeof(mem_basic_info)) != 0)
        {
            bool bp = !(mem_basic_info.Protect & mempage_mask);
            if (mem_basic_info.Protect & (PAGE_GUARD | PAGE_NOACCESS)) bp = true;

            if (!bp)
                frags.push_back(MemFrag(mem_basic_info.BaseAddress, ((uintptr_t)mem_basic_info.BaseAddress) + mem_basic_info.RegionSize));

            current_address = (uintptr_t)mem_basic_info.BaseAddress + (mem_basic_info.RegionSize);
            if (current_address > end)
                break;
        }

        if (frags.size() > 0)
            return true;

        return false;
    }

    uintptr_t FindPatternSafe(uintptr_t module_ptr, _In_ const char* Pattern, const char* PatternMask)
    {
        uintptr_t start = module_ptr;
        size_t size = GetModuleSize((HMODULE)start);
        uintptr_t end = start + size;

        std::vector<MemFrag> frags;
        if (!_PopulateMemFrags(frags, start, end))
            return 0;

        return _FindPatternInternalSafe(frags, (BYTE*)Pattern, PatternMask);
    }

    uintptr_t ResolveX64_REL(uintptr_t ptr_to_asm_location, uint32_t off1, uint32_t off2)
    {
        int32_t rel_offset = *reinterpret_cast<int32_t*>(ptr_to_asm_location + off1);
        intptr_t abs_ptr = (ptr_to_asm_location + off2) + rel_offset;
        return abs_ptr;
    }

    char* GetDirectoryFile(const char* filename)
    {
        if (dlldir[0] == '\0')
        {
            char fPath[MAX_PATH] = {};
            char fPDrive[MAX_PATH] = {};
            char fPPath[MAX_PATH] = {};

            GetModuleFileNameA(nullptr, fPath, MAX_PATH);
            _splitpath(fPath, fPDrive, fPPath, NULL, NULL);

            strcpy_s(dlldir, fPDrive);
            strcat_s(dlldir, fPPath);
        }

        static char path[320];
        strcpy_s(path, dlldir);
        strcat_s(path, filename);
        return path;
    }

    void WriteFile(const char* filepath, PVOID data, size_t len)
    {
        std::ofstream outputFile(filepath, std::ios::binary | std::ios::out);
        outputFile.write((char*)data, len);
        outputFile.flush();
        outputFile.close();
        return;
    }

    void WriteFileLocalDirectory(const char* fileName, PVOID data, size_t len)
    {
        return WriteFile(GetDirectoryFile(fileName), data, len);
    }

};