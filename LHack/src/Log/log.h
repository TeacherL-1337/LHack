#pragma once

#include <fstream>
#include <iostream>
#include <format>

enum LogLevel {
	Debug,
	Info,
	Warning,
	Error,
	Critical
};

class Logger
{
public:
	static void Log(const char* log, LogLevel level = Info)
	{
		SYSTEMTIME sys_time;
		GetLocalTime(&sys_time);

		std::ofstream outfile("LHack_log.log", std::ios::app);

		outfile << sys_time.wYear << "/" << sys_time.wMonth << "/" << sys_time.wDay << " " << sys_time.wHour << ":" << sys_time.wMinute << ":" << sys_time.wSecond << " ";

		switch (level)
		{
		case Debug:
			outfile << "[DEBUG]" << log << "\n";
			break;
		case Info:
			outfile << "[INFO]" << log << "\n";
			break;
		case Warning:
			outfile << "[WARNING]" << log << "\n";
			break;
		case Error:
			outfile << "[ERROR]" << log << "\n";
			break;
		case Critical:
			outfile << "[CRITICAL]" << log << "\n";
			throw;
			break;
		default:
			outfile << "[???]" << log << "\n";
			break;
		}
	}
};