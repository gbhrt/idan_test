#include <stdio.h>
#include <stdlib.h>
#include<iostream>

#include "Header.h"


bool IdanComm::connect()
{
	CHAR *ComPort = "COM11";
	CHAR *szBitrate = "500";
	CHAR *acceptance_code = "1FFFFFFF";
	CHAR *acceptance_mask = "00000039";//"00000039";			
	VOID *flags = CAN_TIMESTAMP_OFF;
	DWORD Mode = Normal;
	char version[10];

	Handle = -1;
	Status = 0;
	SendMSG.Flags = CAN_FLAGS_STANDARD;
	SendMSG.Id = 0x30;//0x31 blink!
	SendMSG.Size = 8;
	Handle = CAN_Open(ComPort, szBitrate, acceptance_code, acceptance_mask, flags, Mode);
	printf("handle= %d\n", Handle);
	if (Handle < 0)
		return true;//error
	memset(version, 0, sizeof(char) * 10);
	Status = CAN_Flush(Handle);
	Status = CAN_Version(Handle, version);
	if (Status == CAN_ERR_OK) {
		printf("Version : %s\n", version);
	}
	return false;


}

bool IdanComm::write(unsigned char mess[])
{
	for (int i = 0; i < 8; i++)
		SendMSG.Data[i] = mess[i];
	Status = CAN_Write(Handle, &SendMSG);
	//CAN_Flush(Handle);
	if (Status == CAN_ERR_OK) {
		//printf("Write Success\n");
		return false;
	}
	std::cout << "Error cannot write to can. status:" << Status << '\n';
	return true;
}
bool IdanComm::read(char mess[])
{
	
	do
	{
		Status = CAN_Read(Handle, &RecvMSG);
		
	}
	while (RecvMSG.Id != 0x39);// Status == -5 || 

	if (Status == CAN_ERR_OK)
	{
		/*printf("Read ID=0x%X, Type=%s, DLC=%d, FrameType=%s, Data=",
			RecvMSG.Id, (RecvMSG.Flags & CAN_FLAGS_STANDARD) ? "STD" : "EXT",
			RecvMSG.Size, (RecvMSG.Flags & CAN_FLAGS_REMOTE) ? "REMOTE" : "DATA");*/
		for (int i = 0; i < RecvMSG.Size; i++)
		{
			//printf("%X,", RecvMSG.Data[i]);
			mess[i] = RecvMSG.Data[i];
		}
		//printf("\n");
		return false;
			
	}
	//printf("Error cannot read from can %d \n", Status);
	return true;
}

bool IdanComm::disconnect()
{
	Status = CAN_Close(Handle);
	if (Status == CAN_ERR_OK)
	{
		printf("close Success\n");
		return false;
	}
	return true;

}
