/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#define PRINT_TRACE_STACK_SIZE 32

#ifdef _WIN32
//# include <dbghelp.h>
//# include <winbase.h>
void print_trace(int frame_start)
{
    // Mingw doesnt have CaptureStackBackTrace
    //void* stack[PRINT_TRACE_STACK_SIZE];
    //HANDLE process = GetCurrentProcess();
    //SymInitialize(process, NULL, TRUE);
    //unsigned short frames = CaptureStackBackTrace(0, PRINT_TRACE_STACK_SIZE, stack, NULL);
    //if (size < PRINT_TRACE_STACK_SIZE && size > 2)
        //size -= 2; // ignore main and __libc_start_main
    //const size_t len = 0xFF;
    //SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + len, sizeof(char));
    //symbol->MaxNameLen = len;
    //symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    //for (unsigned int i=frame_start; i<frames; i++)
    //{
        //SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
        //printf("%d: %s - 0x%0X\n", frames - i - 1, symbol->Name, symbol->Address);
    //}
    //free(symbol);
}
#else
# include <execinfo.h>
# include <cxxabi.h>
void print_trace(int frame_start)
{
    void* array[PRINT_TRACE_STACK_SIZE] = { NULL };
    int size = backtrace(array, PRINT_TRACE_STACK_SIZE);
    if (frame_start >= size) return;
    char** strings = backtrace_symbols(array, size);
    if (size < PRINT_TRACE_STACK_SIZE && size > 2)
        size -= 2; // ignore main and __libc_start_main
    size_t len = 64 * sizeof(char);
    char* demangled_name = (char*)malloc(len);
    int status = 0;
    printf("----------------------------\n");
    for (int i = frame_start; i < size; i++)
    {
        char* s = NULL;
        const char* addr = strchr(strings[i], '[');
        char* name_start = strchr(strings[i], '(');
        if (name_start == NULL || name_start[1] == ')')
        {
            s = strings[i];
            addr = NULL;
        }
        else
        {
            s = &name_start[1];
            char* end = strchr(strings[i], '+');
            if (end != NULL)
                end[0] = '\0';
        }
        size_t _len = len;  // the addr passed to demangle is r/w
        char* name = abi::__cxa_demangle(s, demangled_name, &_len, &status);
        if (name != NULL)
        {
            demangled_name = name;
            s = name;
        }
        printf("%d: %s", i-frame_start, s);
        if (addr != NULL)
            printf(" %s", addr);
        printf("\n");
        if (_len > len)
            len = _len;
    }
    free(demangled_name);
    free(strings);
}
#endif

void print_trace()
{
    print_trace(0);
}

#undef PRINT_TRACE_STACK_SIZE
