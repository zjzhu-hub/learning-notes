# 32-bit Inline Hook Implementation Guide

## Overview

This implementation demonstrates **inline hooking** (also known as function hooking or API hooking) in a 32-bit Windows environment. Inline hooking is a technique used to intercept and redirect function calls at runtime by modifying the target function's machine code in memory.

### How It Works

The implementation works by:
1. **Locating the target function** (`LoadLibraryA` from `kernel32.dll`)
2. **Calculating a jump offset** to redirect execution to a custom function
3. **Modifying memory protection** to allow writing to the function's code
4. **Overwriting the first 5 bytes** of the target function with a JMP instruction
5. **Restoring memory protection** to maintain system stability

The hook uses a **relative JMP instruction** (`0xE9`) followed by a 4-byte offset, totaling 5 bytes that replace the original function's prologue.

## Prerequisites

### Required Tools
- **32-bit C++ compiler** (essential for proper pointer arithmetic and calling conventions)
- **MSYS2 with mingw32** (recommended) or Visual Studio with 32-bit toolchain
- **Windows SDK** (for Windows API headers)

### Dependencies
- `windows.h` - Windows API functions
- `iostream` - Standard I/O operations
- `tchar.h` - Text character handling

## Compilation Instructions

### Using MSYS2/MinGW32

1. **Install MSYS2** and the 32-bit toolchain:
   ```bash
   # In MSYS2 terminal
   pacman -S mingw-w64-i686-gcc
   pacman -S mingw-w64-i686-gdb  # Optional: for debugging
   ```

2. **Set up the environment**:
   ```bash
   # Add to PATH or use MINGW32 shell
   export PATH="/mingw32/bin:$PATH"
   ```

3. **Compile the program**:
   ```bash
   # Basic compilation
   i686-w64-mingw32-g++ -m32 -o inlineHook.exe inlineHook.cpp
   
   # With debugging symbols
   i686-w64-mingw32-g++ -m32 -g -o inlineHook.exe inlineHook.cpp
   
   # Optimized release build
   i686-w64-mingw32-g++ -m32 -O2 -o inlineHook.exe inlineHook.cpp
   ```

### Using Visual Studio

1. **Create a new project** with Win32 Console Application template
2. **Set platform target to x86** (32-bit)
3. **Add the source file** and compile

### Important Compiler Flags

- **`-m32`**: Forces 32-bit compilation (critical for MinGW)
- **`-g`**: Includes debugging information
- **`-O2`**: Optimization level (optional for release builds)

## Usage Examples

### Basic Usage

```cpp
#include "inlineHook.cpp"  // Include the implementation

int main() {
    // Install the hook
    Hook();
    
    // Test the hook - this will trigger FakeLoadLibraryA
    LoadLibraryA("test.dll");
    
    // Optional: Remove the hook
    // UnHook();  // Note: UnHook function needs to be implemented
    
    return 0;
}
```

### Expected Behavior

When you run the compiled executable:
1. A message box appears saying "Inline Hook Success!"
2. When `LoadLibraryA("test.dll")` is called, it's intercepted
3. Instead of loading the DLL, `FakeLoadLibraryA` executes
4. A second message box appears saying "FakeLoadLibraryA"

## Code Structure Analysis

### Key Components

1. **Global Variables**:
   ```cpp
   BYTE oldData32[5];    // Stores original 5 bytes
   BYTE newData32[5];    // JMP instruction template
   ```

2. **Hook Function**:
   - Locates `LoadLibraryA` in `kernel32.dll`
   - Calculates jump offset: `target_address - current_address - 5`
   - Modifies memory protection with `VirtualProtect`
   - Overwrites function prologue with JMP instruction

3. **Fake Function**:
   ```cpp
   HMODULE WINAPI FakeLoadLibraryA(LPCSTR lpLibFileName)
   ```
   - Replacement function that gets called instead
   - Must match original function signature

## Important Notes

### Why 32-bit Compilation is Required

1. **Pointer Size**: 32-bit pointers (4 bytes) vs 64-bit pointers (8 bytes)
2. **Calling Conventions**: Different between x86 and x64
3. **Instruction Encoding**: JMP instruction format differs between architectures
4. **Address Space**: 32-bit applications have different memory layout

### Platform-Specific Considerations

- **Windows Only**: Uses Windows-specific APIs (`VirtualProtect`, `GetModuleHandle`)
- **x86 Architecture**: Designed specifically for 32-bit x86 processors
- **Memory Protection**: Requires appropriate privileges to modify executable memory

### Security Implications

- **Antivirus Detection**: May be flagged as potentially malicious
- **DEP/ASLR**: Modern protections may interfere with hooking
- **Code Signing**: Signed executables may have additional protections

## Troubleshooting

### Common Compilation Issues

1. **"undefined reference" errors**:
   - Ensure you're using 32-bit compiler
   - Link against required Windows libraries: `-lkernel32 -luser32`

2. **Access violation at runtime**:
   - Run as Administrator (may need elevated privileges)
   - Check if DEP (Data Execution Prevention) is interfering

3. **Hook doesn't work**:
   - Verify target function address is correct
   - Check if the target function has been patched by other software
   - Ensure memory protection changes succeeded

### Debugging Tips

1. **Print addresses** to verify calculations:
   ```cpp
   std::cout << "Target function address: " << std::hex << dwBasePfn << std::endl;
   std::cout << "Jump offset: " << std::hex << dwOffset << std::endl;
   ```

2. **Check VirtualProtect return value**:
   ```cpp
   if (!VirtualProtect(...)) {
       std::cout << "VirtualProtect failed: " << GetLastError() << std::endl;
   }
   ```

3. **Use a debugger** to step through the hooking process

## Extending the Implementation

### Adding UnHook Functionality

```cpp
void UnHook() {
    HMODULE hModule = GetModuleHandle(TEXT("Kernel32.dll"));
    DWORD dwBasePfn = (DWORD)GetProcAddress(hModule, "LoadLibraryA");
    
    DWORD oldProtected = 0;
    VirtualProtect((LPVOID)dwBasePfn, 0x1000, PAGE_EXECUTE_READWRITE, &oldProtected);
    
    // Restore original bytes
    memcpy((LPVOID)dwBasePfn, oldData32, 5);
    
    VirtualProtect((LPVOID)dwBasePfn, 0x1000, oldProtected, &oldProtected);
}
```

### Hooking Multiple Functions

Create separate data arrays and hook functions for each target function, following the same pattern as the `LoadLibraryA` example.

## References

- [Microsoft Documentation: VirtualProtect](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualprotect)
- [Intel x86 Instruction Set Reference](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [Windows API Hooking Techniques](https://docs.microsoft.com/en-us/windows/win32/debug/debugging-and-error-handling)
