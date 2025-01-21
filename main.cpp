#include <iostream>
#include <windows.h>
#include <userenv.h>
#include <Windows.h>
#include <Lm.h>


using namespace std;

#pragma comment(lib, "userenv.lib")

NET_API_STATUS CreateUserAccount(wstring const & accountName, wstring const & password)
{
    USER_INFO_4 userInfo;
    ::ZeroMemory(&userInfo, sizeof(userInfo));
    userInfo.usri4_name = const_cast<LPWSTR>(accountName.c_str());
    userInfo.usri4_password = const_cast<LPWSTR>(password.c_str());
    userInfo.usri4_flags = UF_SCRIPT | UF_PASSWD_CANT_CHANGE | UF_DONT_EXPIRE_PASSWD | UF_NOT_DELEGATED | UF_NORMAL_ACCOUNT;
    userInfo.usri4_acct_expires = TIMEQ_FOREVER;
    userInfo.usri4_primary_group_id = DOMAIN_GROUP_RID_USERS;

    const NET_API_STATUS nStatus = ::NetUserAdd(nullptr, 4 /* USER_INFO level*/, reinterpret_cast<LPBYTE>(&userInfo), nullptr);

    return nStatus;
}

std::wstring NetApiStatusToString(NET_API_STATUS status)
{
    wchar_t* messageBuffer = nullptr;

    DWORD result = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        nullptr, status, 0, reinterpret_cast<LPWSTR>(&messageBuffer), 0, nullptr);

    std::wstring message;

    if (result != 0 && messageBuffer != nullptr) {
        message = messageBuffer;
        message.erase(message.find_last_not_of(L"\r\n") + 1);
        LocalFree(messageBuffer);
    } else {
        message = L"Unknown error code";
    }

    return message;
}

int main() {
    NET_API_STATUS createAccountResult = CreateUserAccount(L"taha", L"passqZ@@w-S9:d80");
    const wstring createAccountResultMessage =  NetApiStatusToString(createAccountResult);

    wcout << createAccountResultMessage << endl;

    HANDLE tokenHandle = nullptr;

    // Open a handle to the current user token (assuming you're logged in and running this with proper privileges).
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY, &tokenHandle)) {
        std::cerr << "Failed to open process token. Error: " << GetLastError() << std::endl;
        return 1;
    }

    PROFILEINFOW profileInfo = { 0 };
    profileInfo.dwSize = sizeof(PROFILEINFOW);
    profileInfo.lpUserName = const_cast<LPWSTR>(L"taha"); // Replace with the actual username

    if (!LoadUserProfileW(tokenHandle, &profileInfo)) {
        std::cerr << "Failed to load user profile. Error: " << GetLastError() << std::endl;
    } else {
        std::cout << "User profile loaded successfully." << std::endl;

        // Unload the profile when done
        if (!UnloadUserProfile(tokenHandle, profileInfo.hProfile)) {
            std::cerr << "Failed to unload user profile. Error: " << GetLastError() << std::endl;
        } else {
            std::cout << "User profile unloaded successfully." << std::endl;
        }
    }

    // Close the token handle
    CloseHandle(tokenHandle);
    return 0;
}