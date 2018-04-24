#pragma once
//--------------------------------------------------------------------------------------
// File: Log.h
// Project: Function library
//
// Author Mattias Fredriksson 2017.
//--------------------------------------------------------------------------------------

#include<string>
#include<sstream>
#include<fstream>
#include<stdexcpt.h>
#include<assert.h>

class Log {

private:
	/*File name for the log file*/
	std::string file;
	/*If date should be logged with each message*/
	bool logDate;
	/*If log is intialized*/
	bool initialized;

	inline void isInit() {
		assert(initialized);
	}

public:

	/*Gets the current date and time as a string*/
	static std::string getDate();
	/*An uninitialized log file which when initialized can print log messages*/
	Log();
	/*
	Creates a log file which can print log messages.
	logFile			>> File name and/or directory for the log file
	clearLogFile	>> If logfile should be cleared
	logDate			>> Add date/time to each log message
	initMsg			>> Initialization message
	*/
	Log(const std::string &logFile, bool clearLogFile = false, bool logDate = true, bool initMsg = true);

	/*
	Initialize a log file object which can print log messages.
	logFile			>> File name and/or directory for the log file
	clearLogFile	>> If logfile should be cleared
	logDate			>> Add date/time to each log message
	initMsg			>> Initialization message
	*/
	void initLog(const std::string &logFile, bool clearLogFile = false, bool logDate = true, bool initMsg = true);
	virtual ~Log();

	/*Log a message*/
	void logMsg(const std::string &msg);
	void logMsg(const std::stringstream& stream);
	/*Clear the entire log*/
	void clearLog();
	/*Enables or Disable log date*/
	void setDate(bool logDate);

	std::ofstream get();

	std::ofstream operator <<(const char* str);
	std::ofstream operator <<(float value);
	std::ofstream operator <<(double value);
	std::ofstream operator <<(int value);
	std::ofstream operator <<(unsigned int value);
};

static Log& log()
{
	static Log log = Log("Log.txt", false, true, false);
	return log;
}


//#define LOG_IMPLEMENTATION //Visibility toggle in VS
#ifdef LOG_IMPLEMENTATION
//--------------------------------------------------------------------------------------
// File: Log.h
// Project: Function library
//
// Author Mattias Fredriksson 2017.
//--------------------------------------------------------------------------------------

#include"Log.h"
#include<chrono>
#include<iomanip>
#include<time.h>

Log::Log() :
	file(), logDate(), initialized(false) {
}
Log::Log(const std::string &logFile, bool clearLogFile, bool logDate, bool initMsg) :
	initialized(false) {
	initLog(logFile, clearLogFile, logDate, initMsg);
}

void Log::initLog(const std::string &logFile, bool clearLogFile, bool logDate, bool initMsg) {
	this->file = logFile;
	this->logDate = logDate;
	this->initialized = true;
	if (clearLogFile)
		clearLog();
	if (initMsg)
		logMsg("Log initialized " + (logDate ? "" : getDate()));
}

Log::~Log() {}

void Log::logMsg(const std::string &msg) {
	isInit();
	std::ofstream log_file(file, std::ios_base::out | std::ios_base::app);

	//Print log msg
	if (logDate)
		log_file << getDate() << ' ';
	log_file << msg << std::endl;
}

void Log::logMsg(const std::stringstream &msg)
{
	isInit();
	std::ofstream log_file(file, std::ios_base::out | std::ios_base::app);

	//Print log msg
	if (logDate)
		log_file << getDate() << ' ';
	log_file << msg.rdbuf() << std::endl;
}


std::ofstream Log::get() {
	isInit();
	return std::ofstream(file, std::ios_base::out | std::ios_base::app);
}

std::ofstream Log::operator <<(const char* str) {
	std::ofstream log_file = get();
	log_file << str;
	return log_file;
}
std::ofstream Log::operator <<(float value) {
	isInit();
	std::ofstream log_file = get();
	log_file << value;
	return log_file;
}
std::ofstream Log::operator <<(double value) {
	isInit();
	std::ofstream log_file = get();
	log_file << value;
	return log_file;
}
std::ofstream Log::operator <<(int value) {
	isInit();
	std::ofstream log_file = get();
	log_file << value;
	return log_file;
}
std::ofstream Log::operator <<(unsigned int value) {
	isInit();
	std::ofstream log_file = get();
	log_file << value;
	return log_file;
}

void Log::clearLog() {
	isInit();
	std::ofstream log_file(file, std::ios_base::out | std::ios_base::trunc);
}

std::string Log::getDate() {

	const int parseSize = 20;
	char tArr[parseSize + 2];
	//Get the date & time
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	tm t;
	//Parse it to string
	localtime_s(&t, &in_time_t);
	std::strftime(&tArr[1], 20, "%Y-%m-%d %H:%M:%S", &t);

	//Add some clamps to the string
	tArr[0] = '<'; tArr[parseSize] = '>'; tArr[parseSize + 1] = '\0';

	return std::string(tArr);
}

void Log::setDate(bool logDate) {
	this->logDate = logDate;
}


#endif