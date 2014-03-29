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
using namespace std;

#pragma region LOG_OPERATIONS
#include <time.h>
#define GL_LOG_FILE "OpenglSB6.log"


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

static bool Klog(std::fstream& file, const char* function, int line){

	file.open(GL_LOG_FILE, ios::app);
	if (file.fail() || file.bad()) {
		cerr << "ERROR: could not open " << GL_LOG_FILE << "%s for writing" << endl;
		return false;
	}

	file << ">>" << function << "(..)" << ":" << line << "\t\t";
#if _DEBUG
	cout << ">>" << function << "(..)" << ":" << line << "\t\t";
#endif
	return true;
}

#if _DEBUG
#define FILE_LOG(_X_)  {std::fstream file; Klog(file, __FUNCTION__, __LINE__); file << _X_ << endl;cout << _X_ << endl; file.close();}
#else
#define FILE_LOG(_X_)  {std::fstream file; Klog(file, __FUNCTION__, __LINE__); file << _X_ << endl;file.close();}
#endif
// TODO: reference additional headers your program requires here
#pragma endregion