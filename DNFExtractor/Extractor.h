#pragma once

#ifndef _EXTRACTOR_H_
#define _EXTRACTOR_H_

#include "stdafx.h"
#include <vector>
#include "zlib/zlib.h"
#pragma comment(lib, "zlib.lib")
#include "libpng/png.h"
#pragma comment(lib, "libpng15.lib")

#define MAX_FILENAME_LENGTH 256
#define MAX_HEADER_FLAG 16

#define ARGB_1555 0x0e
#define ARGB_4444 0x0f
#define ARGB_8888 0x10
#define ARGB_LINK 0x11

#define COMPRESS_ZLIB 0x06
#define COMPRESS_NONE 0x05

class CExtractor
{
public:
	struct NPK_Header
	{
		char flag[MAX_HEADER_FLAG]; // 文件标识 "NeoplePack_Bill"
		int count;     // 包内文件的数目
	};

	struct NPK_Index
	{
		unsigned int offset;  // 文件的包内偏移量
		unsigned int size;    // 文件的大小
		char name[MAX_FILENAME_LENGTH];// 文件名
	};


	struct NImgF_Header
	{
		char flag[MAX_HEADER_FLAG]; // 文件标石"Neople Img File"
		int index_size;	// 索引表大小，以字节为单位
		int unknown1;
		int unknown2;
		int index_count;// 索引表数目
	};

	struct NPngF_Type
	{
		unsigned int dwType; //目前已知的类型有 0x0E(1555格式) 0x0F(4444格式) 0x10(8888格式) 0x11(不包含任何数据，可能是指内容同上一帧)
		unsigned int dwCompress; // 目前已知的类型有 0x06(zlib压缩) 0x05(未压缩)
	};

	struct NPngF_Info
	{
		int width;        // 宽度
		int height;       // 高度
		int size;         // 压缩时size为压缩后大小，未压缩时size为转换成8888格式时占用的内存大小
		int key_x;        // X关键点，当前图片在整图中的X坐标
		int key_y;        // Y关键点，当前图片在整图中的Y坐标
		int max_width;    // 整图的宽度
		int max_height;   // 整图的高度，有此数据是为了对齐精灵
	};

	struct NImgF_Index
	{
		unsigned int m_dwIsLink;
		unsigned long m_dwBufferSize;
		BYTE* m_pData;
		NPngF_Type m_stType;
		NPngF_Info m_stHeader;

		NImgF_Index()
		{
			m_dwIsLink = 0;
			m_dwBufferSize = 0;
			m_pData = NULL;
			memset(&m_stType, 0, sizeof(m_stType));
			memset(&m_stHeader, 0, sizeof(m_stHeader));
		}

		~NImgF_Index()
		{
			if (m_pData != NULL && m_dwIsLink == FALSE)
			{
				delete[] m_pData;
			}
			m_pData = NULL;
			m_dwIsLink = 0;
			m_dwBufferSize = 0;
			memset(&m_stType, 0, sizeof(m_stType));
			memset(&m_stHeader, 0, sizeof(m_stHeader));
		}
	};

	struct WriteInfo
	{
		BYTE* pBuffer;
		UINT dwMaxSize;
		UINT dwSize;
	};

	static char s_szFileNameFlag[MAX_FILENAME_LENGTH];
	static char s_szHeaderFlag[MAX_HEADER_FLAG];
	static char s_szImgFlag[MAX_HEADER_FLAG];
	static const UINT s_dwBufferSize = 1024 * 1024 * 3;

	CExtractor(CString szFileName);
	virtual ~CExtractor();

	bool IsLoad() const { return m_bIsLoad; }
	int GetImgCount() const { return m_vIndex.size(); }
	NPK_Index* GetImgByPos(int nPos);

	void SetActiveImg(int nPos);
	void LoadImgData(UINT dwOffset);

	int GetPngCount() const { return m_vPngIndex.size(); }
	NImgF_Index* GetPngByPos(int nPos);

	int NpkToPng(void* pDestBuff, UINT& dwDestSize, const BYTE* pSrcBuff, UINT dwSrcBuff, int nWidth, int nHeight, int nType, int x = 0, int y = 0, int nDestWidth = 0, int nDestHeight = 0);
	static void pngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length);
	static void pngWriteFlush(png_structp png_ptr);

	HRESULT LoadImageFromBuffer(void* pBuffer, int nSize, CImage& img);

private:
	bool		m_bIsLoad;
	CFile		m_oFile;
	CString		m_szFilename;
	//NPK_Header	m_stHeader;
	NImgF_Header m_stActiveImgHeader;
	std::vector<NPK_Index>	m_vIndex;
	std::vector<NImgF_Index> m_vPngIndex;
};

#endif // _EXTRACTOR_H_