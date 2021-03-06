//===============================================================================
// Copyright (c) 2014-2016  Advanced Micro Devices, Inc. All rights reserved.
//===============================================================================
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//  File Name:   Codec_BC7.cpp
//  Description: implementation of the CCodec_BC7 class
//
//////////////////////////////////////////////////////////////////////////////

#if !defined(_CODEC_BC6H_H_INCLUDED_)
#define _CODEC_BC6H_H_INCLUDED_

#include "Compressonator.h"
#include "Codec_DXTC.h"
#include "BC6H_Encode.h"
#include "BC6H_Decode.h"
#include "BC6H_library.h"

class CCodec_BC6H : public CCodec_DXTC  
{
public:
    CCodec_BC6H();
    ~CCodec_BC6H();

    virtual bool SetParameter(const CMP_CHAR* pszParamName, CMP_CHAR* sValue);
    virtual bool SetParameter(const CMP_CHAR* /*pszParamName*/, CMP_DWORD /*dwValue*/);
    virtual bool SetParameter(const CMP_CHAR* /*pszParamName*/, CODECFLOAT /*fValue*/);

    // Required interfaces
    virtual CodecError Compress             (CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc = NULL, DWORD_PTR pUser1 = NULL, DWORD_PTR pUser2 = NULL);
    virtual CodecError Compress_Fast        (CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc = NULL, DWORD_PTR pUser1 = NULL, DWORD_PTR pUser2 = NULL);
    virtual CodecError Compress_SuperFast   (CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc = NULL, DWORD_PTR pUser1 = NULL, DWORD_PTR pUser2 = NULL);
    virtual CodecError Decompress           (CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc = NULL, DWORD_PTR pUser1 = NULL, DWORD_PTR pUser2 = NULL);


private:
    // BC6H User configurable variables
    CMP_WORD     m_ModeMask;
    double          m_Quality;
    WORD            m_NumThreads;    
    bool            m_bIsSigned;
    bool            m_UsePatternRec;
    double          m_Exposure;

    // BC6H Internal status 
    BOOL     m_LibraryInitialized;
    BOOL     m_Use_MultiThreading;
    WORD     m_NumEncodingThreads;
    WORD     m_LiveThreads;
    WORD     m_LastThread;

    // BC6H Encoders and decoders: for encding use the interfaces below
    HANDLE*                m_EncodingThreadHandle;
    BC6HBlockEncoder*    m_encoder[BC6H_MAX_THREADS];
    BC6HBlockDecoder*    m_decoder;

    // Encoder interfaces
    CodecError    CInitializeBC6HLibrary();
    CodecError    CEncodeBC6HBlock(float  in[BC6H_BLOCK_PIXELS][BC6H_MAX_DIMENSION_BIG],BYTE *out);
    CodecError    CFinishBC6HEncoding(void);
};

#endif // !defined(_CODEC_DXT5_H_INCLUDED_)
