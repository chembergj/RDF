#include "stdafx.h"
#include "CRDFPlugin.h"
#include "CRDFScreen.h"
#include <stdexcept>

#pragma comment(lib, "wininet.lib")

using namespace std;

const   int     TAG_ITEM_VOICERECEIVE = 1;

CRDFPlugin::CRDFPlugin()
	: EuroScopePlugIn::CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE,
		MY_PLUGIN_NAME.c_str(),
		MY_PLUGIN_VERSION.c_str(),
		MY_PLUGIN_DEVELOPER.c_str(),
		MY_PLUGIN_COPYRIGHT.c_str())
{
	DisplayUserMessage("Message", "RDF", std::string("Version " + MY_PLUGIN_VERSION + " loaded").c_str(), false, false, false, false, false);

	RegisterClass(&this->windowClass);

	this->hiddenWindow = CreateWindow(
		"RDFHiddenWindowClass",
		"RDFHiddenWindow",
		NULL,
		0,
		0,
		0,
		0,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		reinterpret_cast<LPVOID>(this)
	);

	if (GetLastError() != S_OK) {
		DisplayUserMessage(
			"RDF",
			"RDF",
			"Unable to open communications for RDF plugin",
			true,
			true,
			true,
			true,
			true
		);
	}

}


CRDFPlugin::~CRDFPlugin()
{
	if (this->hiddenWindow != NULL) {
		DestroyWindow(this->hiddenWindow);
	}
}

/*
	Process the message queue
*/
void CRDFPlugin::OnTimer(int counter)
{
	std::lock_guard<std::mutex> lock(this->messageLock);

	// Process any incoming messages from the standalone client
	while (this->messages.size() != 0) {
		this->ProcessMessage(this->messages.front());
		this->messages.pop();
	}
}

void CRDFPlugin::AddMessageToQueue(std::string message)
{
	std::lock_guard<std::mutex> lock(this->messageLock);
	this->messages.push(message);
}

void CRDFPlugin::ProcessMessage(std::string message)
{
	if (message.empty()) {

		// Marks end of transmission
		if (!activeTransmittingPilot.empty())
		{
			previousActiveTransmittingPilot = activeTransmittingPilot;
#ifdef _DEBUG
			DisplayUserMessage("Message", "RDF Plugin", (std::string("End of transmission for  ") + previousActiveTransmittingPilot).c_str(), false, false, false, false, false);
#endif
		}
	}
	activeTransmittingPilot = message;

	// TODO: extract callsign list if more than one pilot transmitting
}

CRadarScreen * CRDFPlugin::OnRadarScreenCreated(const char * sDisplayName,
	bool NeedRadarContent,
	bool GeoReferenced,
	bool CanBeSaved,
	bool CanBeCreated)
{
	DisplayUserMessage("Message", "RDF Plugin", (std::string("Radio Direction Finder plugin activated on ") + sDisplayName).c_str(), false, false, false, false, false);
	
	COLORREF rdfRGB = RGB(255, 255, 255);	// Default: white

	try
	{
		const char* cstrRGB = GetDataFromSettings("RGB");
		if (cstrRGB != NULL)
		{
			string circleRGB = cstrRGB;

			size_t firstColonIndex = circleRGB.find(':');
			if (firstColonIndex != string::npos)
			{
				size_t secondColonIndex = circleRGB.find(':', firstColonIndex + 1);
				if (secondColonIndex != string::npos)
				{
					string redString = circleRGB.substr(0, firstColonIndex);
					string greenString = circleRGB.substr(firstColonIndex + 1, secondColonIndex - firstColonIndex - 1);
					string blueString = circleRGB.substr(secondColonIndex + 1, circleRGB.size() - secondColonIndex - 1);
#ifdef _DEBUG
					DisplayUserMessage("Message", "RDF Plugin", (std::string("R: ") + redString + std::string(" G: ") + greenString + std::string(" B: ") + blueString).c_str(), false, false, false, false, false);
#endif

					if (!redString.empty() && !greenString.empty() && !blueString.empty())
					{
						rdfRGB = RGB(std::stoi(redString), std::stoi(greenString), std::stoi(blueString));
					}
				}
			}
		}
	}
	catch (std::runtime_error const& e)
	{
		DisplayUserMessage("Message", "RDF Plugin", (string("Error: ") + e.what()).c_str(), false, false, false, false, false);
	}
	catch (...)
	{
		DisplayUserMessage("Message", "RDF Plugin", ("Unexpected error: " + std::to_string(GetLastError())).c_str(), false, false, false, false, false);
	}

	return new CRDFScreen(this, rdfRGB);
}
