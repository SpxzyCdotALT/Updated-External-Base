#include "Driver.h"

std::uint32_t Driver_t::Find_Process(const std::string& Process_Name)
{
    std::uint64_t Local_Process = 0;
    HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Snapshot == INVALID_HANDLE_VALUE)
    {
        return Local_Process;
    }

    PROCESSENTRY32 Process_Entry{};
    Process_Entry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(Snapshot, &Process_Entry))
    {
        do
        {
            if (!_stricmp(Process_Name.c_str(), Process_Entry.szExeFile))
            {
                Local_Process = Process_Entry.th32ProcessID;
                Process_ID = Local_Process;
                break;
            }
        } while (Process32Next(Snapshot, &Process_Entry));
    }

    CloseHandle(Snapshot);
    return Local_Process;
}

std::uint64_t Driver_t::Find_Module(const std::string& Module_Name)
{
    std::uint64_t Module_Address = 0;

    if (!Process_Handle)
    {
        return Module_Address;
    }

    DWORD Process_ID = GetProcessId(Process_Handle);
    HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, Process_ID);

    if (Snapshot == INVALID_HANDLE_VALUE)
    {
        return Module_Address;
    }

    MODULEENTRY32 Module_Entry{};
    Module_Entry.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(Snapshot, &Module_Entry))
    {
        do
        {
            if (!_stricmp(Module_Name.c_str(), Module_Entry.szModule))
            {
                Module_Address = reinterpret_cast<uint64_t>(Module_Entry.modBaseAddr);
                Base_Address = Module_Address;
                break;
            }
        } while (Module32Next(Snapshot, &Module_Entry));
    }

    CloseHandle(Snapshot);
    return Module_Address;
}

bool Driver_t::Attach_Process(const std::string& Process_Name)
{
    HANDLE Process = OpenProcess(PROCESS_ALL_ACCESS, false, Find_Process(Process_Name));

    if (Process == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    Process_Handle = Process;

    return true;
}

std::string Driver_t::Read_String(std::uint64_t Address)
{
    std::int32_t String_Length = Read<std::int32_t>(Address + 0x10);
    std::uint64_t String_Address = (String_Length >= 16) ? Read<std::uint64_t>(Address) : Address;

    if (String_Length == 0 || String_Length > 255)
    {
        return "Unknown";
    }

    std::vector<char> Buffer(String_Length + 1, 0);
    Driver_ReadVirtualMemory(Process_Handle, reinterpret_cast<void*>(String_Address), Buffer.data(), Buffer.size(), nullptr);

    return std::string(Buffer.data(), String_Length);
}

void Driver_t::Write_String(std::uint64_t Address, const std::string& Value)
{
    auto Str = Read<RbxString>(Address);

    if (Value.length() > Str.Capacity)
    {
        while (Value.length() > Str.Capacity)
            Str.Capacity = Str.Capacity * 2 + 1;

        Str.Data.Pointer = reinterpret_cast<std::uint64_t>(
            VirtualAllocEx(
                Process_Handle,
                nullptr,
                Str.Capacity,
                MEM_RESERVE | MEM_COMMIT,
                PAGE_READWRITE
            )
            );
    }

    Str.Length = Value.length();

    if (Str.Length > 15)
    {
        Write<RbxString>(Address, Str);
        Driver_WriteVirtualMemory(
            Process_Handle,
            reinterpret_cast<void*>(Str.Data.Pointer),
            (void*)Value.data(),
            Value.length(),
            nullptr
        );
    }
    else
    {
        Str.Capacity = 15;
        Write<RbxString>(Address, Str);
        Driver_WriteVirtualMemory(
            Process_Handle,
            reinterpret_cast<void*>(Address),
            (void*)Value.data(),
            Value.length(),
            nullptr
        );
    }
}

std::size_t Driver_t::WriteRaw(std::uint64_t Address, const void* Data, std::size_t Size)
{
    if (!Process_Handle || !Address || !Data || !Size) return 0;
    ULONG Written = 0;
    Driver_WriteVirtualMemory(Process_Handle, reinterpret_cast<void*>(Address), const_cast<void*>(Data), static_cast<ULONG>(Size), &Written);
    return static_cast<std::size_t>(Written);
}

std::size_t Driver_t::ReadRaw(std::uint64_t Address, void* Buffer, std::size_t Size)
{
    if (!Process_Handle || !Address || !Buffer || !Size) return 0;
    ULONG Read = 0;
    Driver_ReadVirtualMemory(Process_Handle, reinterpret_cast<void*>(Address), Buffer, static_cast<ULONG>(Size), &Read);
    return static_cast<std::size_t>(Read);
}

std::uint64_t Driver_t::Alloc(std::size_t Size, DWORD Protect)
{
    if (!Process_Handle || !Size) return 0;
    return reinterpret_cast<std::uint64_t>(
        VirtualAllocEx(Process_Handle, nullptr, Size, MEM_RESERVE | MEM_COMMIT, Protect)
    );
}

bool Driver_t::Free(std::uint64_t Address)
{
    if (!Process_Handle || !Address) return false;
    return VirtualFreeEx(Process_Handle, reinterpret_cast<void*>(Address), 0, MEM_RELEASE) != 0;
}

std::uint64_t Driver_t::Get_Module(const wchar_t* ModuleName)
{
    if (!Process_Handle || !ModuleName) return 0;

    DWORD PID = GetProcessId(Process_Handle);
    HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, PID);
    if (Snapshot == INVALID_HANDLE_VALUE) return 0;

    MODULEENTRY32W Entry{};
    Entry.dwSize = sizeof(Entry);
    std::uint64_t Result = 0;

    if (Module32FirstW(Snapshot, &Entry))
    {
        do
        {
            if (!_wcsicmp(ModuleName, Entry.szModule))
            {
                Result = reinterpret_cast<std::uint64_t>(Entry.modBaseAddr);
                break;
            }
        } while (Module32NextW(Snapshot, &Entry));
    }

    CloseHandle(Snapshot);
    return Result;
}

bool Driver_t::IsValid(std::uint64_t Address)
{
    return Address >= 0x10000ull && Address < 0x00007FFFFFFFFFFFull;
}

std::uint32_t Driver_t::Get_Process()
{
    return Process_ID;
}

std::uint64_t Driver_t::Get_Module()
{
    return Base_Address;
}

HANDLE Driver_t::Get_Handle()
{
    return Process_Handle;
}