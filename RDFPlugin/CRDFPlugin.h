#include <EuroScopePlugIn.h>
#include <string>
#include <chrono>
#include <mutex>
#include <queue>
#include "HiddenWindow.h"

#pragma once

using namespace std;
using namespace EuroScopePlugIn;

const string MY_PLUGIN_NAME = "RDF Plugin for Euroscope";
const string MY_PLUGIN_VERSION = "1.2.0";
const string MY_PLUGIN_DEVELOPER = "Claus Hemberg Joergensen";
const string MY_PLUGIN_COPYRIGHT = "Free to be distributed as source code";

class CRDFPlugin: public EuroScopePlugIn::CPlugIn
{
private:
	int rxcount = 0;
	string activeTransmittingPilot;
	string previousActiveTransmittingPilot;
	std::chrono::steady_clock::time_point lastOnGetTagItemTime = std::chrono::steady_clock::now();
	
	void ProcessMessage(std::string message);

	HWND hiddenWindow = NULL;

	// Lock for the message queue
	std::mutex messageLock;

	// Internal message quque
	std::queue<std::string> messages;


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
	void AddMessageToQueue(std::string message);

	virtual CRadarScreen *OnRadarScreenCreated(const char * sDisplayName, bool NeedRadarContent, bool GeoReferenced, bool CanBeSaved, bool CanBeCreated);

	inline string GetActiveTransmittingPilot()
	{
		return activeTransmittingPilot;
	}

	inline string GetPreviousActiveTransmittingPilot()
	{
		return previousActiveTransmittingPilot;
	}
};

