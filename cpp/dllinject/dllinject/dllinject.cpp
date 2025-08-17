#include <windows.h>
#include <iostream>
#include <tchar.h>

int main()
{

    // 显示加载dll的路径
    char dllPath[] = "D:\\develop\\msys64\\home\\zjzhu\\projects\\learning-notes\\cpp\\dllinject\\dll\\build\\libMyDLL.dll";
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 2908); // 打开wechat进程，获取句柄 2908是会变的这里测试用每次需要更新
    if (hProcess == NULL)
    {
        printf("OpenProcess failed\n");
        return 1;
    }

    // 这里取的Kernel32.dll 是自己进程的 因为wechat进程也是用的这个dll基地址
    HMODULE hModule = GetModuleHandle(TEXT("Kernel32.dll"));
    if (hModule == NULL)
    {
        printf("GetModuleHandle failed\n");
        return 1;
    }

    // 从kernel32.dll中获取 LoadLibraryA函数地址
    auto pLoadLibraryA = GetProcAddress(hModule, "LoadLibraryA");
    if (pLoadLibraryA == NULL)
    {
        printf("GetProcAddress failed\n");
        return 1;
    }

    // 在目标进程（wechat）的地址空间中分配内存（在wechat开辟一块内存空间）
    LPVOID pDLLPathStr = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_READWRITE);

    // 将dll路径写入目标进程（wechat）的地址空间 （在上面开辟的内存空间写入dll路径）
    BOOL bRet = WriteProcessMemory(hProcess, pDLLPathStr, dllPath, strlen(dllPath), NULL);
    if (!bRet)
    {
        printf("WriteProcessMemory failed\n");
        return 1;
    }

    // 创建远程线程进行注入dll, 这里相对于使用前面开辟的内存空间 调用LoadLibraryA(dllPath) 进行注入
    HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryA, pDLLPathStr, 0, NULL);
    if (hRemoteThread == NULL)
    {
        printf("CreateRemoteThread failed\n");
        return 1;
    }

    WaitForSingleObject(hRemoteThread, INFINITE);

    CloseHandle(hRemoteThread);
    CloseHandle(hProcess);

    return 0;
}
