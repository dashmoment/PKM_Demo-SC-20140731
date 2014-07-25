#include <sys/types.h>
#include <Windows.h>
#include <activeds.h>
#include <yvals.h>
#include <stdio.h>
#include <winbase.h>
#include <iostream>
#include <string> 
#include <stdio.h>

using namespace std;
COMSTAT ComStat;
char* IntC ;
DWORD rlength,tread, errFlag;
string IntS;

class RS232_COM{

public:

	#define COM_NAME L"COM2"
	HANDLE pCOM;

	int init_port();
	int write_port(string command);


};

int RS232_COM::init_port(){

	DCB dcb;
	BOOL fSuccess;
	char RecData[10]={0};
	DWORD lpNumberOfBytesWritten = 0; 
	//LPCWSTR com_name = (LPCWSTR)"COM2";

	pCOM = CreateFile(COM_NAME,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,0,0);
	
	if (pCOM == INVALID_HANDLE_VALUE) {
		printf ("CreateFile failed.\n");
	}
	else{
		printf("Open %s succeed.\n","COM1"); 
	}

	fSuccess = GetCommState(pCOM, &dcb);
	if (!fSuccess) {
		printf ("GetCommState failed.\n");
	}
	else{
		printf ("GetCommState succeed.\n");
	}

	dcb.BaudRate =38400;
	dcb.ByteSize=8;
	dcb.Parity = NOPARITY;   // 同位元 (0-4) = 無，奇同位，偶同位，mark，space
	dcb.StopBits = ONESTOPBIT; // 停止位元 (0，1，2) = 1， 1。5， 2
	dcb.fOutX = FALSE;           // XON/XOFF out flow control
	dcb.fInX = FALSE;            // XON/XOFF in flow control
	dcb.fOutxCtsFlow = FALSE;    // CTS output flow control
	dcb.fOutxDsrFlow = FALSE;    // DSR output flow control
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity = FALSE; // DSR sensitivity
	dcb.fTXContinueOnXoff = FALSE;// XOFF continues Tx
	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fAbortOnError = FALSE;   // abort reads/writes on error
	dcb.fDummy2 = 0;
	dcb.wReserved = 0;
	dcb.XonLim = 0;
	dcb.XoffLim = 0;
	dcb.XonChar = 0;
	dcb.XoffChar = 0;
	dcb.ErrorChar = 0;
	dcb.EofChar = 0;
	dcb.EvtChar = 0;
	dcb.wReserved1 =0;


	fSuccess = SetCommState (pCOM, &dcb); 
	if (!fSuccess){
		printf ("SetCommState failed.\n");
		return 0;
	}
	else{
		printf ("SetCommState succeed.\n");
		return 1;
	}
}


int RS232_COM::write_port(string command){

	command=command+"\r";

	cout<<command<<endl;

	const char* lpBufferToWrite = command.c_str();
	unsigned int nNumOfBytesToWrite = command.size();
	unsigned long nNumOfBytesWritten;

	WriteFile(pCOM, lpBufferToWrite, nNumOfBytesToWrite, &nNumOfBytesWritten, NULL);

	return 1;
}