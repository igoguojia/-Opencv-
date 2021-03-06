#include "stdafx.h"
#include "GlobalApi.h"
#include "Cdib.h"

#include <io.h>
#include <errno.h>

#include <math.h>
#include <direct.h>
//using namespace std;

// FOURBYTES��������������4�����������
#define FOURBYTES(bits)    (((bits) + 31) / 32 * 4)

/**************************************************************************
 *  �ļ�����ImageCoding.cpp
 *
 *  �����任API�����⣺
 *
 *  WRITE2IMG()				- 256ɫͼ�󱣴�Ϊ256ɫIMG�ļ�
 *  LOADIMG()				- ��ȡָ����256ɫIMG�ļ� 
 *  DIBBITPLANE()			- ͼ��λƽ��ֽ�
 *
 ************************************************************************
*/

// PCX�ļ�ͷ�ṹ
typedef struct{
		 BYTE bManufacturer;
		 BYTE bVersion;
		 BYTE bEncoding;
		 BYTE bBpp;
		 WORD wLeft;
		 WORD wTop;
		 WORD wRight;
		 WORD wBottom;
		 WORD wXResolution;
		 WORD wYResolution;
		 BYTE bPalette[48];
		 BYTE bReserved;
		 BYTE bPlanes;
		 WORD wLineBytes;
		 WORD wPaletteType;
		 WORD wSrcWidth;
		 WORD wSrcDepth;
		 BYTE bFiller[54];
} PCXHEADER;

typedef struct{
		 BYTE bBpp;
		 WORD wLeft;
		 WORD wTop;
		 WORD wRight;
		 WORD wBottom;
		 WORD wXResolution;
		 WORD wYResolution;
} IMGHEADER;

/*************************************************************************
 *
 * �������ƣ�
 *   WRITE2IMG()
 *
 * ����:
 *   CDib * pDib        - ָ��CDib�����ָ��
 *   CFile& file        - Ҫ������ļ�
 *
 * ����ֵ:
 *   BOOL               - �ɹ�����True�����򷵻�False��
 *
 * ˵��:
 *   �ú�����ָ����ͼ�󱣴�ΪIMG�ļ���
 *
 *************************************************************************/
