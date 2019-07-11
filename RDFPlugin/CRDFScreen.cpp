#include "stdafx.h"
#include "CRDFScreen.h"


CRDFScreen::CRDFScreen(CRDFPlugin *plugin)
{
	this->rdfPlugin = plugin;
}


CRDFScreen::~CRDFScreen()
{
}

void CRDFScreen::OnAsrContentToBeClosed(void)
{
	delete this;
}

void CRDFScreen::OnRefresh(HDC hDC, int Phase)
{
	if (Phase == REFRESH_PHASE_AFTER_TAGS) {

		for (CRadarTarget radarTarget = GetPlugIn()->RadarTargetSelectFirst(); radarTarget.IsValid();
			radarTarget = GetPlugIn()->RadarTargetSelectNext(radarTarget))
		{
			if (rdfPlugin->GetActiveTransmittingPilot() == radarTarget.GetCallsign()
				||
				(rdfPlugin->GetActiveTransmittingPilot().empty() 
				&& rdfPlugin->GetPreviousActiveTransmittingPilot() == radarTarget.GetCallsign()
				&& (GetKeyState(VK_MBUTTON) == -127 || GetKeyState(VK_MBUTTON) == -128)))
			{
				// We are now highlighting either
				// 1) the present transmitting pilot, or...
				// 2) since the middle mouse button is pressed, the most recent transmitting pilot
				
				
				POINT p = ConvertCoordFromPositionToPixel(radarTarget.GetPosition().GetPosition());
				
				HGDIOBJ oldBrush = SelectObject(hDC, GetStockObject(HOLLOW_BRUSH));
				// HGDIOBJ oldPen = SelectObject(hDC, GetStockObject(BLACK_PEN));
				if (PlaneIsVisible(p, GetRadarArea()))
				{
					Ellipse(hDC, p.x - 50, p.y - 50, p.x + 50, p.y + 50);
				}
				else
				{
					// Outside screen - draw line
					POINT oldPoint;
					MoveToEx(hDC, (GetRadarArea().right - GetRadarArea().left) / 2, (GetRadarArea().bottom - GetRadarArea().top) / 2, &oldPoint);
					LineTo(hDC, p.x, p.y);
					MoveToEx(hDC, oldPoint.x, oldPoint.y, NULL);
				}

				SelectObject(hDC, oldBrush);
				// SelectObject(hDC, oldPen);
			}
		}
	}
}

bool CRDFScreen::PlaneIsVisible(POINT p, RECT radarArea)
{
	return p.x >= radarArea.left && p.x <= radarArea.right && p.y >= radarArea.top && p.y <= radarArea.bottom;
}

	

