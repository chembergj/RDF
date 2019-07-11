#pragma once
#include <EuroScopePlugIn.h>
#include "CRDFPlugin.h"

using namespace std;
using namespace EuroScopePlugIn;

class CRDFScreen : public CRadarScreen
{
private:
	CRDFPlugin *rdfPlugin;

	bool PlaneIsVisible(POINT p, RECT radarArea);
public:
	CRDFScreen(CRDFPlugin *plugin);
	virtual ~CRDFScreen();

	virtual void    OnAsrContentToBeClosed(void);
	virtual void OnRefresh(HDC hDC, int Phase);

	

};