BOOL WINAPI WRITE2IMG(CDib * pDib, CFile& file)
{
	
	// ָ��Դͼ���ָ��
	unsigned char*	lpSrc;

	//ͼ��Ŀ�Ⱥ͸߶�
	LONG    lWidth;
	LONG    lHeight;

	// ͼ��ÿ�е��ֽ���
	LONG	lLineBytes;
	
	// ѭ������
	LONG	i;
	LONG	j;
		
	// ����Ԥ������غ͵�ǰ���������
	BYTE	bCharA;
	BYTE	bCharB;
	BYTE	bCharC;
	BYTE	bCharD;
    
	// Ԥ��ֵ
	int		nTemp;
	
	// Ԥ���Ĳв�
	int		nDpcm;
			
	// ָ������ͼ�����ݵ�ָ��
	BYTE *	lpDst;		
		
	//�õ�ͼ��Ŀ�Ⱥ͸߶�
	CSize   SizeDim;
	SizeDim = pDib->GetDimensions();
	lWidth  = SizeDim.cx;
	lHeight = SizeDim.cy;	
	
	//�õ�ʵ�ʵ�Dibͼ��洢��С
	CSize   SizeRealDim;
	SizeRealDim = pDib->GetDibSaveDim();

	// ����ͼ��ÿ�е��ֽ���
	lLineBytes = SizeRealDim.cx;
	
	//ͼ�����ݵ�ָ��
	LPBYTE  lpDIBBits = pDib->m_lpImage;
			
	/**********************************************************************
	*д��IMG�ļ�ͷ��Ϣ
	***********************************************************************
	*/
	IMGHEADER Header4IMG;
	
	// ���ļ�ͷ��ֵ
		
	// ����λ����256ɫΪ8λ��
	Header4IMG.bBpp = 8;
	
	// ͼ���������Ļ�����Ͻ�X���꣨������Ϊ��λ��
	Header4IMG.wLeft = 0;
	
	// ͼ���������Ļ�����Ͻ�Y���꣨������Ϊ��λ��
	Header4IMG.wTop = 0;
	
	// ͼ���������Ļ�����½�X���꣨������Ϊ��λ��
	Header4IMG.wRight = lWidth - 1;
	
	// ͼ���������Ļ�����½�Y���꣨������Ϊ��λ��
	Header4IMG.wBottom = lHeight - 1;
	
	// ͼ���ˮƽ�ֱ���
	Header4IMG.wXResolution = lWidth;
	
	// ͼ��Ĵ�ֱ�ֱ���
	Header4IMG.wYResolution = lHeight;

	// д���ļ�ͷ
	file.Write((LPSTR)&Header4IMG, sizeof(IMGHEADER));	
	
	// �����0��
	i = 0;
	for ( j = 0; j < lWidth; j++)
	{
		// ָ��ͼ��0��j�����ص�ָ��
		lpSrc = (BYTE *)lpDIBBits + lLineBytes * (lHeight - 1 - i) +j ;
		
		// ��bCharD��ֵ
		bCharD = *lpSrc;

		// ����ǵ�0��0�У�ֱ�ӽ�����ֵд��
		if(j == 0)
			nDpcm = (int)bCharD;
		// ���� Dpcm ��D �� A ����в� 
		else
		{
			bCharA = *(lpSrc - 1);
			nDpcm  = (int)bCharD - (int)bCharA;
		}

		// ���в�д���ļ�
		file.Write(&nDpcm , sizeof(int));
	}
	
	// �����1�е�lHeight-1��
	for ( i=1;i<lHeight; i++)
	{
		for ( j = 0; j < lWidth; j++)
		{
			// ָ��ǰ����Ԫ�ص�ָ��
			lpSrc = (BYTE *)lpDIBBits + j + lLineBytes * (lHeight - 1 - i);
		
			// ��ֵ
			bCharD = *lpSrc;
			bCharB = *(lpSrc + lLineBytes);

			// ����ǵ�һ�У����� �вD ��B ����Ԥ��
			if(j == 0)
				nDpcm = (int)bCharD - (int)bCharB;
			else
			{
				// ����(B-C)/2+A����Ԥ��ֵ
				bCharA = *(lpSrc - 1);
				bCharC = *(lpSrc + lLineBytes - 1);
				nTemp  =  (int)((bCharB-bCharC) / 2 + bCharA);
				
				// ���Ԥ��ֵС��0��ֱ�Ӹ���
				if(nTemp < 0)
					nTemp = 0;
				// ���Ԥ��ֵ����255��ֱ�Ӹ�ֵ255
				else if(nTemp > 255)
					nTemp = 255;
				else
					nTemp = nTemp;

				// �õ��в�
				nDpcm = (int)bCharD - nTemp;
			}

			// ���в�д���ļ�
			file.Write(&nDpcm , sizeof(int));
		}
	}
	
	// ����һƬ�������Ա����ɫ��
	lpDst = new BYTE[769];
	
	// ��ɫ����ʼ�ֽ�
	* lpDst = 0x0C;	

	// �õ�ͼ��ĵ�ɫ��
	LPRGBQUAD lpbmc = (LPRGBQUAD)pDib->m_lpvColorTable;
	
	// ��ȡ��ǰͼ��ĵ�ɫ��
	for (i = 0; i < 256; i ++)
	{
		
			// ��ȡDIB��ɫ���ɫ����
			lpDst[i * 3 + 1] = lpbmc[i].rgbRed;
			
			// ��ȡDIB��ɫ����ɫ����
			lpDst[i * 3 + 2] = lpbmc[i].rgbGreen;
			
			// ��ȡDIB��ɫ����ɫ����
			lpDst[i * 3 + 3] = lpbmc[i].rgbBlue;
	}
	
	// д���ɫ����Ϣ
	file.Write((LPSTR)lpDst, 769);

	// ����ֵ
	return TRUE;
}

/*************************************************************************
 *
 * �������ƣ�
 *   LOADIMG()
 *
 * ����:
 *   CDib * pDib        - ָ��CDib���ָ��
 *   CFile& file        - Ҫ��ȡ���ļ�
 *
 * ����ֵ:
 *   BOOL               - �ɹ�����TRUE
 *
 * ˵��:
 *   �ú�������ȡָ����IMG�ļ�
 *
 *************************************************************************/
