#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <iostream>
#include <vector>
#include <fstream>

bool CompareMemoryWithDisk(HANDLE hProcess, const std::wstring& exePath) {
    BYTE memBuffer[0x1000] = { 0 };
    SIZE_T bytesRead = 0;
    if (!ReadProcessMemory(hProcess, (LPCVOID)0x400000, memBuffer, sizeof(memBuffer), &bytesRead)) {
        return false;
    }

    std::ifstream exeFile(exePath, std::ios::binary);
    if (!exeFile.is_open()) return false;

    std::vector<BYTE> diskBuffer(0x1000);
    exeFile.read(reinterpret_cast<char*>(diskBuffer.data()), 0x1000);
    exeFile.close();

    return memcmp(memBuffer, diskBuffer.data(), 0x1000) == 0;
}

std::wstring GetProcessPath(DWORD pid) {
    std::wstring path(MAX_PATH, L'\0');
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess) return L"";

    if (GetModuleFileNameExW(hProcess, nullptr, &path[0], MAX_PATH)) {
        CloseHandle(hProcess);
        return std::wstring(path.c_str());
    }

    CloseHandle(hProcess);
    return L"";
}

void ScanProcesses() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return;
    }

    do {
        std::wstring procName = pe32.szExeFile;
        std::wstring procPath = GetProcessPath(pe32.th32ProcessID);

        HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
        if (hProcess) {
            bool isClean = CompareMemoryWithDisk(hProcess, procPath);
            std::wcout << L"[+] PID: " << pe32.th32ProcessID << L" | " << procName
                       << (isClean ? L" - OK" : L" - ⚠️ Possible Hollowing!") << std::endl;
            CloseHandle(hProcess);
        }

    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
}

int wmain() {
    std::wcout << L"=== Process Hollowing Detection Tool ===" << std::endl;
    ScanProcesses();
    return 0;
}
