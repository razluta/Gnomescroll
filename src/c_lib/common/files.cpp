/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "files.hpp"

#include <sys/stat.h>
#include <limits.h>
#ifdef _WIN32
# include <shlobj.h>
# ifdef __GNUC__
#  include <io.h>
# endif
#endif

#include <common/macros.hpp>
#include <common/dirent.h>

int GS_MKDIR(const char* dir, int permissions)
{
    #ifdef _WIN32
    # ifdef __MSVC__
    return _mkdir(dir);
    # else
    return mkdir(dir);
    # endif
    #else
    return mkdir(dir, S_IRWXU);
    #endif
}

int GS_RENAME(const char* src, const char* dst)
{
    #ifdef _WIN32
    remove(dst);
    #endif

    return rename(src, dst);
}

off_t get_filesize(const char *filename)
{
    struct stat st;
    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1;
}

// free the returned char* buffer after use
static char* read_file_to_buffer(const char* filename, size_t* size, const char* mode)
{
    IF_ASSERT(strstr(mode, "r") == NULL) return NULL;

    *size = 0;
    off_t expected_size = get_filesize(filename);
    char *source = NULL;
    FILE *fp = fopen(filename, mode);
    if (fp == NULL)
    {
        printf("read_file_to_buffer: error, could not open %s\n", filename);
        return NULL;
    }

    /* Go to the end of the file. */
    if (fseek(fp, 0L, SEEK_END) == 0)
    {
        /* Get the size of the file. */
        long bufsize = ftell(fp);
        if (bufsize != (long)expected_size)
        {
            printf("%s:%d - expected file size does not match reported size file %s\n", __FUNCTION__, __LINE__, filename);
            fclose(fp);
            return NULL;
        }
        if (bufsize < 0)
        {
            printf("%s:%d - error stat'ing file %s\n", __FUNCTION__, __LINE__, filename);
            fclose(fp);
            return NULL;
        }
        if (bufsize > LONG_MAX-2)
        {
            printf("%s:%d - file too large %s\n", __FUNCTION__, __LINE__, filename);
            fclose(fp);
            return NULL;
        }
        if ( ((unsigned long) bufsize) > ((size_t)-1) )
        {
            printf("%s:%d - file size is larger than size_t max! %s\n", __FUNCTION__, __LINE__, filename);
            fclose(fp);
            return NULL;
        }
        /* Allocate our buffer to that size. */
        source = (char*) calloc(bufsize+2, sizeof(char));
        /* Go back to the start of the file. */
        if (fseek(fp, 0L, SEEK_SET) != 0)
        {
            free(source);
            fclose(fp);
            printf("read_file_to_buffer: error seeking file %s\n", filename);
            return NULL;
        }

        /* Read the entire file into memory. */
        size_t newLen = fread(source, sizeof(char), bufsize, fp);
        if (ferror(fp))
        {
            free(source);
            fclose(fp);
            printf("read_file_to_buffer: error reading file %s\n", filename);
            return NULL;
        }
        *size = newLen;
        source[++newLen] = '\0'; /* Just to be safe. */
    }
    int err = fclose(fp);
    GS_ASSERT(!err);
    if (err) printf("Error closing file %s\n", filename);

    return source;
}

// free the returned char* buffer after use
char* read_file_to_buffer(const char* filename, size_t* size)
{
    return read_file_to_buffer(filename, size, "r");
}

// free the returned char* buffer after use
char* read_binary_file_to_buffer(const char* filename, size_t* size)
{
    return read_file_to_buffer(filename, size, "rb");
}

int count_lines(char* buffer)
{
    int lines = 0;
    int i = 0;
    char c;
    while ((c = buffer[i++]) != '\0')
        if (c == '\n')
            lines++;
    return lines;
}

int count_lines(char* buffer, int* longest)
{
    int lines = 0;
    int length = 0;
    int i = 0;
    char c;
    while ((c = buffer[i++]) != '\0')
    {
        if (c == '\n')
        {
            if (length > *longest)
                *longest = length;
            length = 0;
            lines++;
        }
        else
            length++;
    }
    if (length > *longest)
        *longest = length;
    return lines;
}

void free_read_lines(char** readlines, int lines)
{
    if (readlines == NULL) return;
    for (int i=0; i<lines; i++)
        free(readlines[i]);
    free(readlines);
}

//// free the returned char** array after use
char** read_lines(char* buffer, int* lines)
{
    int longest = 0;
    *lines = count_lines(buffer, &longest);
    char** arr = (char**)malloc(sizeof(char*)**lines);
    for (int i=0; i<*lines; i++)
        arr[i] = (char*)malloc(sizeof(char)*(longest+1));
    int i = 0;
    int j = 0;
    int n = 0;
    char c;
    while ((c = buffer[i++]) != '\0')
    {
        if (c == '\n')
        {
            arr[n][j] = '\0';
            if (j < longest+1)
                arr[n] = (char*)realloc(arr[n], j+1);
            j = 0;
            n++;
        }
        else
            arr[n][j++] = c;
    }
    return arr;
}

