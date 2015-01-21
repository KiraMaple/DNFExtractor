#include "stdafx.h"
#include <stdlib.h>
#include "Extractor.h"

char CExtractor::s_szFileNameFlag[MAX_FILENAME_LENGTH] = "puchikon@neople dungeon and fighter DNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNF";
char CExtractor::s_szHeaderFlag[MAX_HEADER_FLAG] = "NeoplePack_Bill";
char CExtractor::s_szImgFlag[MAX_HEADER_FLAG] = "Neople Img File";

CExtractor::CExtractor(CString szFilename)
{
	m_hMainWindow = AfxGetMainWnd()->GetSafeHwnd();
	m_bIsLoad = false;
	m_szFilename = "";
	//memset(&m_stHeader, 0, sizeof(m_stHeader));
	m_vIndex.clear();

	if (szFilename == "")
	{
		return;
	}

	if ( !(m_oFile.Open(szFilename, CFile::modeRead)))
	{
		MessageBox(m_hMainWindow, _T("Can not Open This File.\nThis File does not exist Or It is used by another application."), _T("Waning!"), MB_OK);
		return;
	}

	ULONGLONG ullFileSize = m_oFile.GetLength();
	if (ullFileSize > UINT_MAX)
	{
		MessageBox(m_hMainWindow, _T("Can not Open This File.\nThis File is too big."), _T("Waning!"), MB_OK);
		return;
	}
	UINT dwFileSize = (UINT)ullFileSize;
	NPK_Header stHeader;
	memset(&stHeader, 0, sizeof(stHeader));
	m_oFile.Read(&stHeader, sizeof(stHeader));

	if (strcmp(stHeader.flag, s_szHeaderFlag) != 0)
	{
		MessageBox(NULL, _T("This File is not a valid NPK file."), _T("Waning!"), MB_OK);
		return;
	}

	for (int i = 0; i < stHeader.count; ++i)
	{
		NPK_Index stIndex;
		m_oFile.Read(&stIndex, sizeof(stIndex));

		for (int i = 0; i < MAX_FILENAME_LENGTH; ++i) {
			stIndex.name[i] = stIndex.name[i] ^ s_szFileNameFlag[i];
		}

		m_vIndex.push_back(stIndex);
	}

	m_szFilename = szFilename;
	m_bIsLoad = true;
}


CExtractor::~CExtractor()
{
	m_bIsLoad = false;
	m_szFilename = "";
	m_vIndex.clear();
	m_vPngIndex.clear();
}

CExtractor::NPK_Index* CExtractor::GetImgByPos(int nPos)
{
	ASSERT(nPos < (int)m_vIndex.size());
	if ( nPos >= (int)m_vIndex.size() )
	{
		return NULL;
	}

	return &m_vIndex[nPos];
}

void CExtractor::SetActiveImg(int nPos)
{
	NPK_Index* pstIndex = GetImgByPos(nPos);
	if (pstIndex == NULL)
	{
		return;
	}

	LoadImgData(pstIndex->offset);
}

