// FILP_01_01.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <windows.h>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <chrono>
#include <list>

using namespace std;
using namespace chrono;

class CAllocator {
public:
    void* alloc(size_t Size) {
        if (Size > BS_End - iterator) {
            Make_Buffer(Size);
        }
        char* old_iterator = iterator;
        iterator += Size;
        return old_iterator;
    }

    void free(void* ptr) {

    }

    size_t BS = 10*1024*1024;
    list<char*> ListBS;
    char* iterator;
    char* BS_End;
    CAllocator() {
        Make_Buffer(0);
    }

    ~CAllocator() {
        for (char* element : ListBS) {
            ::free(element);
        }
    }

    void Make_Buffer(size_t Size) {
        size_t max_Size = max(BS, Size);
        char* Buffer = (char*) malloc(max_Size);
        ListBS.push_back(Buffer);
        BS_End = Buffer + max_Size;
        iterator = Buffer;
    }
};



CAllocator Allocator;

int main()
{
    HANDLE hFile;
    hFile = CreateFile(L"C:\\Users\\vlads\\Desktop\\FILP\\Tolstoy_Leo_War_nd_Peace.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        printf("\nFile not found\n %d", GetLastError());
        return 2;
    }
    

    LARGE_INTEGER s;
    BOOL hFileSize = GetFileSizeEx(hFile, &s);
    if (!hFileSize) {
        printf("\nSomething is wrong\n");
        return 1;
    }
    //cout << "File size: " << s.QuadPart;
    char* buffer = new char[s.QuadPart + 1];
    DWORD lpNumberOfBytesRead;
    BOOL hFileRead = ReadFile(hFile, buffer, s.QuadPart, &lpNumberOfBytesRead, NULL);
    if (!hFileSize) {
        printf("\nSomething is wrong\n");
        return 1;
    }
    //printf(Buffer);
    if (lpNumberOfBytesRead != s.QuadPart) {
        printf("\nThe number of bytes read differs from the number of bytes in the source file\n");
        return 3;
    }
    //printf("The file has been read");
    milliseconds startTime = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
        );

    struct cmpByStringLength {
        bool operator()(const char* a, const char* b) const {
            return strcmp(a, b) < 0;
        }
    };

    buffer[s.QuadPart] = '\0';
    char* start = buffer;
    map<char*, size_t, cmpByStringLength> words;
    for (int i = 0; i < s.QuadPart; i++) {
        if (!((buffer[i] >= 48 && buffer[i] <= 57) ||
            (buffer[i] >= 65 && buffer[i] <= 90) ||
            (buffer[i] >= 97 && buffer[i] <= 122))) {
            buffer[i] = '\0';
            if (start[0] != '\0') {
                    words[start]++;
            }
            start = &buffer[i + 1];
        }
    }

    vector<pair<char*, size_t>> vect;
    for (pair<char*, size_t> item : words) {
        vect.push_back(item);
    }

    sort(vect.begin(), vect.end(), []
    (pair<char*, size_t> a, pair<char*, size_t> b) {
        return a.second > b.second;
    });
    
    milliseconds endTime = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
        );

    int i = 0;
    for (pair<char*, size_t> item : vect) {
        if (++i > 10) break;
        printf("%s %d\n", item.first, (int) item.second);
    }

    milliseconds diff = endTime - startTime;
    cout << "\nProgram running time in milliseconds: " << diff.count() << '\n';
}