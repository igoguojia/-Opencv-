#pragma once
#include "stdafx.h"
BOOL Mirror(LONG lHeight, LONG lWidth, LONG lLineBytes, int pixelByte,  LPBYTE lpDIBBits)
{
	LPBYTE	lpSrc;			// ָ��ԭͼ���ָ��	
	LPBYTE	lpDst;			// ָ�򻺴�ͼ���ָ��	
	LPBYTE	lpBits;
	LPBYTE	lpNewDIBBits;	// ָ�򻺴�DIBͼ���ָ��
	HLOCAL	hNewDIBBits;
	long i, j;			//ѭ������
	long i1;                 //��ѭ������
	long j1;                 //��ѭ������
	BYTE pixel;	//����ֵ
	int int_pixel;

	hNewDIBBits = LocalAlloc(LHND, lLineBytes *lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}

	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);

	// ��ʼ���·�����ڴ棬�趨��ʼֵΪ0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)0, lLineBytes *lHeight);
	for (i = 0; i < lHeight / 2; i++)// ��ֱ�������ͼ��ÿ�н��в���
	{
		//for(j = 0; j < lWidth / 2; j++)// ���ÿ��ͼ����벿�ֽ��в���
		//{							
		lpSrc = (LPBYTE)lpDIBBits + lLineBytes * i;
		lpBits = (LPBYTE)lpDIBBits + lLineBytes * (lHeight - i - 1);

		memcpy(lpNewDIBBits, lpBits, lLineBytes);
		memcpy(lpBits, lpSrc, lLineBytes);
		memcpy(lpSrc, lpNewDIBBits, lLineBytes);
		//*lpDstDIBBits=*lpBits;//�����м�����								
		//*lpBits = *lpSrcDIBBits;// ��������i�У���j�����ظ��Ƶ�������i�У�������j������								
		//*lpSrcDIBBits=*lpDstDIBBits;// ��������i�У�������j�����ظ��Ƶ�������i�У���j������
		//}			
	}
	//memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
}