void CExtractor::LoadImgData( UINT dwOffset )
{
	m_oFile.Seek(dwOffset, CFile::begin);

	memset(&m_stActiveImgHeader, 0, sizeof(m_stActiveImgHeader));
	m_oFile.Read(&m_stActiveImgHeader, sizeof(m_stActiveImgHeader));

	if (strcmp(m_stActiveImgHeader.flag, s_szImgFlag) != 0)
	{
		MessageBox(m_hMainWindow, _T("This Img is invalid."), _T("Warning"), MB_OK);
		memset(&m_stActiveImgHeader, 0, sizeof(m_stActiveImgHeader));
		return;
	}

	m_vPngIndex.clear();

	NImgF_Index stPreIndex;

	for (int i = 0; i < m_stActiveImgHeader.index_count; i++)
	{
		NImgF_Index stIndex;

		m_oFile.Read(&stIndex.m_stType, sizeof(stIndex.m_stType));

		if (stIndex.m_stType.dwType == ARGB_LINK)
		{
			stIndex = stPreIndex;
			stIndex.m_dwIsLink = 1;
		}
		else
		{
			m_oFile.Read(&stIndex.m_stHeader, sizeof(stIndex.m_stHeader));
			stIndex.m_dwIsLink = 0;
		}

		m_vPngIndex.push_back(stIndex);

		stPreIndex = stIndex;
	}

	m_oFile.Seek(dwOffset + m_stActiveImgHeader.index_size + 32, CFile::begin);

	NImgF_Index* pPreIndex = NULL;
	std::vector<NImgF_Index>::iterator it;
	for (it = m_vPngIndex.begin(); it != m_vPngIndex.end(); it++)
	{
		NImgF_Index& stIndex = *it;

		if (stIndex.m_dwIsLink != FALSE)
		{
			if (pPreIndex != NULL)
			{
				stIndex.m_dwBufferSize = pPreIndex->m_dwBufferSize;
				stIndex.m_pData = pPreIndex->m_pData;
			}
			continue;
		}

		int nBufferSize = stIndex.m_stHeader.size;
		if (stIndex.m_stType.dwCompress == COMPRESS_NONE)
		{
			if (stIndex.m_stType.dwType == ARGB_8888)
			{
				nBufferSize = stIndex.m_stHeader.size;
			}
			else if (stIndex.m_stType.dwType == ARGB_1555 || stIndex.m_stType.dwType == ARGB_4444)
			{
				nBufferSize = stIndex.m_stHeader.size / 2;
			}

			stIndex.m_pData = new BYTE[nBufferSize];
			m_oFile.Read(stIndex.m_pData, nBufferSize);
			stIndex.m_dwBufferSize = nBufferSize;

			pPreIndex = &stIndex;
		}
		else if (stIndex.m_stType.dwCompress == COMPRESS_ZLIB)
		{
			BYTE* pFileBuffer = new BYTE[nBufferSize];
			BYTE* pUnzipBuffer = new BYTE[s_dwBufferSize];
			memset(pUnzipBuffer, 0, s_dwBufferSize);

			//stIndex.m_dwBufferSize = nBufferSize * ZLIB_EFFICIENCY;
			stIndex.m_dwBufferSize = s_dwBufferSize;
			m_oFile.Read(pFileBuffer, nBufferSize);
			int ret = uncompress(pUnzipBuffer, &stIndex.m_dwBufferSize, pFileBuffer, nBufferSize);
			ASSERT(ret == Z_OK);
			if (ret != Z_OK)
			{
				MessageBox(m_hMainWindow, _T("A File Uncompress Failed."), _T("Error"), MB_OK);
			}
			else
			{
				stIndex.m_pData = new BYTE[stIndex.m_dwBufferSize];
				memcpy(stIndex.m_pData, pUnzipBuffer, stIndex.m_dwBufferSize);
				pPreIndex = &stIndex;
			}

			delete[] pUnzipBuffer;
			delete[] pFileBuffer;
		}
		else
		{
			MessageBox(m_hMainWindow, _T("A File Compress Type is invalid."), _T("Error"), MB_OK);
			ASSERT(0);
		}
	}
}

CExtractor::NImgF_Index* CExtractor::GetPngByPos(int nPos)
{
	ASSERT(nPos < (int)m_vPngIndex.size());
	if (nPos >= (int)m_vPngIndex.size())
	{
		return NULL;
	}

	return &m_vPngIndex[nPos];
}

