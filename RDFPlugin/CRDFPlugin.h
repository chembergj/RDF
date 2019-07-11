#include <EuroScopePlugIn.h>
#include <string>
#include <chrono>

#pragma once

using namespace std;
using namespace EuroScopePlugIn;

const string MY_PLUGIN_NAME = "RDF Plugin for Euroscope";
const string MY_PLUGIN_VERSION = "1.0cd ";
const string MY_PLUGIN_DEVELOPER = "Claus Hemberg Joergensen";
const string MY_PLUGIN_COPYRIGHT = "Free to be distributed as source code";

class CRDFPlugin: public EuroScopePlugIn::CPlugIn
{
private:
	int rxcount = 0;
	string activeTransmittingPilot;
	string previousActiveTransmittingPilot;
	std::chrono::steady_clock::time_point lastOnGetTagItemTime = std::chrono::steady_clock::now();

	string GetActivePilotCallsign(string voiceServer, string voiceChannel);
public:
	CRDFPlugin();
	virtual ~CRDFPlugin();

	virtual void OnVoiceReceiveStarted(CGrountToAirChannel Channel);
	virtual void OnVoiceReceiveEnded(CGrountToAirChannel Channel);
	virtual CRadarScreen *OnRadarScreenCreated(const char * sDisplayName, bool NeedRadarContent, bool GeoReferenced, bool CanBeSaved, bool CanBeCreated);
	void OnGetTagItem(EuroScopePlugIn::CFlightPlan FlightPlan, EuroScopePlugIn::CRadarTarget RadarTarget, int ItemCode, int TagData, char sItemString[16], int * pColorCode, COLORREF * pRGB, double * pFontSize);

	inline string GetActiveTransmittingPilot()
	{
		return activeTransmittingPilot;
	}

	inline string GetPreviousActiveTransmittingPilot()
	{
		return previousActiveTransmittingPilot;
	}

	// DEBUG ONLY
	virtual void OnVoiceTransmitStarted(bool OnPrimary);
	virtual void OnVoiceTransmitEnded(bool OnPrimary);
};

