// Zip/Handler.h

#ifndef __ZIP_HANDLER_H
#define __ZIP_HANDLER_H

#include "../../../Common/DynamicBuffer.h"
#include "../../ICoder.h"
#include "../IArchive.h"

#include "../../Common/CreateCoder.h"

#include "ZipIn.h"
#include "ZipCompressionMode.h"

#ifdef COMPRESS_MT
#include "../../../Windows/System.h"
#endif

namespace NArchive {
namespace NZip {

class CHandler:
  public IInArchive,
#ifndef EXTRACT_ONLY
  public IOutArchive,
  public ISetProperties,
  PUBLIC_ISetCompressCodecsInfo
#endif
  public CMyUnknownImp
{
public:
  MY_QUERYINTERFACE_BEGIN2(IInArchive)
#ifndef EXTRACT_ONLY
  MY_QUERYINTERFACE_ENTRY(IOutArchive)
  MY_QUERYINTERFACE_ENTRY(ISetProperties)
  QUERY_ENTRY_ISetCompressCodecsInfo
#endif
  MY_QUERYINTERFACE_END
  MY_ADDREF_RELEASE

  INTERFACE_IInArchive(;)
#ifndef EXTRACT_ONLY
  INTERFACE_IOutArchive(;)

  STDMETHOD(SetProperties)(const wchar_t **names, const PROPVARIANT *values, Int32 numProperties);

  DECL_ISetCompressCodecsInfo
#endif

  CHandler();
private:
  CObjectVector<CItemEx> m_Items;
  CInArchive m_Archive;

  int m_Level;
  int m_MainMethod;
  UInt32 m_DicSize;
  UInt32 m_Algo;
  UInt32 m_NumPasses;
  UInt32 m_NumFastBytes;
  UInt32 m_NumMatchFinderCycles;
  bool m_NumMatchFinderCyclesDefined;

  bool m_IsAesMode;
  Byte m_AesKeyMode;

  bool m_WriteNtfsTimeExtra;
  bool m_ForseLocal;
  bool m_ForseUtf8;

  #ifdef COMPRESS_MT
  UInt32 _numThreads;
  #endif

  DECL_EXTERNAL_CODECS_VARS

  void InitMethodProperties()
  {
    m_Level = -1;
    m_MainMethod = -1;
    m_Algo =
    m_DicSize =
    m_NumPasses =
    m_NumFastBytes =
    m_NumMatchFinderCycles = 0xFFFFFFFF;
    m_NumMatchFinderCyclesDefined = false;
    m_IsAesMode = false;
    m_AesKeyMode = 3; // aes-256
    m_WriteNtfsTimeExtra = false;
    m_ForseLocal = false;
    m_ForseUtf8 = false;
    #ifdef COMPRESS_MT
    _numThreads = NWindows::NSystem::GetNumberOfProcessors();;
    #endif
  }
};

}}

#endif