BOOL WINAPI LOADIMG(CDib * pDib, CFile& file)
{
	// ѭ������
	LONG	i;
	LONG	j;
		
	// ͼ��߶�
	LONG	lHeight;
	
	// ͼ����
	LONG	lWidth;
	
	// ͼ��ÿ�е��ֽ���
	LONG	lLineBytes;
	
	// �м����
	BYTE	bChar;
	int     nTemp;
	
	// ָ��Դͼ�����ص�ָ��
	int *	lpSrc;
	
	// ָ������ͼ�����ݵ�ָ��
	BYTE *	lpDst;
	
	// ��ʱָ��
	int *	lpTemp;

	// ��������Ԥ����������غ͵�ǰ��������
	BYTE	bCharA;
	BYTE	bCharB;
	BYTE	bCharC;

	// IMGͷ�ļ�
	IMGHEADER Header4IMG;
	
	/**********************************************************************
	*����IMG�ļ�ͷ��Ϣ
	***********************************************************************
	*/
	
	// ���Զ�ȡ�ļ�ͷ
	if (file.Read((LPSTR)&Header4IMG, sizeof(IMGHEADER)) 
		!= sizeof(IMGHEADER))
	{
		// ��С���ԣ�����NULL��
		return NULL;
	}
	
	// ��ȡͼ��߶�
	lHeight = Header4IMG.wBottom - Header4IMG.wTop + 1;
	
	// ��ȡͼ����
	lWidth  = Header4IMG.wRight - Header4IMG.wLeft + 1;
	
	// ����ͼ��ÿ�е��ֽ���
	lLineBytes = FOURBYTES(lWidth * 8);
		
	// �����ʾͼ���ͷ�ļ�
	LPBITMAPINFOHEADER lpBI=pDib->m_lpBMIH;	
		
	// ��ͼ��ͷ�ļ���Ա��ֵ
	lpBI->biSize = 40;
	lpBI->biWidth = lWidth;
	lpBI->biHeight = lHeight;
	lpBI->biPlanes = 1;
	lpBI->biBitCount = 8;
	lpBI->biCompression = BI_RGB;
	lpBI->biSizeImage = lHeight * lLineBytes;
	lpBI->biXPelsPerMeter = Header4IMG.wXResolution;
	lpBI->biYPelsPerMeter = Header4IMG.wYResolution;
	lpBI->biClrUsed = 0;
	lpBI->biClrImportant = 0;
	
	// �����ڴ��Զ�ȡ����������
	lpSrc = new int[(file.GetLength() - sizeof(IMGHEADER)-769) ];
	
	lpTemp = lpSrc;
	
	// ��ȡ����������
	if (file.Read(lpSrc, file.GetLength() - sizeof(IMGHEADER)-769) 
		!= file.GetLength() - sizeof(IMGHEADER)-769 )
	{
	
		return FALSE;
	}
	
	// ���·����ڴ棬����Ӧ�µ�ͼ���С
	delete 	pDib->m_lpImage;
	pDib->m_lpImage = new unsigned char[lHeight * lLineBytes];

	// CDib��������λ��
	lpDst = pDib->m_lpImage;
	
	// �����0��
	i = 0;
	for(j = 0; j < lWidth; j++)
	{
		if(j==0)
		{
			// �����0��0�У�����ֵ������ʵֵ
			lpDst[j + lLineBytes * (lHeight - 1 - i)] = (BYTE)(*lpTemp);
			lpTemp ++;
		}
		else
		{
			// ���� D��A���в� �õ�ԭ��������
			lpDst[j+ lLineBytes * (lHeight - 1 - i)]
				= (BYTE)(*lpTemp) + lpDst[j + lLineBytes * (lHeight - 1 - i) - 1];
			lpTemp++;
		}
	}

	// �����1�е���lHeight��1��
	for (i = 1; i < lHeight; i++)
    {
		for (j = 0; j < lWidth; j++)
		{
			// �õ�����B��ֵ
			bCharB = lpDst[j + lLineBytes * (lHeight  - i)];
			
			// �����һ��
			if(j==0)
			{
				// ���� D��B���в� �õ�ԭ��������ֵ
				lpDst[j+ lLineBytes * (lHeight - 1 - i)] = (BYTE)((*lpTemp) + bCharB);
				lpTemp++;
			}
			
			// ����ʣ�µ���
			else
			{
				// ���� D=(B-C)/2 + A + �в� �õ�ԭ��������ֵ
				bCharA=lpDst[j - 1 + lLineBytes * (lHeight - 1 - i)];
				bCharC=lpDst[j - 1 + lLineBytes * (lHeight - i)];
				
				// ����ʱ��Ԥ��
				nTemp=(int)((bCharB - bCharC) / 2  +bCharA);
				
				// Ԥ��ֵС��0��ֱ�Ӹ�0
				if(nTemp<0)
					nTemp = 0;
				// Ԥ��ֵ����255��ֱ�Ӹ�ֵ255
				else if(nTemp>255)
					nTemp = 255;				 
				else
					nTemp = nTemp;

				// Ԥ��ֵ���в�
				lpDst[j + lLineBytes * (lHeight - 1 - i)] 
					= (BYTE)(*lpTemp + (BYTE)nTemp);
				lpTemp++;
			}
		}
	}

	// �ͷ��ڴ�
	delete lpSrc;
	lpDst  = NULL;
	
	// ����ɫ���־λ
	file.Read(&bChar, 1);
	if (bChar != 0x0C)
	{		
		// ����NULL��
		return FALSE;
	}
	
	// �����ڴ��Զ�ȡ����������
	lpDst = new BYTE[768];
	
	// ͼ���е�ɫ���λ��
	LPRGBQUAD lpbmc = (LPRGBQUAD)pDib->m_lpvColorTable;
	
	// ��ȡ��ɫ��
	if (file.Read(lpDst, 768) != 768)
	{		
		return FALSE;
	}
	
	// ����ɫ�帳ֵ
	for (i = 0; i < 256; i++)
	{	
		lpbmc[i].rgbRed   = lpDst[i * 3 + 2];
		lpbmc[i].rgbGreen = lpDst[i * 3 + 1];
		lpbmc[i].rgbBlue  = lpDst[i * 3];
		lpbmc[i].rgbReserved = 0;
	}
	
	// ����ֵ
	return TRUE;
}