int CExtractor::NpkToPng(void* pDestBuff, UINT& dwDestSize, const BYTE* pSrcBuff, UINT dwSrcBuff, int nSrcWidth, int nSrcHeight, int nType, int x/*=0*/, int y/*=0*/, int nDestWidth/*=0*/, int nDestHeight/*=0*/)
{
	if (pDestBuff == NULL || pSrcBuff == NULL || dwDestSize == 0 || dwSrcBuff == 0 || dwDestSize < dwSrcBuff)
	{
		return -1;
	}

	nDestWidth = nDestWidth >= nSrcWidth ? nDestWidth : nSrcWidth;
	nDestHeight = nDestHeight >= nSrcHeight ? nDestHeight : nSrcHeight;

	png_structp pstPngPtr;
	png_infop pstPngHeader;
	png_bytep* pPngBuff;

	pstPngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!pstPngPtr)
	{
		return 1;
	}

	pstPngHeader = png_create_info_struct(pstPngPtr);
	if (!pstPngHeader)
	{
		png_destroy_write_struct(&pstPngPtr, NULL);
		return 1;
	}

	if (setjmp(png_jmpbuf(pstPngPtr)))
	{
		png_destroy_write_struct(&pstPngPtr, &pstPngHeader);
		return 1;
	}

	WriteInfo stWriteInfo;
	stWriteInfo.pBuffer = (BYTE*)pDestBuff;
	stWriteInfo.dwMaxSize = dwDestSize;
	stWriteInfo.dwSize = 0;

	png_set_write_fn(pstPngPtr, &stWriteInfo, CExtractor::pngWriteCallback, CExtractor::pngWriteFlush);

	if ( setjmp( png_jmpbuf(pstPngPtr) ) )
	{
		png_destroy_write_struct(&pstPngPtr, &pstPngHeader);
		return 1;
	}

	png_set_IHDR(pstPngPtr, pstPngHeader, nDestWidth, nDestHeight,
		8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(pstPngPtr, pstPngHeader);

	if (setjmp(png_jmpbuf(pstPngPtr)))
	{
		return 1;
	}

	pPngBuff = (png_bytep*)malloc(nDestHeight*sizeof(png_bytep));
	memset(pPngBuff, 0, nDestHeight*sizeof(png_bytep));

	for (int i = 0; i < nDestHeight; i++)
	{
		pPngBuff[i] = (png_bytep)malloc(sizeof(unsigned char)* 4 * nDestWidth);
		memset(pPngBuff[i], 0, sizeof(unsigned char)* 4 * nDestWidth);

		if (i < y || i > (y + nSrcHeight))
		{
			continue;
		}

		for (int j = 0; j < nDestWidth; ++j)
		{
			if (j < x || j > (x + nSrcWidth))
			{
				continue;;
			}
			// png is rgba
			switch (nType)
			{
			case ARGB_1555://1555
				pPngBuff[i][j * 4 + 0] = ((pSrcBuff[(i - y) * nSrcWidth * 2 + (j - x) * 2 + 1] & 127) >> 2) << 3;   // red  
				pPngBuff[i][j * 4 + 1] = (((pSrcBuff[(i - y) * nSrcWidth * 2 + (j - x) * 2 + 1] & 0x0003) << 3) |
					((pSrcBuff[(i - y) * nSrcWidth * 2 + (j - x) * 2] >> 5) & 0x0007)) << 3; // green  
				pPngBuff[i][j * 4 + 2] = (pSrcBuff[(i - y) * nSrcWidth * 2 + (j - x) * 2] & 0x003f) << 3; // blue 
				pPngBuff[i][j * 4 + 3] = (pSrcBuff[(i - y) * nSrcWidth * 2 + (j - x) * 2 + 1] >> 7) == 0 ? 0 : 255; // alpha
				break;
			case ARGB_4444://4444
				pPngBuff[i][j * 4 + 0] = (pSrcBuff[(i - y) * nSrcWidth * 2 + (j - x) * 2 + 1] & 0x0f) << 4;   // red  
				pPngBuff[i][j * 4 + 1] = ((pSrcBuff[(i - y) * nSrcWidth * 2 + (j - x) * 2 + 0] & 0xf0) >> 4) << 4; // green  
				pPngBuff[i][j * 4 + 2] = (pSrcBuff[(i - y) * nSrcWidth * 2 + (j - x) * 2 + 0] & 0x0f) << 4;; // blue  
				pPngBuff[i][j * 4 + 3] = ((pSrcBuff[(i - y) * nSrcWidth * 2 + (j - x) * 2 + 1] & 0xf0) >> 4) << 4; // alpha
				break;
			case ARGB_8888://8888
				pPngBuff[i][j * 4 + 0] = pSrcBuff[(i - y) * nSrcWidth * 4 + (j - x) * 4 + 2]; // red
				pPngBuff[i][j * 4 + 1] = pSrcBuff[(i - y) * nSrcWidth * 4 + (j - x) * 4 + 1]; // green
				pPngBuff[i][j * 4 + 2] = pSrcBuff[(i - y) * nSrcWidth * 4 + (j - x) * 4 + 0]; // blue
				pPngBuff[i][j * 4 + 3] = pSrcBuff[(i - y) * nSrcWidth * 4 + (j - x) * 4 + 3]; // alpha
				break;
			case ARGB_LINK:// 占位，无图片资源
				break;
			default:
				printf("error known type:%d\n", nType);
				break;
			}
		}
	}
	png_write_image(pstPngPtr, pPngBuff);

	if (setjmp(png_jmpbuf(pstPngPtr))) {
		return 1;
	}

	png_write_end(pstPngPtr, NULL);
	png_destroy_write_struct(&pstPngPtr, &pstPngHeader);

	dwDestSize = stWriteInfo.dwSize;

	for (int i = 0; i < nDestHeight; i++)
	{
		free(pPngBuff[i]);
	}
	free(pPngBuff);

	return 0;
}

void CExtractor::pngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length)
{
	/* with libpng15 next line causes pointer deference error; use libpng12 */
	WriteInfo* pstInfo = (WriteInfo*)png_get_io_ptr(png_ptr); /* was png_ptr->io_ptr */

	if (pstInfo->dwSize + length > pstInfo->dwMaxSize)
	{
		ASSERT(0);
		MessageBox(NULL, "Png Size is bigger than buffer.", "Error", MB_OK);
		return;
	}

	memcpy(pstInfo->pBuffer + pstInfo->dwSize, data, length);
	pstInfo->dwSize += length;
}

void CExtractor::pngWriteFlush(png_structp png_ptr)
{
	;
}

HRESULT CExtractor::LoadImageFromBuffer(void* pBuffer, int nSize, CImage& img)
{
	COleStreamFile osf;
	osf.CreateMemoryStream(NULL);
	osf.Write(pBuffer, nSize);
	osf.SeekToBegin();
	return img.Load(osf.GetStream());
}