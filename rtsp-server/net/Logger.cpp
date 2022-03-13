// PHZ
// 2018-5-15

#if defined(WIN32) || defined(_WIN32) 
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "Logger.h"
#include "Timestamp.h"
#include <stdarg.h>
#include <iostream>

using namespace xop;

const char* Priority_To_String[] =
{
	"DEBUG",
	"CONFIG",
	"INFO",
	"WARNING",
	"ERROR"
};

Logger::Logger() = default;

Logger &Logger::Instance()
{
	static Logger s_logger;
	return s_logger;
}

Logger::~Logger()
= default;

void Logger::Init(char *pathname)
{
	std::unique_lock lock(mutex_);

	if (pathname != nullptr)
	{
		ofs_.open(pathname, std::ios::out | std::ios::binary);
		if (ofs_.fail())
		{
			std::cerr << "Failed to open logfile." << std::endl;
		}
	}
}

void Logger::Exit()
{
	std::unique_lock lock(mutex_);

	if (ofs_.is_open())
	{
		ofs_.close();
	}
}

void Logger::SetWriteCallback(const LogWriteCallbackFun writeCallback)
{
	_writeCallback = writeCallback;
}

void Logger::Log(const Priority priority, const char* __file, const char* __func, const int __line, const char *fmt, ...)
{	
	std::unique_lock lock(mutex_);

	char buf[2048] = {0};
	sprintf(buf, "[%s][%s:%s:%d] ", Priority_To_String[priority],  __file, __func, __line);
	va_list args;
	va_start(args, fmt);
	vsprintf(buf + strlen(buf), fmt, args);
	va_end(args);

	this->Write(std::string(buf));
	_writeCallback(priority, std::string(buf));
}

void Logger::Log2(const Priority priority, const char *fmt, ...)
{
	std::unique_lock lock(mutex_);

	char buf[4096] = { 0 };
	sprintf(buf, "[%s] ", Priority_To_String[priority]);  
	va_list args;
	va_start(args, fmt);
	vsprintf(buf + strlen(buf), fmt, args);
	va_end(args);

	this->Write(std::string(buf));
	_writeCallback(priority, std::string(buf));
}

void Logger::Write(const std::string &info)
{
	if (ofs_.is_open())
	{
		ofs_ << "[" << Timestamp::Localtime() << "]"
			<< info << std::endl;
	}
}
