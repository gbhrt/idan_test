#include <windows.h>
#include<iostream>
#include "Header.h"


int open_port(HANDLE & hFile, const char port_name[])
{
	COMMTIMEOUTS comTimeOut;  // moved from inside the function to the top.  otherwise gives an error 15-10-2015 shiller

	int a;

	a = 0;

	//	open port
	hFile = CreateFile(port_name,  // 
		GENERIC_READ | GENERIC_WRITE,       // Specify mode that open device.
		0,                                  // the devide isn't shared.
		NULL,                               // the object gets a default security.
		OPEN_EXISTING,                      // Specify which action to take on file. 
		FILE_ATTRIBUTE_NORMAL,              // default. // 0
		NULL);                              // default.


											// Assign user parameter.
											//   config_.BaudRate = 9600;  //dcb_imu.BaudRate;    // Specify buad rate of communicaiton.
											//   config_.StopBits = dcb_imu.StopBits;    // Specify stopbit of communication.
											//   config_.Parity = dcb_imu.Parity;        // Specify parity of communication.
											//   config_.ByteSize = dcb_imu.ByteSize;    // Specify  byte of size of communication.

											//					GetCommState(handlePort_,&config_)					 
											//					 SetCommState(handlePort_,&config_)


											//*********************

											//   TCHAR *pcCommPort = TEXT("\\\\.\\COM2");

											//   hCom = CreateFile( pcCommPort,
											//                    GENERIC_READ | GENERIC_WRITE,
											//                   0,    // must be opened with exclusive-access
											//                    NULL, // default security attributes
											//                    OPEN_EXISTING, // must use OPEN_EXISTING
											//                    0,    // not overlapped I/O
											//                    NULL  // hTemplate must be NULL for comm devices
											//                    );

	if (hFile == INVALID_HANDLE_VALUE || hFile == 0)
	{
		// Handle the error.
		std::cout<<"CreateFile failed with error"<< GetLastError()<< '\n';
		return (1);
	}

	//initilize all members of the structure to 0
	DCB dcb_idan;
	ZeroMemory(&dcb_idan, sizeof(dcb_idan));

	//  SecureZeroMemory(&dcb_imu, sizeof(DCB));
	dcb_idan.DCBlength = sizeof(DCB);
	bool success = GetCommState(hFile, &dcb_idan);

	if (!success)
	{
		// Handle the error.
		printf("GetCommState failed with error %d.\n", GetLastError());
		return (2);
	}
	// Fill in DCB: 57,600 bps, 8 data bits, no parity, and 1 stop bit.

	dcb_idan.BaudRate = 500000;     // set the baud rate
									   //dcb_imu.BaudRate = 9600;
	dcb_idan.ByteSize = 8;             // data size, xmit, and rcv
	dcb_idan.Parity = NOPARITY;        // no parity bit
	dcb_idan.StopBits = ONESTOPBIT;    // one stop bit



									  // instance an object of COMMTIMEOUTS.
									  ////COMMTIMEOUTS comTimeOut;
									  // Specify time-out between charactor for receiving.
	comTimeOut.ReadIntervalTimeout = 10;//10
	// Specify value that is multiplied 
	// by the requested number of bytes to be read. 
	comTimeOut.ReadTotalTimeoutMultiplier = 0;//0
	// Specify value is added to the product of the 
	// ReadTotalTimeoutMultiplier member
	comTimeOut.ReadTotalTimeoutConstant = 100;//100
	// Specify value that is multiplied 
	// by the requested number of bytes to be sent. 
	comTimeOut.WriteTotalTimeoutMultiplier = 0;
	// Specify value is added to the product of the 
	// WriteTotalTimeoutMultiplier member
	comTimeOut.WriteTotalTimeoutConstant = 100;
	// set the time-out parameter into device control.
	SetCommTimeouts(hFile, &comTimeOut);

	success = SetCommState(hFile, &dcb_idan);

	if (!success)
	{
		// Handle the error.
		printf("SetCommState failed with error %d.\n", GetLastError());
		return 3;
	}

	//  _tprintf (TEXT("Serial port %s successfully reconfigured.\n"), pcCommPort);
	printf("Serial port open.\n");
	return 0;
}
static void close_port(HANDLE hFile)
{
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
}

static int send_to_port(HANDLE hFile, unsigned char* data, int size)//const char
{
	DWORD n;
	WriteFile(hFile, data, size, &n, NULL);
	return n;
}

int receive_from_port(HANDLE & hFile, unsigned char* data)
{
	DWORD *myBytesRead = new DWORD;
	if (!ReadFile(hFile, data, 50, myBytesRead, NULL))
	{
		fprintf(stderr, "Error\n");
		CloseHandle(hFile);
		return 1;
	}
	//for (int i = 0; i < *myBytesRead; i++)
	//	std::cout << data[i] << "	";
	//std::cout << '\n';
	//if (*myBytesRead != 8)
	//{
	//	std::cout << "error: myBytesRead: " << *myBytesRead << '\n';
	//	return 1;
	//}


	return 0;
}


IdanComm::IdanComm()
{

}
IdanComm::~IdanComm()
{

}

int IdanComm::connect()
{
	const char port_name[] = "COM5";
	int error = open_port(hFile, port_name);
	return error;
}
void IdanComm::disconnect()
{
	close_port(hFile);
}
void IdanComm::receive(unsigned char idan_rData[8])
{
	receive_from_port(hFile, idan_rData);
	//for (int i = 0; i < 8; i++)
	//	std::cout << idan_rData[i] << "	";
	//std::cout << '\n';
}
void IdanComm::send(unsigned char idan_sData[8])
{
	send_to_port(hFile, idan_sData, 8);
}
