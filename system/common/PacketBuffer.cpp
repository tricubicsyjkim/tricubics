#include "PacketBuffer.h"

#include <stdlib.h>
#include <ctype.h>

#include <stdio.h>
#include <string.h>
//sean_050124 #include <stdarg.h>

//sean_050124
#include <cstdarg>
//#include <iostream>




#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

//sean_00124 #define SAFE_DELETE_ARR(x) { if(x) delete [] x;x=NULL; }
#define SAFE_DELETE_ARR(x) { if(x) delete [] x;x=nullptr; }


#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

CPacketBuffer::CPacketBuffer()
{
    m_nUsed = 0;
    m_nMaxSize = MAXIMUM_PACKAGESIZE;
    m_nRead = 0;
    
    m_pBuffer = new uint8_t[m_nMaxSize];
    memset(m_pBuffer, 0, m_nMaxSize);
}

CPacketBuffer::CPacketBuffer(unsigned int nSize)
{
    m_nUsed = 0;
    m_nMaxSize = nSize;
    m_nRead = 0;

    m_pBuffer = new uint8_t[m_nMaxSize];
    memset(m_pBuffer, 0, m_nMaxSize);
}
CPacketBuffer::~CPacketBuffer()
{
    SAFE_DELETE_ARR(m_pBuffer);
}

unsigned int CPacketBuffer::GetSize()
{
    return m_nMaxSize;
}

bool CPacketBuffer::SetSize(unsigned int nSize)
{    
    //    DBGLOG((ENABLE, "max->%d, setsize->%d\n", m_nMaxSize, nSize));
    
    if(m_nMaxSize>=nSize)
        return true;

    int nNewSize = nSize + (nSize%1024);

    uint8_t* pNewBuffer = new uint8_t[nNewSize];
    //sean_050124
    if(pNewBuffer == nullptr)
        return false;
    
    if(m_nUsed>0)
        memcpy(pNewBuffer, m_pBuffer, m_nUsed);
    
    SAFE_DELETE_ARR(m_pBuffer);
    
    m_pBuffer = pNewBuffer;
    m_nMaxSize = nNewSize;

    return true;
}

void CPacketBuffer::SetUsed(unsigned int nUsed)
{
    if(nUsed>m_nMaxSize) {
        SetSize(nUsed);
    }
    m_nUsed = nUsed;
}

uint8_t *CPacketBuffer::GetBuffer()
{
    return m_pBuffer;
}

uint8_t *CPacketBuffer::GetBuffer(unsigned int nPos)
{
    if(nPos>=m_nMaxSize)
        //sean_050124
        return nullptr;
    return (m_pBuffer+nPos);
}

void CPacketBuffer::AppendBuffer(const char* strFormat, ...)
{
    char sBuffer[4096] = { 0, };
    va_list args;
    //sean

    va_start(args, strFormat);
#ifdef WIN32
    vsnprintf_s(sBuffer, 4096, strFormat, args);
#else
    vsnprintf(sBuffer, 4096, strFormat, args);
#endif
    va_end(args);

    AddData(sBuffer, strlen(sBuffer));
}


bool CPacketBuffer::AddData(uint8_t data, int nPos)
{
    return AddData(&data, 1, nPos);
}

bool CPacketBuffer::AddData(unsigned int data, int nPos)
{
   //sean  return AddData((const uint8_t * const)(&data), sizeof(unsigned int), nPos);
    return AddData(reinterpret_cast<const uint8_t *>(&data), sizeof(unsigned int), nPos);
}


bool CPacketBuffer::AddData(unsigned short data, int nPos)
{
   //sean return AddData((const uint8_t * const)(&data), sizeof(unsigned short), nPos);
    return AddData(reinterpret_cast<const uint8_t *>(&data), sizeof(unsigned short), nPos);
}

/*
* Adds a stream of char to the buffer. 
*
*/
bool CPacketBuffer::AddData(const char *const pData, unsigned int nSize, int nPos)
{ 
    //sean return AddData((const uint8_t * const)(pData),nSize, nPos);
    return AddData(reinterpret_cast<const uint8_t *>(&pData),nSize, nPos);
}

/*
* Adds a stream of uint8_tS to the buffer.
*
*/
bool CPacketBuffer::AddData(const uint8_t *const pData, unsigned int nSize, int nPos)
{
    if(nSize<=0)
        return true;

    nPos = (nPos<0) ? m_nUsed:nPos;

    if(!SetSize(nPos+nSize))
        return false;
    
    memcpy(m_pBuffer + nPos, pData, nSize);
    
    m_nUsed = max(nSize+nPos, m_nUsed);
    
    return true;
}

bool CPacketBuffer::GetData(int nSize, uint8_t* pData, int nPos)
{
    if(nSize<=0)
        return true;

    nPos = (nPos==-1)? m_nRead:nPos;

    if((nPos+nSize)>=(int)m_nUsed)
        return false;

    memcpy(pData, GetBuffer(nPos), nSize);

    m_nRead = nPos + nSize;


    //	DBGLOG((ENABLE, "GetData: %d/%d\n", m_nRead, m_nUsed));

    return true;
}

void CPacketBuffer::EmptyUsed()
{
    m_nUsed = 0;
    m_nRead = 0;
}

void CPacketBuffer::Clear()
{
    EmptyUsed();
    memset(m_pBuffer, 0, GetSize());
}

///////////////////////////////////////////////////////////////
