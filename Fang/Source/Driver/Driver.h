#pragma once
#include <windows.h>
#include <TlHelp32.h>
#include <vector>
#include <string>
#include <memory>

extern "C" intptr_t Driver_ReadVirtualMemory(
    HANDLE ProcessHandle,
    PVOID BaseAddress,
    PVOID Buffer,
    ULONG NumberOfBytesToRead,
    PULONG NumberOfBytesRead
);

extern "C" intptr_t Driver_WriteVirtualMemory(
    HANDLE ProcessHandle,
    PVOID BaseAddress,
    PVOID Buffer,
    ULONG NumberOfBytesToWrite,
    PULONG NumberOfBytesWritten
);

union RbxStringData {
    char Inline[16];
    std::uint64_t Pointer;
};

struct RbxString {
    RbxStringData Data;
    std::uint64_t Length;
    std::uint64_t Capacity;
};

class Driver_t final {
public:
    Driver_t() = default;
    ~Driver_t() = default;

    std::uint32_t Find_Process(const std::string& Process_Name);
    std::uint64_t Find_Module(const std::string& Module_Name);
    bool Attach_Process(const std::string& Process_Name);

    std::string Read_String(std::uint64_t Address);
    void Write_String(std::uint64_t Address, const std::string& Value);

    template <typename T>
    T Read(std::uint64_t Address);

    template <typename T>
    void Write(std::uint64_t Address, T Value);

    std::size_t WriteRaw(std::uint64_t Address, const void* Data, std::size_t Size);
    std::size_t ReadRaw(std::uint64_t Address, void* Buffer, std::size_t Size);
    std::uint64_t Alloc(std::size_t Size, DWORD Protect);
    bool Free(std::uint64_t Address);
    std::uint64_t Get_Module(const wchar_t* ModuleName);
    bool IsValid(std::uint64_t Address);

    std::uint32_t Get_Process();
    std::uint64_t Get_Module();
    HANDLE Get_Handle();

private:
    std::uint32_t Process_ID;
    std::uint64_t Base_Address;
    HANDLE Process_Handle;
};

template <typename T>
T Driver_t::Read(std::uint64_t Address) {
    T Buffer{};
    Driver_ReadVirtualMemory(Process_Handle, reinterpret_cast<void*>(Address), &Buffer, sizeof(T), nullptr);
    return Buffer;
}

template <typename T>
void Driver_t::Write(std::uint64_t Address, T Value) {
    Driver_WriteVirtualMemory(Process_Handle, reinterpret_cast<void*>(Address), &Value, sizeof(T), nullptr);
}

inline std::unique_ptr<Driver_t> Driver = std::make_unique<Driver_t>();