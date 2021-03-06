#pragma once

#include "afx.h"
#include "Cdib.h"
class CImageOperation
{

public:
	CImageOperation();
	~CImageOperation();
	BOOL StaColor(LONG	lHeight,LONG lWidth,LONG lLineBytes, int pixelByte, double * d_Proba,LPBYTE lpDIBBits);
	BOOL LapplacianED(LONG	lHeight,LONG lWidth, LONG lLineBytes, int pixelByte, LPBYTE  lpDIBBits);
	BOOL SobelED(LONG	lHeight, LONG lWidth, LONG lLineBytes, int pixelByte, LPBYTE  lpDIBBits);
	BOOL Fusion(LONG lHeight, LONG lWidth, LONG lLineBytes, int pixelByte, double * d_Proba
	,LPBYTE lpDIBBits,LPBYTE lp_OLD_DIBBits);
private:
	CDib *m_pDib;

};

