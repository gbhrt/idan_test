#include<iostream>
#include <chrono>
#include<thread>
#include"Header.h"

void sendingData::reset()
{
	acc = 0;
	steer = 0;
	neutralize = false;
}
void receivingData::reset()
{
	acc = 0;
	steer = 0;
	steering_mode = 13;
	steering_error = 255;
	acc_mode = 13;
	acc_error = 255;
}

convertIdanData::convertIdanData()
{
	steer_max = 3700;
	steer_min = 300;
	steer_middle = 2000;
	acc_max = 3700;
	acc_min = 300;
	acc_middle = 2000;
}
convertIdanData::~convertIdanData()
{
}
void convertIdanData::convert_to_idan(sendingData * sData, unsigned char idan_sData[8])
{
	int i_steer = (sData->steer*(steer_max - steer_min) + steer_min + steer_max) / 2;
	if (i_steer > steer_max)
	{
		std::cout << "i_steer > steer_max\n";
		i_steer = steer_max;
	}
	if (i_steer < steer_min)
	{
		std::cout << "i_steer < steer_min\n";
		i_steer = steer_min;
	}
	//std::cout << "send - steer: " << sData->steer << " raw: " << i_steer << '\n';

	idan_sData[0] = i_steer >> 8;
	idan_sData[1] = i_steer;
	int i_acc = (sData->acc *(acc_max - acc_min) + acc_min + acc_max) / 2;
	if (i_acc > acc_max)
	{
		std::cout << "i_acc > acc_max\n";
		i_acc = steer_max;
	}	
	if (i_acc < acc_min)
	{
		std::cout << "i_acc < acc_min\n";
		i_steer = steer_min;
	}
	//std::cout << "send - acc: " << sData->acc << " raw: " << i_acc << '\n';
	//i_acc = 400;
	idan_sData[2] = i_acc >> 8;
	idan_sData[3] = i_acc;
	if (sData->neutralize)
		idan_sData[7] = 0x5A;

	/*for (int i = 0; i < 8; i++)
		printf("%X,", idan_sData[i]);
	printf("\n");*/
	return;

}
void convertIdanData::convert_from_idan(char idan_rData[8], receivingData * rData)
{

	rData->steering_mode = unsigned char(idan_rData[0]);
	rData->steering_error = unsigned char(idan_rData[1]);
	//unsigned char tmp = 0b00001111;
	//std::cout << "rData->steering_mode: " << rData->steering_mode << '\n';
	if (rData->steering_mode == 5)
	{
		//idan_rData[2] = tmp & unsigned char(idan_rData[2]);
		int i_steer = unsigned char(idan_rData[2]) << 8 | unsigned char(idan_rData[3]);
		
		if (i_steer<steer_min || i_steer> steer_max)
			std::cout << "steering out of range\n";
		else
			rData->steer = float(2 * i_steer - steer_max - steer_min) / (steer_max - steer_min);
		//std::cout << "read - steer: " << rData->steer << " raw: " << i_steer << '\n';
	}
	rData->acc_mode = unsigned char(idan_rData[4]);
	rData->acc_error = unsigned char(idan_rData[5]);
	if (rData->acc_mode == 5)
	{
		//idan_rData[6] = tmp & unsigned char(idan_rData[6]);
		int i_acc = unsigned char(idan_rData[6]) << 8 | unsigned char(idan_rData[7]);
		
		if (i_acc<acc_min || i_acc> acc_max)
			std::cout << "throttle/brake out of range\n";
		else
			rData->acc = float(2 * i_acc - acc_max - acc_min) / (acc_max - acc_min);
		//std::cout << "read - acc: " << rData->acc << " raw: " << i_acc << '\n';
	}

}

Idan::Idan()
{
	send_active = new bool;
	done = new bool;
	sData.reset();
}
Idan::~Idan()
{
	sData.reset();
}

int Idan::connect()
{
	if (comm.connect())//connect to idan
		return 1;
	else
		return 0;
	
}

void continues_communication(bool * comm_active, bool * done, sendingData * sData, receivingData * rData, IdanComm comm,std::string file_name)//, 
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<float, std::milli> duration;
	static clock::time_point start = clock::now();
	unsigned char  idan_sData[8] = { 0 };
	char  idan_rData[8] = { 0 };
	convertIdanData Conv;
	std::ofstream outfile;
	outfile.open(file_name);
	outfile << "acc" << '\t' << "acc_mode" << '\t' << "acc_error" << '\t' << "steer" << '\t' << "steering_mode" << '\t' << "steering_error" << std::endl;

	while (*comm_active)
	{
		duration elapsed = clock::now() - start;
		std::chrono::high_resolution_clock::time_point time_stamp = std::chrono::high_resolution_clock::now();
		//std::cout << "t: " << elapsed.count() << '\t';
		if (elapsed.count() >= 10)
		{
			//std::cout << "\n time: " << elapsed.count() << '\n';
			
			int read_failed = comm.read(idan_rData);
			if (!read_failed)
			{
				Conv.convert_from_idan(idan_rData, rData);
			/*	std::cout << "idan - steer: " << rData->steer << " acc: " << rData->acc << '\n';
				std::cout << " steerMode: " << rData->steering_mode << " accMode: " << rData->acc_mode << '\n';
				std::cout << " steering_error: " << rData->steering_error << " acc_error: " << rData->acc_error << '\n';*/
				outfile << rData->acc << '\t' << rData->acc_mode << '\t' << rData->acc_error << '\t' << rData->steer << '\t' << rData->steering_mode << '\t' << rData->steering_error << std::endl;
			}
			Conv.convert_to_idan(sData, idan_sData);
			//elapsed = clock::now() - start;
			//std::cout << "time after convert: " << elapsed.count() << '\n';

			comm.write(idan_sData);
			start = time_stamp;
			
		}
	}

	outfile.close();
	*done = true;
}

std::thread Idan::start(std::string file_name)//start sending commands to idan at 50Hz
{
	*send_active = true;
	*done = false;
	std::thread sending_thread(continues_communication, send_active, done, &sData, &rData, comm, file_name);//
	//sending_thread = new thread(continues_sending, send_active, &sData);
	return sending_thread;
}
void Idan::end()
{
	*send_active = false;
	//std::chrono::this_thread::sleep_for(500ms);
	//while (!*done);
	comm.disconnect();
	
}


void Idan::update_steer(float steer)
{
	mtx.lock();//lock
	sData.steer = steer;
	mtx.unlock();//end lock
}
void Idan::update_acc(float acc)
{
	mtx.lock();//lock
	sData.acc = acc;
	mtx.unlock();//end lock
}
void Idan::update(float acc, float steer)
{
	mtx.lock();//lock
	sData.acc = acc;
	sData.steer = steer;
	mtx.unlock();//end lock
}
void Idan::neutralize()
{
	mtx.lock();//lock
	sData.neutralize = true;
	mtx.unlock();//end lock
}
