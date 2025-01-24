#ifndef _PACKETBUFFER_H_
#define _PACKETBUFFER_H_

#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef WIN32
#include <wtypes.h>
#include <tchar.h>
#endif

#include <string>
#include <stdint.h>

#define MAXIMUM_PACKAGESIZE				65536		// 64k

class CPacketBuffer  
{
public:
    CPacketBuffer();
    CPacketBuffer(unsigned int nSize);
    ~CPacketBuffer();

    bool SetSize(unsigned int nSize);
    unsigned int GetSize();

    // Write different types of variabels to the buffer
    bool AddData(const uint8_t * pData,unsigned int nSize, int nPos = -1);
    //////////

    bool AddData(uint8_t data, int nPos = -1);
    bool AddData(unsigned int data, int nPos = -1);
    bool AddData(unsigned short data, int nPos = -1);
    bool AddData(const char * pData,unsigned int nSize, int nPos = -1);

    bool GetData(int nSize, uint8_t* pData, int nPos = -1);

    void AppendBuffer(const char* strFormat, ...);
    void AppendBuffer(int strFormat, ...);

    // returns A pointer to the Buffer
    uint8_t* GetBuffer();
    uint8_t* GetBuffer(unsigned int nPos);

    unsigned int GetUsed() { return m_nUsed; }
    void SetUsed(unsigned int nUsed);
    void EmptyUsed();
    void Clear();

    ////////////////////////////////////////////////////////

protected:

    // number of uint8_ts that are Used.
    unsigned int m_nUsed;
    unsigned int m_nMaxSize;
    unsigned int m_nRead;

    // The buffer.
    uint8_t *m_pBuffer;
};

#endif // _PACKETBUFFER_H_