/*************************************************************************
 *
 * �������ƣ�
 *   DIBBITPLANE()
 *
 * ����:
 *   CDib * pDib        - ָ��CDib���ָ��
 *   CFile& file        - Ҫ��ȡ���ļ�
 *
 * ����ֵ:
 *   BOOL               - �ɹ�����TRUE
 *
 * ˵��:
 *   �ú������ƶ���ͼ�����λƽ��ֽ�
 *
 *************************************************************************/
BOOL WINAPI DIBBITPLANE(CDib *pDib,BYTE bBitNum)
{
	// ����������������Ҫ�󣬲����зֽ�
	if(bBitNum <1 || bBitNum >8)
		return FALSE;
	
	// ָ��Դͼ���ָ��
	BYTE *	lpSrc;

	//ͼ��Ŀ�Ⱥ͸߶�
	LONG    lWidth;
	LONG    lHeight;

	// ͼ��ÿ�е��ֽ���
	LONG	lLineBytes;
	
	// ѭ������
	LONG	i;
	LONG	j;

	// �м����
	BYTE bTemp;
	BYTE bA;

	// �����Ƶ�iλ��Ӧ��ʮ����ֵ
	BYTE bCount;
				
	//�õ�ͼ��Ŀ�Ⱥ͸߶�
	CSize   SizeDim;
	SizeDim = pDib->GetDimensions();
	lWidth  = SizeDim.cx;
	lHeight = SizeDim.cy;	
	
	//�õ�ʵ�ʵ�Dibͼ��洢��С
	CSize   SizeRealDim;
	SizeRealDim = pDib->GetDibSaveDim();

	// ����ͼ��ÿ�е��ֽ���
	lLineBytes = SizeRealDim.cx;
	
	//ͼ�����ݵ�ָ��
	LPBYTE  lpDIBBits = pDib->m_lpImage;	
	
	bCount = 1<<(bBitNum - 1);
	
	for (i = 0; i<lHeight; i++)
	{
		for ( j = 0; j < lWidth; j++)
		{
			// ָ��λͼi��j�е�ָ��
			lpSrc = (BYTE *)lpDIBBits + j+lLineBytes * (lHeight - 1 - i);

			// ��λͼ���Ҷ�����зֽ�
			if(bBitNum == 8)
			{
				// ��������λͼ8��ֱ��ȡ��ֵ�ĵ�8λ
				bTemp = (*(lpSrc) & bCount) / bCount;
				bTemp = bTemp * 255;
			}
			else
			{
				// ������������ûҶ���
				bTemp = (*(lpSrc) & bCount) / bCount;
				bA    = bCount * 2;

				// ��i��1λͼ������ֵ
				bA = (*(lpSrc) & bA) / bA;

				// ���
				bTemp = (bTemp ^ bA) * 255;
			}

			// ����Դͼ��
			*(lpSrc) = bTemp;	
		}
	}

	// ����ֵ
	return TRUE;
}
