#include "stdafx.h"
#include "CRDFPlugin.h"
#include "CRDFScreen.h"
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

using namespace std;

const   int     TAG_ITEM_VOICERECEIVE = 1;
const string RDF_DISPLAY = "Radio Direction Finder";

CRDFPlugin::CRDFPlugin()
	: EuroScopePlugIn::CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE,
		MY_PLUGIN_NAME.c_str(),
		MY_PLUGIN_VERSION.c_str(),
		MY_PLUGIN_DEVELOPER.c_str(),
		MY_PLUGIN_COPYRIGHT.c_str())
{
	RegisterTagItemType("RX in progress", TAG_ITEM_VOICERECEIVE);
	DisplayUserMessage("Message", "RDF", std::string("Version " + MY_PLUGIN_VERSION + " loaded").c_str(), false, false, false, false, false);
	RegisterDisplayType(RDF_DISPLAY.c_str(), true, true, true, true);
}


CRDFPlugin::~CRDFPlugin()
{
}

CRadarScreen * CRDFPlugin::OnRadarScreenCreated(const char * sDisplayName,
	bool NeedRadarContent,
	bool GeoReferenced,
	bool CanBeSaved,
	bool CanBeCreated)
{
	bool isRDF = (RDF_DISPLAY == sDisplayName);

	if (isRDF) 
	{
		DisplayUserMessage("Message", "RDF Plugin", "Radar Direction Finder display opened", false, false, false, false, false);
	}

	return RDF_DISPLAY == sDisplayName ? new CRDFScreen(this) : NULL;
}

void CRDFPlugin::OnVoiceReceiveStarted(CGrountToAirChannel Channel)
{
	rxcount = 0;

	try
	{
		string callsign = GetActivePilotCallsign(Channel.GetVoiceServer(), Channel.GetVoiceChannel());
		activeTransmittingPilot = callsign;
		DisplayUserMessage("Message", "RDF Plugin", (std::string("OnVoiceReceiveStarted, GetActivePilotCallsign=") + callsign).c_str(), false, false, false, false, false);
	}
	catch (std::runtime_error const& e)
	{
		DisplayUserMessage("Message", "RDF Plugin", (string("Error: ") + e.what()).c_str(), false, false, false, false, false);
	}
	catch(...)
	{
		DisplayUserMessage("Message", "RDF Plugin", ("Unexpected error: " + std::to_string(GetLastError())).c_str(), false, false, false, false, false);
	}
}

void    CRDFPlugin::OnVoiceTransmitStarted(bool OnPrimary)
{
	// DEBUG ONLY: activeTransmittingPilot = "TAP1979";

}

void CRDFPlugin::OnVoiceTransmitEnded(bool OnPrimary)
{
	/* DEBUG ONLY: 
	previousActiveTransmittingPilot = activeTransmittingPilot;
	activeTransmittingPilot.clear(); */
}

string CRDFPlugin::GetActivePilotCallsign(string voiceServer, string voiceChannel)
{
	string sUrl = "http://" + voiceServer + ":18009?opts=-R-D";

	HINTERNET hConnect = InternetOpen("RDF Agent", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hConnect) {
		HINTERNET hOpen = InternetOpenUrl(hConnect, sUrl.c_str(), NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0);
		if (hOpen) {
			char data[1024];
			DWORD bytesRead{ 0 };
			std::string result;
			while (InternetReadFile(hOpen, data, 1024, &bytesRead) && bytesRead)
			{
				result.append(data, bytesRead);
			}
			InternetCloseHandle(hOpen);
			InternetCloseHandle(hConnect);

			// Example:  /enbr_w_appCurrently transmitting: PIP7R (1309217)<br>
			string searchString = "/" + voiceChannel + "Currently transmitting: ";
			size_t startpos = result.find(searchString);
			size_t endpos = result.find("(", startpos);
			size_t endBRpost = result.find("<br>", startpos);
			if (endBRpost < endpos)
			{
				// <br> found before ) meaning no callsign
				return "";
			}

			return result.substr(startpos + searchString.length(), endpos - startpos - searchString.length() - 1);
		}
		else {
			InternetCloseHandle(hConnect);
			throw runtime_error("Error " + std::to_string(GetLastError()) + " in InternetOpenUrl");
		}
	}
	else
	{
		throw runtime_error("Error " + std::to_string(GetLastError()) + " in InternetOpen");
	}

}

void CRDFPlugin::OnVoiceReceiveEnded(CGrountToAirChannel Channel)
{
	DisplayUserMessage("Message", "RDF Plugin", (std::string("OnVoiceReceiveEnded ") + Channel.GetName()).c_str(), false, false, false, false, false);
	previousActiveTransmittingPilot = activeTransmittingPilot;
	activeTransmittingPilot.clear();
}

void CRDFPlugin::OnGetTagItem(EuroScopePlugIn::CFlightPlan FlightPlan, 
	EuroScopePlugIn::CRadarTarget RadarTarget,
	int ItemCode,
	int TagData,
	char sItemString[16],
	int * pColorCode,
	COLORREF * pRGB,
	double * pFontSize)
{

	// only for flight plans
	if (!FlightPlan.IsValid())
		return;

	// stitch by the code
	switch (ItemCode)
	{
	case TAG_ITEM_VOICERECEIVE:
		if (activeTransmittingPilot == RadarTarget.GetCallsign()	// Draw circle if target is currently active
			|| (activeTransmittingPilot.empty()						// ..or if noone is active, but the middle button is pressed and the recent active pilot as our target
				&& previousActiveTransmittingPilot == RadarTarget.GetCallsign()
				&& (GetKeyState(VK_MBUTTON) == -127 || GetKeyState(VK_MBUTTON) == -128))
			
			) {

			// Only update each half second to avoid flicker
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastOnGetTagItemTime).count() >= 500)
			{
				rxcount++;
				lastOnGetTagItemTime = std::chrono::steady_clock::now();
			}
			strcpy_s(sItemString, 16, rxcount % 2 == 0 ? "***** RX *****" : "      RX      ");
		}
		

		break;
	}
}