#include <EuroScopePlugIn.h>
#include <string>
#include <chrono>
#include <mutex>
#include <queue>
#include <set>
#include "HiddenWindow.h"

#pragma once

using namespace std;
using namespace EuroScopePlugIn;

const string MY_PLUGIN_NAME = "RDF Plugin for Euroscope";
const string MY_PLUGIN_VERSION = "1.2.2";
const string MY_PLUGIN_DEVELOPER = "Claus Hemberg Joergensen";
const string MY_PLUGIN_COPYRIGHT = "Free to be distributed as source code";

class CRDFPlugin: public EuroScopePlugIn::CPlugIn
{
private:
	int rxcount = 0;
	set<string> activeTransmittingPilots;
	set<string> previousActiveTransmittingPilots;
	chrono::steady_clock::time_point lastOnGetTagItemTime = std::chrono::steady_clock::now();
	
	void ProcessMessage(std::string message);

	set<string> SplitString(string str);

	HWND hiddenWindow = NULL;

	// Lock for the message queue
	mutex messageLock;

	// Internal message quque
	queue<string> messages;


	// Class for our window
	WNDCLASS windowClass = {
	   NULL,
	   HiddenWindow,
	   NULL,
	   NULL,
	   GetModuleHandle(NULL),
	   NULL,
	   NULL,
	   NULL,
	   NULL,
	   "RDFHiddenWindowClass"
	};


public:
	CRDFPlugin();
	virtual ~CRDFPlugin();

	void OnTimer(int counter) override;
	void AddMessageToQueue(string message);

	COLORREF GetRGB(const char* settingValue);

	virtual CRadarScreen *OnRadarScreenCreated(const char * sDisplayName, bool NeedRadarContent, bool GeoReferenced, bool CanBeSaved, bool CanBeCreated);

	inline set<string>& GetActiveTransmittingPilots()
	{
		return activeTransmittingPilots;
	}

	inline set<string>& GetPreviousActiveTransmittingPilots()
	{
		return previousActiveTransmittingPilots;
	}
};

