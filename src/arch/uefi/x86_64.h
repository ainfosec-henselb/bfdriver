

#ifndef HYP_X86_64_H
#define HYP_X86_64_H


#include <stdint.h>

typedef union _KGDTENTRY64
{
    struct
    {
        UINT16 LimitLow;
        UINT16 BaseLow;
        union
        {
            struct
            {
                UINT8 BaseMiddle;
                UINT8 Flags1;
                UINT8 Flags2;
                UINT8 BaseHigh;
            } Bytes;
            struct
            {
                UINT32 BaseMiddle : 8;
                UINT32 Type : 5;
                UINT32 Dpl : 2;
                UINT32 Present : 1;
                UINT32 LimitHigh : 4;
                UINT32 System : 1;
                UINT32 LongMode : 1;
                UINT32 DefaultBig : 1;
                UINT32 Granularity : 1;
                UINT32 BaseHigh : 8;
            } Bits;
        };
        UINT32 BaseUpper;
        UINT32 MustBeZero;
    };
    struct
    {
        INT64 DataLow;
        INT64 DataHigh;
    };
} KGDTENTRY64, *PKGDTENTRY64;

#pragma pack(push,4)
typedef struct _KTSS64
{
    UINT32 Reserved0;
    UINT64 Rsp0;
    UINT64 Rsp1;
    UINT64 Rsp2;
    UINT64 Ist[8];
    UINT64 Reserved1;
    UINT16 Reserved2;
    UINT16 IoMapBase;
} KTSS64, *PKTSS64;
#pragma pack(pop)

typedef struct _KDESCRIPTOR
{
    UINT16 Pad[3];
    UINT16 Limit;
    void* Base;
} KDESCRIPTOR, *PKDESCRIPTOR;

void _writeeflags(int64_t);
int64_t _readeflags();
void _sgdt(void*);
void _lgdt(void*);
void _ltr(int16_t);
uint16_t _str();


EFI_STATUS PrepareProcessor();
EFI_STATUS CheckProcessor();

#endif
