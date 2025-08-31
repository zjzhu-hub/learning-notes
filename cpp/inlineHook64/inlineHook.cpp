#include <iostream>
#include <windows.h>
#include <tchar.h>

// 定义一个函数指针 其实就是定义一个约定 模具
typedef HMODULE(WINAPI *lpLoadLibraryA)(LPCSTR);

BYTE oldData64[14] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// BYTE newData32[5] = {0xE9, 0x0, 0x0, 0x0, 0x0}; // 相对跳转指令 步长 4字节 8位 4GB 太大可能跳不过去
BYTE newData64[6] = {0xFF, 0x25, 0x0, 0x0, 0x0, 0x0};                  // 0xFF25是绝对跳转指令  后面8个字节是操作数（跳到哪里去）
BYTE destAddr64[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 8字节的目的地址 也就是FakeLoadLibraryA的地址

void UnHook();
void Hook();
// 仿造一个 LoadLibraryA
HMODULE WINAPI FakeLoadLibraryA(LPCSTR lpLibFileName)
{
    // UnHook();
    MessageBox(NULL, TEXT("FakeLoadLibraryA"), TEXT("Message"), MB_OK);
    // 获取 LoadLibraryA 函数在内存中的实际地址
    // GetProcAddress 返回的是 FARPROC（一个通用函数指针）
    // 需要将其转换为我们定义的特定类型 也就是 lpLoadLibraryA
    // lpLoadLibraryA loadLibraryAPfn = (lpLoadLibraryA)GetProcAddress(GetModuleHandle(TEXT("Kernel32.dll")), "LoadLibraryA");
    // 现在，可以像使用真正的 LoadLibraryA 一样使用这个指针, 这相当于调用 HMODULE h = LoadLibraryA(lpLibFileName);
    // HMODULE hModule = loadLibraryAPfn(lpLibFileName); // 调用真正的LoadLibraryA 这里会弹出一个框 说test.dll不存在  因为我随便写的dll 不存在  但是我们已经Hook了LoadLibraryA 所以会弹出两个框 一个是FakeLoadLibraryA 一个是test.dll不存在
    Hook();
    return NULL;
};

void Hook()
{
    // 这里拿到LoadLibraryA的函数指针地址
    DWORD64 dwBasePfn = (DWORD64)GetProcAddress(GetModuleHandle(TEXT("Kernel32.dll")), "LoadLibraryA");
    if (dwBasePfn == NULL)
    {
        printf("GetProcAddress failed\n");
        return;
    }
    std::cout << "dwBasePfn: " << dwBasePfn << std::endl;

    // 中转一下取到FakeLoadLibraryA的地址
    DWORD64 tmpPfnAddr = (DWORD64)FakeLoadLibraryA;

    // 保存破坏前的14个字节, 从LoadLibraryA地址开始的14个字节， 因为等下需要写入14个字节
    memcpy(oldData64, (LPVOID)dwBasePfn, 14);
    // 保存FakeLoadLibraryA函数目的地址
    memcpy(destAddr64, &tmpPfnAddr, 8);

    // 旧的内存页属性 用于恢复现场
    DWORD oldProtected = 0;
    // 修改内存页属性可写可执行。内存区域的起始地址, 内存区域的大小(默认是按照页修改 填写5和0x1000是一样的),  新的内存保护标志(这里改为可读可写可执行),  旧的内存保护标志的指针
    VirtualProtect((LPVOID)dwBasePfn, 0x1000, PAGE_EXECUTE_READWRITE, &oldProtected);

    // 写入跳转指令
    memcpy((LPVOID)dwBasePfn, newData64, sizeof(newData64)); // 写入跳转指令 这里是直接拷贝内存 所以需要修改内存页属性为可写可执行  也可以用WriteProcessMemory 但是需要获取目标进程句柄
    // 填入目的地址 这里+6 是前面指令长度是6需要向下写
    memcpy((LPVOID)(dwBasePfn + 6), destAddr64, sizeof(destAddr64));
    // 内存属性恢复
    VirtualProtect((LPVOID)dwBasePfn, 0x1000, oldProtected, &oldProtected);
}

void UnHook()
{

    // 先拿到正常的LoadLibraryA地址
    HMODULE hModule = GetModuleHandle(TEXT("Kernel32.dll"));
    if (hModule == NULL)
    {
        printf("GetModuleHandle failed\n");
        return;
    }
    // 这里拿到LoadLibraryA的函数指针地址
    FARPROC pLoadLibraryA = GetProcAddress(hModule, "LoadLibraryA");
    if (pLoadLibraryA == NULL)
    {
        printf("GetProcAddress failed\n");
        return;
    }
    DWORD64 dwBasePfn = (DWORD64)pLoadLibraryA;
    if (dwBasePfn == NULL)
    {
        printf("GetProcAddress failed\n");
        return;
    }
    std::cout << "dwBasePfn: " << dwBasePfn << std::endl;

    // 旧的内存页属性 用于恢复现场
    DWORD oldProtected = 0;
    VirtualProtect((LPVOID)dwBasePfn, 0x1000, PAGE_EXECUTE_READWRITE, &oldProtected);

    // 写入跳转指令
    memcpy((LPVOID)dwBasePfn, oldData64, sizeof(oldData64)); // 写入跳转指令 这里是直接拷贝内存 所以需要修改内存页属性为可写可执行  也可以用WriteProcessMemory 但是需要获取目标进程句柄
    // 内存属性恢复
    VirtualProtect((LPVOID)dwBasePfn, 0x1000, oldProtected, &oldProtected);
}

int main()
{
    Hook();
    MessageBox(NULL, TEXT("Inline Hook Success!"), TEXT("Message"), MB_OK); // 这里会调用LoadLibraryA 但是被Hook了 所以会弹出两个框 一个是FakeLoadLibraryA 一个是Inline Hook Success!  这里的test.dll是我随便写的dll 不存在
    LoadLibraryA("test.dll");
    UnHook();
    MessageBox(NULL, TEXT("UnHook Success!"), TEXT("Message"), MB_OK);
    return 0;
}
