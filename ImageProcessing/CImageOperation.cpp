#include "stdafx.h"
#include "CImageOperation.h"

CImageOperation::CImageOperation()
{

}

CImageOperation::~CImageOperation()
{
}
//ͳ�ƻҶ�ͼ�ĻҶȷֲ� ����24λ��ɫͼ��HSIɫ��ģ�͵�I��I=��r+g+b��/3����
BOOL CImageOperation::StaColor(LONG lHeight, LONG lWidth, LONG lLineBytes, int pixelByte, double * dProba,LPBYTE lpDIBBits)
{
	LONG i, j, k;
	LPBYTE lpSrc;
	// ��������Ҷ�ֵ�ļ���
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			int pixelSum = 0;

			for (k = 0; k < pixelByte; k++)
			{
				//ָ��ͼ��ָ��
				lpSrc = lpDIBBits + lLineBytes * i + j * pixelByte + k;
				pixelSum += *lpSrc;
			}
			if (pixelByte > 1)
				pixelSum /= 3;//HSI ��ɫģ�͵�I=��r+g+b)/3;
			
			// ������1
			dProba[pixelSum] = dProba[pixelSum] + 1;
		}
	}
	return TRUE;
}

BOOL CImageOperation::LapplacianED(LONG lHeight, LONG lWidth, LONG lLineBytes, int pixelByte, LPBYTE  lpDIBBits)
{
	HLOCAL hNewDIBBits;
	LPBYTE	lpNewDIBBits;	// ָ�򻺴�DIBͼ���ָ��
	LPBYTE	lpDst;			// ָ�򻺴�ͼ���ָ��	
	LPBYTE lpSrc;
	// ѭ������
	LONG	i;
	LONG	j;
	LONG    k;
	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// ��ʼ���·�����ڴ棬�趨��ʼֵΪ0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)0, lLineBytes * lHeight);

	int x, y;
	BYTE t;
	// ��������Ҷ�ֵ�ļ��������ڷ�256ɫλͼ���˴�������dProba��ֵ��������ͬ��
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			for (k = 0; k < pixelByte; k++)
			{
				if (i == 0 || i == lHeight - 1 || j == 0 || j == lWidth - 1)
				{	// ָ��ͼ��ָ��
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
	//�ͷ��ڴ�
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	return TRUE;

}

BOOL CImageOperation::SobelED(LONG lHeight, LONG lWidth, LONG lLineBytes, int pixelByte, LPBYTE lpDIBBits)
{
	HLOCAL hNewDIBBits;
	LPBYTE	lpNewDIBBits;	// ָ�򻺴�DIBͼ���ָ��
	LPBYTE	lpDst;			// ָ�򻺴�ͼ���ָ��	
	LPBYTE lpSrc;
	// ѭ������
	LONG	i;
	LONG	j;
	LONG    k;
	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// ��ʼ���·�����ڴ棬�趨��ʼֵΪ0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);

	int x, y;
	BYTE t;
	// ��������Ҷ�ֵ�ļ��������ڷ�256ɫλͼ���˴�������dProba��ֵ��������ͬ��
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			for (k = 0; k < pixelByte; k++)
			{
				if (i == 0 || i == lHeight - 1 || j == 0 || j == lWidth - 1)
				{	// ָ��ͼ��ָ��
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
					//y������
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
	//�ͷ��ڴ�
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	return TRUE;
}
BOOL CImageOperation::Fusion( LONG lHeight, LONG lWidth, LONG lLineBytes, int pixelByte, double * d_Proba,
	LPBYTE lpDIBBits, LPBYTE lp_OLD_DIBBits)
{
	LPBYTE	lpSrc;			// ָ��ԭͼ���ָ��	
	LPBYTE	fusion_lpSrc;			// 

	LPBYTE	lpDst;			// ָ�򻺴�ͼ���ָ��	
	LPBYTE	lpNewDIBBits;	// ָ�򻺴�DIBͼ���ָ��
	HLOCAL	hNewDIBBits;
	long i, j;			//ѭ������
	long i1;                 //��ѭ������
	long j1;                 //��ѭ������
	BYTE pixel;	//����ֵ
	//LPBYTE lpDIBBits = m_pDib->GetData();//�ҵ�ԭͼ�����ʼλ��
	//LPBYTE fusionlpDIBBits = fusion_pDib->GetData();//�ҵ�ԭͼ�����ʼλ��
	//LONG lLineBytes = m_pDib->GetLineByte();// ������ͼ��ÿ�е��ֽ���
	//LONG fusion_lLineBytes = fusion_pDib->GetLineByte();// ������ͼ��ÿ�е��ֽ���

	//LONG lWidth = m_pDib->GetWidth();    //���ԭͼ��Ŀ��
	//LONG lHeight = m_pDib->GetHeight();  //���ԭͼ��ĸ߶�

	hNewDIBBits = LocalAlloc(LHND, lLineBytes *lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}

	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);

	// ��ʼ���·�����ڴ棬�趨��ʼֵΪ0
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
	// ���Ʊ任���ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	//�ͷ��ڴ�
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	return TRUE;
}
