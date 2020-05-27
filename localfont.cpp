#include <windows.h>
#include <wingdi.h>
#include <iostream>
#include <shlwapi.h>

#define FONTS_DIR "fonts"

bool setFontFiles(std::string folderPath)
{
    HANDLE hFind;
    WIN32_FIND_DATA win32fd;
    std::string search_name = folderPath + "\\*";

    hFind = FindFirstFile(search_name.c_str(), &win32fd);

    if (hFind == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("file not found");
        return false;
    }

    /* 指定のディレクトリ以下のファイル名をファイルがなくなるまで取得する */
    do {
        if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            /* ディレクトリの場合は何もしない */
        }
        else {
            std::string fontpath_s = std::string(FONTS_DIR) + "/" + win32fd.cFileName;
            LPCSTR fontpath = fontpath_s.c_str();
            // MessageBox(NULL, fontpath,NULL, MB_OK);
            // フォント以外も追加しようとするが無効なのでフィルターより全投げ
            AddFontResourceEx(fontpath, FR_PRIVATE, 0);
        }
    } while (FindNextFile(hFind, &win32fd));

    FindClose(hFind);

    return true;
}

BOOL WINAPI DllMain( HINSTANCE hInstance,DWORD dwNotification,LPVOID lpReserved )
{
    switch (dwNotification) {
        case DLL_PROCESS_ATTACH:
            // exe直下でもplugins内設置でもカレントはaviutl.exe直下になる
            if (!PathIsDirectory(FONTS_DIR)) {
                MessageBox(NULL, TEXT("Fonts directory is missing"),NULL, MB_OK);
                return TRUE;
            }
            setFontFiles(FONTS_DIR);
    }
    return TRUE;
}

int main(int, char**) {}