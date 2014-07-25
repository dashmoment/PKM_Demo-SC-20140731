#include <iostream>
#include <windows.h>
using namespace std;

class pipe_client{

public:
	#define FULL_PIPE_NAME      L"\\\\.\\pipe\\SamplePipe"
	
	BOOL fFinishRead;
	HANDLE pipe;

	int client_init();
	char* read_msg();
	int send_msg(char msg[]);
};


int pipe_client::client_init(){

	wcout << "Connecting to pipe..." << endl;
 
    // Open the named pipe
    // Most of these parameters aren't very relevant for pipes.
    pipe = CreateFile(
        FULL_PIPE_NAME,
        GENERIC_READ | GENERIC_WRITE, // only need read access
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
 
    if (pipe == INVALID_HANDLE_VALUE) {
        wcout << "Failed to connect to pipe." << endl;
        // look up error code here using GetLastError()
        system("pause");
        return 1;
    }
 
    wcout << "Reading data from pipe..." << endl;

}

char* pipe_client::read_msg(){

		wchar_t chResponse[128];
        DWORD cbResponse, cbRead;
        cbResponse = sizeof(chResponse);

        fFinishRead = ReadFile(
            pipe,                  // Handle of the pipe
            chResponse,             // Buffer to receive the reply
            cbResponse,             // Size of buffer in bytes
            &cbRead,                // Number of bytes read 
            NULL                    // Not overlapped 
            );

	if (!fFinishRead){   
			
            wprintf(L"ReadFile from pipe failed \n");
			 CloseHandle(pipe);	
			 system("pause");
			
            return NULL;
    }

	else{

		char *data_c;
			int utf8Size = WideCharToMultiByte(CP_UTF8, 0, chResponse, -1, NULL, 0, NULL, false);
			data_c =  new char[utf8Size + 1];
			WideCharToMultiByte(CP_UTF8, 0, chResponse, -1, data_c, utf8Size, NULL, false);
			cout<<"Receive bytes from server:"<<data_c<<endl;
		return data_c;
	}
}


int pipe_client::send_msg(char msg[]){

		const int size = MultiByteToWideChar(CP_ACP, 0, msg, -1, NULL, 0);
		wchar_t *pWStr = new wchar_t[size+1];
		MultiByteToWideChar(CP_ACP, 0, msg, -1, pWStr, size);

		wchar_t data[] = L"*******************************************";
		
		
		DWORD numBytesWritten ;
		DWORD cbResponse = sizeof(data);

		BOOL result = WriteFile(
		    pipe, // handle to our outbound pipe
			pWStr, // data to send
			cbResponse, // length of data to send (bytes)
			&numBytesWritten, // will store actual amount of data sent
			NULL // not using overlapped IO
		 );

		if (result) {

			wcout << "Number of bytes sent: " << numBytesWritten << endl;
			return 1;
		} 

		else {
			wcout << "Failed to send data." << endl;
			return 0;
       
		 }



}