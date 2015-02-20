// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <libconfig\libconfig.h++>
using namespace std;

#include <g2log.h>
#include <g2logworker.h>

#include "FileLogger.h"

#define Typedef(_T_, _X_)	typedef _T_ _X_
#define SafeDelete(_X_) if(_X_ != nullptr) {delete _X_; _X_ = nullptr;}

#pragma region LOG_OPERATIONS
#define GL_LOG_FILE "OpenglSB6.log"
#include <ctime>
enum class log_level { DBG, WARN, ERR };

static std::fstream g_File;

static bool start_klog() {
	FILE* file = nullptr; errno_t err = fopen_s(&file, GL_LOG_FILE, "w+");
	if (!file) {
		fprintf(stderr, "ERROR: could not open %s log file for writing\n", GL_LOG_FILE);
		return false;
	}
	time_t now = time(NULL);
	char date[64] = { 0 };
	err = ctime_s(date, 64, &now);
	fprintf(file, "%s @local time %s\n", GL_LOG_FILE, date);
	fprintf(stdout, "%s @local time %s\n", GL_LOG_FILE, date);
	fclose(file);
	return true;
}

static bool Klog(std::fstream& file, log_level lvl, const char* function, int line){
	file.open(GL_LOG_FILE, ios::app);
	if (file.fail() || file.bad()) {
		cerr << "ERROR: could not open " << GL_LOG_FILE << "%s for writing" << endl;
		return false;
	}

	switch (lvl)
	{
	case log_level::DBG:
		file << "[DBG] >>";
		break;
	case log_level::ERR:
		file << "[ERR] >>";
		break;
	case log_level::WARN:
		file << "[WRN] >>";
		break;
	default:
		break;
	}
	file << function << "(..):" << line << "\t\t";
#if _DEBUG
	cout << function << "(..):" << line << "\t\t";
#endif
	return true;
}

#ifndef FILELOGGER_HPP
#if !defined(DISABLE_LOG)
#define LOG_D(_X_)  {if(Klog(g_File, log_level::DBG, __FUNCTION__, __LINE__)){g_File << _X_ << endl; g_File.close();} cout << _X_ << endl;}
#define LOG_W(_X_)  {if(Klog(g_File, log_level::WARN, __FUNCTION__, __LINE__)){g_File << _X_ << endl;g_File.close();} cout << _X_ << endl;}
#define LOG_E(_X_)  {if(Klog(g_File, log_level::ERR, __FUNCTION__, __LINE__)){g_File << _X_ << endl; g_File.close();} cerr << _X_ << endl;}
#else
#define LOG_D(_X_)
#define LOG_E(_X_)
#endif
#else

extern ige::FileLogger g_flog_obj;
#define LOG(level) g_flog_obj.getLogger<ige::FileLogger::e_logType::##level>()
#define LOG_D(_X_)  {g_flog_obj.getLogger<ige::FileLogger::e_logType::DBG>() << _X_ << endl;cout << _X_ << endl;}
#define LOG_W(_X_)  {g_flog_obj.getLogger<ige::FileLogger::e_logType::WARN>()<< _X_ << endl;cerr << _X_ << endl;}
#define LOG_E(_X_)  {g_flog_obj.getLogger<ige::FileLogger::e_logType::ERR>() << _X_ << endl;cerr << _X_ << endl;}
#endif
#pragma endregion

#include <vmath.h>