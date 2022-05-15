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
            /* ディレクトリの場合は再帰探索 */
            std::string subFolderPath = win32fd.cFileName;
            if ((subFolderPath != ".") && (subFolderPath != "..")) {
                setFontFiles(folderPath + "/" + subFolderPath);
            }
        }
        else {
            std::string fontpath_s = std::string(folderPath) + "/" + win32fd.cFileName;
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
            // ただしGUIからのexeの直接起動と異なりショートカット経由や外部呼出しではカレントは異なる場合があるため、相対パスではAviUtil内のfontsを参照できなかったりカレント内の意図しないfontsフォルダを参照してしまう場合があるのでこれは行わない

//            if (PathIsDirectory(FONTS_DIR)) {
//                setFontFiles(FONTS_DIR);
//                return TRUE;
//            }

            // カレントを使用せずに実行中ファイルのパスからAviUtl直下を算出する
            TCHAR modulePath[MAX_PATH];
            if (!GetModuleFileName(0, modulePath, MAX_PATH)) {
                MessageBox(NULL, TEXT("Error: GetModuleFileName"),NULL, MB_OK);
                return TRUE;
            }
            if(PathRemoveFileSpec(modulePath) == 0) {
                MessageBox(NULL, TEXT("Error: PathRemoveFileSpec"),NULL, MB_OK);
                MessageBox(NULL, TEXT(modulePath),NULL, MB_OK);
                return TRUE;
            }
            std::string modulePathString = modulePath;
            std::string fontsFullPath = modulePathString + "\\" + FONTS_DIR;
            if (!PathIsDirectory(fontsFullPath.c_str())) {
                MessageBox(NULL, TEXT("Fonts directory is missing"),NULL, MB_OK);
                MessageBox(NULL, fontsFullPath.c_str(),NULL, MB_OK);
            } else {
                setFontFiles(fontsFullPath);
            }
        }
    return TRUE;
}

int main(int, char**) {}