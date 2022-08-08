#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <WinHttp.h>

#pragma comment(lib, "winhttp.lib")

char* Get(char* ip, unsigned int port, char* uri)
{
    wchar_t* sip = get_utf16(ip, CP_UTF8);
    wchar_t* suri = get_utf16(uri, CP_UTF8);

    wprintf(L"%s\n", sip);
    wprintf(L"%s\n", suri);

    char* response = NULL;
    response = (char*)malloc((24000) * sizeof(char));
    ZeroMemory(response, 24000);

    LPSTR pszOutBuffer;

    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    BOOL  bResults = FALSE;

    HINTERNET hSession = NULL,
        hConnect = NULL,
        hRequest = NULL;

    hSession = WinHttpOpen(L"test",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);

    if (hSession) 
    {
        hConnect = WinHttpConnect(hSession,
            sip,
            port,
            0);
    }
    else
        printf("Error %u in WinHttpOpen.\n", GetLastError());

    if (hConnect) 
    {
        hRequest = WinHttpOpenRequest(hConnect,
            L"GET", suri,
            NULL,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            0);
    }
    else
        printf("Error %u in WinHttpConnect.\n", GetLastError());

    if (hRequest) 
    {
        bResults = WinHttpSendRequest(hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0,
            WINHTTP_NO_REQUEST_DATA,
            0,
            0,
            0);
    }
    else
        printf("Error %u in WinHttpOpenRequest.\n", GetLastError());

    if (bResults) {
        bResults = WinHttpReceiveResponse(hRequest, NULL);
    }

    if (bResults)
    {
        do
        {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {}

            pszOutBuffer = (char*)malloc((dwSize + 1) * sizeof(char));
            if (!pszOutBuffer)
            {
                dwSize = 0;
            }
            else
            {
                ZeroMemory(pszOutBuffer, dwSize + 1);

                if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {}
                else {
                    strcat(response, pszOutBuffer);
                    free(pszOutBuffer);
                }
            }
        } while (dwSize > 0);
    }

    if (!bResults)
        printf("Error %d has occurred.\n", GetLastError());

    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);

    return response;
}

int main()
{
    char* ip = "httpbin.org";
    unsigned int port = 80;

    char* get = Get(ip, port, "/get");
    printf("%s\n", get);

    free(get);

    return 0;
}
