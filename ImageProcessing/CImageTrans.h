#pragma once
#include "stdafx.h"
BOOL Mirror(LONG lHeight, LONG lWidth, LONG lLineBytes, int pixelByte,  LPBYTE lpDIBBits)
{
	LPBYTE	lpSrc;			// 指向原图像的指针	
	LPBYTE	lpDst;			// 指向缓存图像的指针	
	LPBYTE	lpBits;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	long i, j;			//循环变量
	long i1;                 //行循环变量
	long j1;                 //列循环变量
	BYTE pixel;	//像素值
	int int_pixel;

	hNewDIBBits = LocalAlloc(LHND, lLineBytes *lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}

	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)0, lLineBytes *lHeight);
	for (i = 0; i < lHeight / 2; i++)// 垂直镜像，针对图像每行进行操作
	{
		//for(j = 0; j < lWidth / 2; j++)// 针对每行图像左半部分进行操作
		//{							
		lpSrc = (LPBYTE)lpDIBBits + lLineBytes * i;
		lpBits = (LPBYTE)lpDIBBits + lLineBytes * (lHeight - i - 1);

		memcpy(lpNewDIBBits, lpBits, lLineBytes);
		memcpy(lpBits, lpSrc, lLineBytes);
		memcpy(lpSrc, lpNewDIBBits, lLineBytes);
		//*lpDstDIBBits=*lpBits;//保存中间像素								
		//*lpBits = *lpSrcDIBBits;// 将倒数第i行，第j个像素复制到倒数第i行，倒数第j个像素								
		//*lpSrcDIBBits=*lpDstDIBBits;// 将倒数第i行，倒数第j个像素复制到倒数第i行，第j个像素
		//}			
	}
	//memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
}