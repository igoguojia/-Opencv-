#include "stdafx.h"
#include "CImageOperation.h"

CImageOperation::CImageOperation()
{

}

CImageOperation::~CImageOperation()
{
}
//统计灰度图的灰度分布 或是24位彩色图的HSI色彩模型的I（I=（r+g+b）/3）；
BOOL CImageOperation::StaColor(LONG lHeight, LONG lWidth, LONG lLineBytes, int pixelByte, double * dProba,LPBYTE lpDIBBits)
{
	LONG i, j, k;
	LPBYTE lpSrc;
	// 计算各个灰度值的计数
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			int pixelSum = 0;

			for (k = 0; k < pixelByte; k++)
			{
				//指向图象指针
				lpSrc = lpDIBBits + lLineBytes * i + j * pixelByte + k;
				pixelSum += *lpSrc;
			}
			if (pixelByte > 1)
				pixelSum /= 3;//HSI 颜色模型的I=（r+g+b)/3;
			
			// 计数加1
			dProba[pixelSum] = dProba[pixelSum] + 1;
		}
	}
	return TRUE;
}

BOOL CImageOperation::LapplacianED(LONG lHeight, LONG lWidth, LONG lLineBytes, int pixelByte, LPBYTE  lpDIBBits)
{
	HLOCAL hNewDIBBits;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	LPBYTE	lpDst;			// 指向缓存图像的指针	
	LPBYTE lpSrc;
	// 循环变量
	LONG	i;
	LONG	j;
	LONG    k;
	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)0, lLineBytes * lHeight);

	int x, y;
	BYTE t;
	// 计算各个灰度值的计数（对于非256色位图，此处给数组dProba赋值方法将不同）
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			for (k = 0; k < pixelByte; k++)
			{
				if (i == 0 || i == lHeight - 1 || j == 0 || j == lWidth - 1)
				{	// 指向图象指针
					lpSrc = lpDIBBits + lLineBytes * i + j * pixelByte + k;
					lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
					*lpDst = (BYTE)*lpSrc;
				}
				else
				{
					x = 4 * *(lpDIBBits + i * lLineBytes + j * pixelByte + k)
						- *(lpDIBBits + (i + 1)*lLineBytes + j * pixelByte + k)
						- *(lpDIBBits + (i - 1)*lLineBytes + j * pixelByte + k)
						- *(lpDIBBits + i * lLineBytes + (j - 1)*pixelByte + k)
						- *(lpDIBBits + i * lLineBytes + (j + 1)*pixelByte + k);
					t = (BYTE)(x / 4);
					//t = (BYTE)x;
					if (t > 255)
						t = 255;
					lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;

					*lpDst = t;

				}
			}
		}
	}
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	//释放内存
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	return TRUE;

}

BOOL CImageOperation::SobelED(LONG lHeight, LONG lWidth, LONG lLineBytes, int pixelByte, LPBYTE lpDIBBits)
{
	HLOCAL hNewDIBBits;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	LPBYTE	lpDst;			// 指向缓存图像的指针	
	LPBYTE lpSrc;
	// 循环变量
	LONG	i;
	LONG	j;
	LONG    k;
	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);

	int x, y;
	BYTE t;
	// 计算各个灰度值的计数（对于非256色位图，此处给数组dProba赋值方法将不同）
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			for (k = 0; k < pixelByte; k++)
			{
				if (i == 0 || i == lHeight - 1 || j == 0 || j == lWidth - 1)
				{	// 指向图象指针
					lpSrc = lpDIBBits + lLineBytes * i + j * pixelByte + k;
					lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
					*lpDst = (BYTE)*lpSrc;
				}
				else
				{
					x = *(lpDIBBits + (i - 1)*lLineBytes + (j + 1)*pixelByte + k)
						+ 2 * *(lpDIBBits + i * lLineBytes + (j + 1)*pixelByte + k)
						+ *(lpDIBBits + (i + 1)*lLineBytes + (j + 1)*pixelByte + k)
						- *(lpDIBBits + (i - 1)*lLineBytes + (j - 1)*pixelByte + k)
						- 2 * *(lpDIBBits + i * lLineBytes + (j - 1)*pixelByte + k)
						- *(lpDIBBits + (i + 1)*lLineBytes + (j - 1)*pixelByte + k);
					//y方向梯
					y = *(lpDIBBits + (i - 1)*lLineBytes + (j - 1)*pixelByte + k)
						+ 2 * *(lpDIBBits + (i - 1)*lLineBytes + j * pixelByte + k)
						+ *(lpDIBBits + (i - 1)*lLineBytes + (j + 1)*pixelByte + k)
						- *(lpDIBBits + (i + 1)*lLineBytes + (j - 1)*pixelByte + k)
						- 2 * *(lpDIBBits + (i + 1)*lLineBytes + j * pixelByte + k)
						- *(lpDIBBits + (i + 1)*lLineBytes + (j + 1)*pixelByte + k);
					x /= 4;
					y /= 4;
					t = (BYTE)(sqrt(x*x + y * y) + 0.5);
					if (t > 255)
						t = 255;
					lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;

					*lpDst = t;

				}
			}
		}
	}
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	//释放内存
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	return TRUE;
}
BOOL CImageOperation::Fusion( LONG lHeight, LONG lWidth, LONG lLineBytes, int pixelByte, double * d_Proba,
	LPBYTE lpDIBBits, LPBYTE lp_OLD_DIBBits)
{
	LPBYTE	lpSrc;			// 指向原图像的指针	
	LPBYTE	fusion_lpSrc;			// 

	LPBYTE	lpDst;			// 指向缓存图像的指针	
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	long i, j;			//循环变量
	long i1;                 //行循环变量
	long j1;                 //列循环变量
	BYTE pixel;	//像素值
	//LPBYTE lpDIBBits = m_pDib->GetData();//找到原图像的起始位置
	//LPBYTE fusionlpDIBBits = fusion_pDib->GetData();//找到原图像的起始位置
	//LONG lLineBytes = m_pDib->GetLineByte();// 计算新图像每行的字节数
	//LONG fusion_lLineBytes = fusion_pDib->GetLineByte();// 计算新图像每行的字节数

	//LONG lWidth = m_pDib->GetWidth();    //获得原图像的宽度
	//LONG lHeight = m_pDib->GetHeight();  //获得原图像的高度

	hNewDIBBits = LocalAlloc(LHND, lLineBytes *lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}

	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)0, lLineBytes *lHeight);
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lLineBytes; j++)
		{
			lpSrc = (LPBYTE)lpDIBBits + lLineBytes * i + j;
			fusion_lpSrc = (LPBYTE)lp_OLD_DIBBits + lLineBytes * i + j;
			pixel = (BYTE)(*lpSrc) / 2 + (BYTE)(*fusion_lpSrc) / 2;
			if (pixel > 255)
				pixel = 255;
			lpDst = (LPBYTE)lpNewDIBBits + lLineBytes * i + j;
			
			*lpDst = pixel;
			d_Proba[*(lpDst)] = d_Proba[*(lpDst)] + 1;

		}

	}
	// 复制变换后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	//释放内存
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	return TRUE;
}
