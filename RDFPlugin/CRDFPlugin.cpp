#include "stdafx.h"
#include "CRDFPlugin.h"
#include "CRDFScreen.h"
#include <wininet.h>

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

void CRDFPlugin::OnVoiceReceiveStarted(CGrountToAirChannel Channel)
{
	rxcount = 0;

	try
	{
		string callsign = GetActivePilotCallsign(Channel.GetVoiceServer(), Channel.GetVoiceChannel());
		activeTransmittingPilot = callsign;
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
#ifdef _DEBUG
	const char* TestRDFCallsign = GetDataFromSettings("TestRDFCallsign");
	if (TestRDFCallsign != NULL) {
		activeTransmittingPilot = TestRDFCallsign;
	}
	else
	{
		SaveDataToSettings("TestRDFCallsign", "In debug mode: which aircraft to highlight during voice transmit", "Test123");
	}
#endif
}

void CRDFPlugin::OnVoiceTransmitEnded(bool OnPrimary)
{
#ifdef _DEBUG
	previousActiveTransmittingPilot = activeTransmittingPilot;
	activeTransmittingPilot.clear(); 
#endif
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
#ifdef _DEBUG
	DisplayUserMessage("Message", "RDF Plugin", (std::string("OnVoiceReceiveEnded ") + Channel.GetName()).c_str(), false, false, false, false, false);
#endif
	previousActiveTransmittingPilot = activeTransmittingPilot;
	activeTransmittingPilot.clear();
}