#include "WebGrabber.h"

const std::string userAgentHeaderName = L"User-Agent: ";

void print_usage()
{
    std::cout << "Usage: WebGrabber.exe <User-Agent> <Address>" << std::endl;;
}

int _tmain(int argc, _TCHAR* argv[])
{
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer;
    BOOL bHeadersSet = FALSE;
    BOOL bResults = FALSE;
    HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;

    if (argc != 3)
    {
        std::cout << "Incorrect number of arguments." << std::endl;
        print_usage();
        return 1;
    }

    std::string userAgentValue = argv[1];
    auto addressToGrab = argv[2];

    std::string userAgentHeader = userAgentHeaderName + userAgentValue;

    std::cout << "HTTP GET " << addressToGrab << std::endl;
    std::cout << userAgentHeader << std::endl;

    // Use WinHttpOpen to obtain a session handle.
    hSession = WinHttpOpen(L"WebGrabber",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    // Specify an HTTP server.
    if (hSession)
    {
        hConnect = WinHttpConnect(hSession, addressToGrab,
            INTERNET_DEFAULT_HTTPS_PORT, 0);
    }
    else
    {
        std::cerr << "Failed to create a session." << std::endl;
    }

    // Create an HTTP request handle.
    if (hConnect)
    {
        hRequest = WinHttpOpenRequest(hConnect, L"GET", NULL,
            NULL, WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE);
    }
    else
    {
        std::cerr << "Failed to create a connection to \"" << addressToGrab << "\"."  << std::endl;
    }

    // Send a request.
    if (hRequest)
    {
        // Add the UA header.
        bHeadersSet  = WinHttpAddRequestHeaders(hRequest,
            userAgentHeader.c_str(),
            (ULONG)-1L,
            WINHTTP_ADDREQ_FLAG_ADD);
    }
    else
    {
        std::cerr << "Failed to set the User Agent String." << std::endl;
    }

    if (bHeadersSet)
    {
        bResults = WinHttpSendRequest(hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS, 0,
            WINHTTP_NO_REQUEST_DATA, 0,
            0, 0);
    }
    else
    {
        std::cerr << "Failed to send the request." << std::endl;
    }

    // End the request.
    if (bResults)
    {
        bResults = WinHttpReceiveResponse(hRequest, NULL);
    }
    else
    {
        std::cerr << "Request failed." << std::endl;
    }

    // Keep checking for data until there is nothing left.
    if (bResults)
    {
        do
        {
            // Check for available data.
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
            {
                std::cerr << "Error " << GetLastError() << " in WinHttpQueryDataAvailable." << std::endl;
            }
            // Allocate space for the buffer.
            pszOutBuffer = new char[dwSize + 1];
            if (!pszOutBuffer)
            {
                std::cerr << "Out of memory." << std::endl;
                dwSize = 0;
            }
            else
            {
                // Read the data.
                ZeroMemory(pszOutBuffer, dwSize + 1);

                if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
                    dwSize, &dwDownloaded))
                {
                    std::cerr << "Error " << GetLastError() << " in WinHttpReadData." << std::endl;
                }
                else
                {
                    printf("%s", pszOutBuffer);
                }
                // Free the memory allocated to the buffer.
                delete[] pszOutBuffer;
            }
        } while (dwSize > 0);
    }


    // Report any errors.
    if (!bResults)
    {
        std::cerr << "Error " << GetLastError() << " occurred." << std::endl;
    }

    // Close any open handles.
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);

    return !bResults;
}