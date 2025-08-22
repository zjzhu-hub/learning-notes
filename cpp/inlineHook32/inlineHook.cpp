#include <iostream>
#include <windows.h>
#include <tchar.h>

// 定义一个函数指针 其实就是定义一个约定 模具
typedef HMODULE(WINAPI *lpLoadLibraryA)(LPCSTR);

BYTE oldData32[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
BYTE newData32[5] = {0xE9, 0x0, 0x0, 0x0, 0x0}; // 0xE9是jmp指令操作码（相对跳转）硬编码指令  后面4个字节是操作数（跳多长的距离）
// JMP 步长（操作数）= 目的地址 - 当前指令地址 - 当前指令的长度（5字节）

void UnHook();
void Hook();
// 仿造一个 LoadLibraryA
HMODULE WINAPI FakeLoadLibraryA(LPCSTR lpLibFileName)
{
    UnHook();
    MessageBox(NULL, TEXT("FakeLoadLibraryA"), TEXT("Message"), MB_OK);
    // 获取 LoadLibraryA 函数在内存中的实际地址
    // GetProcAddress 返回的是 FARPROC（一个通用函数指针）
    // 需要将其转换为我们定义的特定类型 也就是 lpLoadLibraryA
    lpLoadLibraryA loadLibraryAPfn = (lpLoadLibraryA)GetProcAddress(GetModuleHandle(TEXT("Kernel32.dll")), "LoadLibraryA");
    // 现在，可以像使用真正的 LoadLibraryA 一样使用这个指针, 这相当于调用 HMODULE h = LoadLibraryA(lpLibFileName);
    HMODULE hModule = loadLibraryAPfn(lpLibFileName); // 调用真正的LoadLibraryA 这里会弹出一个框 说test.dll不存在  因为我随便写的dll 不存在  但是我们已经Hook了LoadLibraryA 所以会弹出两个框 一个是FakeLoadLibraryA 一个是test.dll不存在
    Hook();
    return hModule;
};

void Hook()
{
    // 这里拿到LoadLibraryA的函数指针地址
    DWORD dwBasePfn = (DWORD)GetProcAddress(GetModuleHandle(TEXT("Kernel32.dll")), "LoadLibraryA");
    if (dwBasePfn == NULL)
    {
        printf("GetProcAddress failed\n");
        return;
    }
    std::cout << "dwBasePfn: " << dwBasePfn << std::endl;

    // 计算JMP指令步长 目标函数指针地址 - LoadLibraryA地址 - 5
    DWORD dwOffset = (DWORD)FakeLoadLibraryA - dwBasePfn - 5;
    std::cout << "dwOffset: " << dwOffset << std::endl;

    // 保存破坏前的5个字节, 从LoadLibraryA地址开始的5个字节
    memcpy(oldData32, (PVOID)dwBasePfn, 5);
    // 拼接填充JMP指令的操作数（跳多长的距离）从第一个字节开始
    memcpy(&newData32[1], &dwOffset, 4);

    // 旧的内存页属性 用于恢复现场
    DWORD oldProtected = 0;
    // 修改内存页属性可写可执行。内存区域的起始地址, 内存区域的大小(默认是按照页修改 填写5和0x1000是一样的),  新的内存保护标志(这里改为可读可写可执行),  旧的内存保护标志的指针
    VirtualProtect((LPVOID)dwBasePfn, 0x1000, PAGE_EXECUTE_READWRITE, &oldProtected);

    // 写入跳转指令
    memcpy((LPVOID)dwBasePfn, newData32, sizeof(newData32)); // 写入跳转指令 这里是直接拷贝内存 所以需要修改内存页属性为可写可执行  也可以用WriteProcessMemory 但是需要获取目标进程句柄
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
    DWORD dwBasePfn = (DWORD)GetProcAddress(hModule, "LoadLibraryA");
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
    memcpy((LPVOID)dwBasePfn, oldData32, sizeof(oldData32)); // 写入跳转指令 这里是直接拷贝内存 所以需要修改内存页属性为可写可执行  也可以用WriteProcessMemory 但是需要获取目标进程句柄
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
