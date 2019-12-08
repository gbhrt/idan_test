 #include<iostream>
#include<vector>

#include<thread>
#include<string>

#include "Header.h"

void manual_commands_loop(Idan & idan)
{
	std::string command = { "s0.0" };
	float steer = 0, acc = 0;
	bool update_loop = true;
	while (update_loop)
	{
		std::cout << "enter command: ";
		std::cin >> command;

		try
		{
			switch (command[0])
			{
			case 's':
				steer = std::stof(command.erase(0, 1));
				std::cout << "steer: " << steer << '\n';
				idan.update_steer(steer);
				break;
			case 'a':
				acc = std::stof(command.erase(0, 1));
				std::cout << "acc: " << acc << '\n';
				idan.update_acc(acc);
				break;
			case 'b':
				std::cout << "neutralize" << '\n';
				idan.neutralize();
				break;
			case 'x':
				update_loop = false;
				break;

			default:
				std::cout << "error command format\n";
				break;
			}

		}
		catch (...)//
		{
			std::cout << "error command\n";
		}

		
	}
	return;

}

bool file_commands_loop(Idan & idan, std::string file_name)
{
	std::ifstream commandFile;
	commandFile.open(file_name);
	if (!commandFile.is_open())
	{
		std::cout << "file error\n";
		return true;
	}

	int dt;
	float steer, acc;
	commandFile >> dt;
	if (commandFile.eof())
	{
		std::cout << "empty file\n";
		return true;
	}

	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<float, std::milli> duration;
	static clock::time_point start = clock::now();
	static clock::time_point t;
	while (true) 
	{
		t = clock::now();
		duration elapsed = t - start;
		if(elapsed.count() >= dt)
		{
			std::cout << "\n time: " << elapsed.count() << '\n';
			start = t;

			commandFile >> steer >> acc;
			if (commandFile.eof())
				break;
			std::cout << "steer: " << steer << " acc: " << acc << '\n';
			idan.update_steer(steer);
			idan.update_acc(acc);
		}
	}
	commandFile.close();
	idan.neutralize();
	return false;

}


 int main(){

	 bool manual_control = false;//control using single commands from keyboard
	//straight_slow.txt
	//straight_fast.txt
	//slalom.txt
	//slalom_low.txt
	 std::string input_file_name = "slalom_low.txt";
	 std::string output_file_name = "slalom_low_out.txt";

	 float acc = -0.3, steer = 0.0;
     Idan idan;
	 if (idan.connect())
	 {
		 std::cout << "cannot connect to idan\n"; 
		 system("pause");
		 return 0;
	 }
	 idan.update_acc(acc);
	 idan.update_steer(steer);
	 std::thread sending_thread = idan.start(output_file_name);
	 
	 
	 if (manual_control)
		 std::cout << "enter commands:\n";
	 else
		 std::cout << "start sending commands from file: " << input_file_name << ".\n";
		 
	 std::cout << "continue? [y/n]" << '\n';
	 std::string command = { "n" };
	 std::cin >> command;
	 if (command[0] == 'y')
		 std::cout << "start!\n";
	 else
	 {
		 system("pause");
		 return 0;
	 }



	 bool update_loop = true;

	 if (manual_control)
	 {
	 	manual_commands_loop(idan);
	 }
	 else
	 {
	 	file_commands_loop(idan, input_file_name);
	 }
		

		 
	 
	
	 idan.end();
	 sending_thread.join();

	 system("pause");
     return 0;
 }