#ifdef _WIN32
static const char SEPARATOR = '\\';
#else
static const char SEPARATOR = '/';
#endif

void create_path(const char* fn)
{   // walk down the path, create folders as needed
    size_t len = strlen(fn);
    char* path = (char*)malloc((len+1) * sizeof(char));
    strcpy(path, fn);

    struct stat file_stat;

    char c;
    int i=0;
    while ((c = path[i++]) != '\0')
    {
        if (c != SEPARATOR) continue;
        path[i-1] = '\0';
        if (stat(path, &file_stat) != 0)
        {
            GS_MKDIR(path, 0777);
        }
        path[i-1] = SEPARATOR;
    }

    if (stat(path, &file_stat) != 0)
        GS_MKDIR(path, 0777);

    free(path);
}

void create_path_to_file(const char* fn)
{
    int i = ((int)strlen(fn)) - 1;
    for (; i>=0; i--)
        if (fn[i] == SEPARATOR)
            break;
    GS_ASSERT(i >= 0);
    if (i < 0) return;

    char* path = (char*)malloc((i+1) * sizeof(char));
    int j=0;
    for (; j<i; j++) path[j] = fn[j];
    path[j] = '\0';
    create_path(path);
    free(path);
}

bool file_exists(const char* filename)
{
    struct stat file_stat;
    return (stat(filename, &file_stat) == 0);
}

bool save_tmp_file(const char* fn, const char* fn_tmp, const char* fn_bak)
{   // Saves a tmp file to real filename, backing up old filename
    if (file_exists(fn))
    {
        int ret = GS_RENAME(fn, fn_bak);
        IF_ASSERT(ret != 0) return false;
    }
    int ret = GS_RENAME(fn_tmp, fn);
    GS_ASSERT(ret == 0);
    return (ret == 0);
}

// reads lines of a file into fixed width per-line continuous array
// return false on error (e.g., a line is wider than width)
bool read_fixed_lines(const char* buf, const size_t width, char** lines, size_t* count)
{
    *lines = NULL;
    *count = 0;

    for (size_t i=0; buf[i] != '\0'; i++)
        if (buf[i] == '\n')
            (*count)++;

    if (!(*count)) return true;

    char* _lines = (char*)malloc((width+1)*(*count) * sizeof(char));

    size_t m = 0;   // tracks length of single line
    size_t n = 0;   // tracks length in char* lines array
    size_t _count = 0;
    for (size_t i=0; buf[i] != '\0'; i++)
    {
        if (m > width)
        {
            free(_lines);
            *lines = NULL;
            *count = 0;
            return false;
        }

        char c = buf[i];
        if (c == '\n')
        {
            for (size_t j=m; j<width+1; j++)
                _lines[n++] = '\0';
            _count++;
            m = 0;
        }
        else
        {
            m++;
            _lines[n++] = c;
        }
    }

    *count = _count;
    *lines = _lines;

    return true;
}

void get_home_directory(char*& home)
{
    #if DC_CLIENT
    if (Options::data_directory != NULL && Options::data_directory[0] != '\0')
    {
        home = (char*)malloc((strlen(Options::data_directory) + 1) * sizeof(*home));
        strcpy(home, Options::data_directory);
        return;
    }
    #endif

    #ifdef _WIN32
    // Window appdata / user home directory
    LPTSTR _home = (LPTSTR)calloc(MAX_PATH+1, sizeof(*_home));
    HRESULT result = SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL,
                                     SHGFP_TYPE_CURRENT, _home);
    if (!SUCCEEDED(result))
    {
        size_t len = _tcslen(_home);
        #ifdef UNICODE
        len *= 2;
        #endif
        home = (char*)calloc((len + 1), sizeof(*home));
        #ifdef UNICODE
        wcstombs(home, _home, len + 1);
        #else
        strncpy(home, _home, len + 1);
        #endif
        home[len] = '\0';
    }
    else
    {
        home = NULL;
    }
    free(_home);

    #else
    // Linux/OSX home directory
    const char* _home = getenv("HOME");
    if (_home == NULL)
    {
        const struct passwd* p = getpwuid(getuid());
        if (p != NULL)
            _home = p->pw_dir;
    }
    size_t len = strlen(_home) + 1;
    home = (char*)malloc(len * sizeof(*home));
    strncpy(home, _home, len);
    home[len-1] = '\0';
    #endif
}
