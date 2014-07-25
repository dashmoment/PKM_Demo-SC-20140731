#include <iostream>
#include <windows.h>
using namespace std;
 

class pipe_sever{

public:
	#define FULL_PIPE_NAME      L"\\\\.\\pipe\\SamplePipe"
	#define PIPE_TIMEOUT_CONNECT	50 
	
	HANDLE pipe;
	BOOL fFinishRead;
	
	int init_pipe();
	int send_msg(char msg[]);
	char* read_msg();
};

int pipe_sever::init_pipe(){

	// Create a pipe to send data
   pipe = CreateNamedPipe(
        FULL_PIPE_NAME, // name of the pipe
        PIPE_ACCESS_DUPLEX, // 1-way pipe -- send only
        PIPE_TYPE_BYTE, // send data as a byte stream
        1, // only allow 1 instance of this pipe
        0, // no outbound buffer
        0, // no inbound buffer
        PIPE_TIMEOUT_CONNECT, // use default wait time
        NULL // use default security attributes
    );


    if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {
        cout << "Failed to create outbound pipe instance.";
        // look up error code here using GetLastError()
        //system("pause");
        return 0;
    }
 
 
    // This call blocks until a client process connects to the pipe
	
		cout << "Waiting for a client to connect to the pipe..." << endl;
		BOOL result = ConnectNamedPipe(pipe, NULL);


		if (!result) {

			cout << "Failed to make connection on named pipe." << endl;
			// look up error code here using GetLastError()
			CloseHandle(pipe); // close the pipe
			//system("pause");
			return 0;
		}

		else{
			return 1;
		}

		
	}
   



int pipe_sever::send_msg(char msg[]){

		
		const int size = MultiByteToWideChar(CP_ACP, 0, msg, -1, NULL, 0);
		wchar_t *pWStr = new wchar_t[size+1];
		MultiByteToWideChar(CP_ACP, 0, msg, -1, pWStr, size);

		wchar_t data[] = L"*******************************************";
		
	
		DWORD numBytesWritten ; 
		DWORD cbResponse = sizeof(data);

		int result = WriteFile(
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

char* pipe_sever::read_msg(){

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