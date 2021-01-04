#include "stdafx.h"
#include "CRDFScreen.h"


CRDFScreen::CRDFScreen(CRDFPlugin *plugin, COLORREF rdfColor, COLORREF rdfConcurrentTransmissionsColor, int CircleRadius)
{
	this->rdfPlugin = plugin;
	this->rdfColor = rdfColor;
	this->rdfConcurrentTransmissionsColor = rdfConcurrentTransmissionsColor;
	this->circleRadius = CircleRadius;
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
			if (rdfPlugin->GetActiveTransmittingPilots().find(radarTarget.GetCallsign()) != rdfPlugin->GetActiveTransmittingPilots().end()
				||
				(rdfPlugin->GetActiveTransmittingPilots().empty() 
				&& (rdfPlugin->GetPreviousActiveTransmittingPilots().find(radarTarget.GetCallsign()) != rdfPlugin->GetPreviousActiveTransmittingPilots().end())
				&& (GetKeyState(VK_MBUTTON) == -127 || GetKeyState(VK_MBUTTON) == -128)))
			{
				// We are now highlighting either
				// 1) the present transmitting pilot, or...
				// 2) since the middle mouse button is pressed, the most recent transmitting pilot
				
				
				POINT p = ConvertCoordFromPositionToPixel(radarTarget.GetPosition().GetPosition());
				
				HGDIOBJ oldBrush = SelectObject(hDC, GetStockObject(HOLLOW_BRUSH));

				// Which color should be used? rdfColor for single transmission, rdfConcurrentTransmisionsColor for more
				COLORREF penColor;
				if (rdfPlugin->GetActiveTransmittingPilots().size() == 1)
				{
					penColor = rdfColor;
				}
				else if (rdfPlugin->GetActiveTransmittingPilots().size() > 1)
				{
					penColor = rdfConcurrentTransmissionsColor;
				}
				else if (rdfPlugin->GetPreviousActiveTransmittingPilots().size() == 1)
				{
					penColor = rdfColor;
				}
				else if (rdfPlugin->GetPreviousActiveTransmittingPilots().size() > 1)
				{
					penColor = rdfConcurrentTransmissionsColor;
				}

				HPEN hPen = CreatePen(PS_SOLID, 1, penColor);
				HGDIOBJ oldPen = SelectObject(hDC, hPen);

				if (PlaneIsVisible(p, GetRadarArea()))
				{
					Ellipse(hDC, p.x - circleRadius, p.y - circleRadius, p.x + circleRadius, p.y + circleRadius);
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
				SelectObject(hDC, oldPen);

				DeleteObject(hPen);
			}
		}
	}
}

bool CRDFScreen::PlaneIsVisible(POINT p, RECT radarArea)
{
	return p.x >= radarArea.left && p.x <= radarArea.right && p.y >= radarArea.top && p.y <= radarArea.bottom;
}

	

