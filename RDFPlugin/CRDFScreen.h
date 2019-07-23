#pragma once
#include <EuroScopePlugIn.h>
#include "CRDFPlugin.h"

using namespace std;
using namespace EuroScopePlugIn;

class CRDFScreen : public CRadarScreen
{
private:
	CRDFPlugin *rdfPlugin;
	COLORREF rdfColor;

	bool PlaneIsVisible(POINT p, RECT radarArea);
public:
	CRDFScreen(CRDFPlugin *plugin, COLORREF rdfColor);
	virtual ~CRDFScreen();

	virtual void    OnAsrContentToBeClosed(void);
	virtual void OnRefresh(HDC hDC, int Phase);

	

};

