#include<iostream>
#include<Windows.h>
#include<TlHelp32.h>
HANDLE systemToken()
{
    PROCESSENTRY32 proc;
    proc.dwSize = sizeof(PROCESSENTRY32);
    HANDLE SysToken{};
    HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Snap == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Invalid handle value. Error Code: " << GetLastError() << std::endl;

        return INVALID_HANDLE_VALUE;
    }

    while (Process32Next(Snap, &proc))
    {
        HANDLE openProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc.th32ProcessID);

        if (openProc == INVALID_HANDLE_VALUE)
        {
            continue;
        }

        HANDLE token{};
        if (!OpenProcessToken(openProc, MAXIMUM_ALLOWED, &token)) 
        {
       
            CloseHandle(openProc);
            continue;
        }

        if (ImpersonateLoggedOnUser(token))
        {
            DWORD size = 256; 
            WCHAR userName[256];

            if (GetUserName(userName, &size))
            {
               if(wcscmp(userName,L"SYSTEM")==0)
               {
                   std::wcout << userName << std::endl;

                   RevertToSelf();
                   CloseHandle(openProc); 
                   CloseHandle(token);

                   return token;
               }
            }
       

            RevertToSelf();
        }
      

        CloseHandle(openProc);
        CloseHandle(token);
    }

    CloseHandle(Snap);
    return INVALID_HANDLE_VALUE;
}

int main()
{

    HANDLE sysToken = systemToken();

    if (sysToken == INVALID_HANDLE_VALUE)
    {
 
      std::cerr << "Failed to obtain system token. Error Code: " << GetLastError << std::endl;

      return -1;
    }
    

    std::wcout << L"Successfully obtained system token." << std::endl;

    system("pause");
 
}
