
#include "WireProtocol_Message.h"
#include "Memory.h"

char *bufferPointer = (char *)(uint32_t)&ram_disk_start_address;
unsigned int ram_size = (uint32_t)&ram_disk_size;

static bool Init = false;

void InitTraceWireProtocol()
{
    memset(bufferPointer, '*', ram_size);
}

static uint8_t *pp[400];
static int counter = 0;

void TraceWireProtocol(WP_Message _inboundMessage, bool Out)
{
    char *outStart = "{";
    char *outEnd = "}";
    char *inStart = "[";
    char *inEnd = "]";
    char *start;
    char *end;
    if (!Init)
    {
        InitTraceWireProtocol();
        Init = true;
    }
    if (Out)
    {
        start = outStart;
        end = outEnd;
    }
    else
    {
        start = inStart;
        end = inEnd;
    }

    pp[counter++] = bufferPointer;

    int numWritten;

    if (Out)
    {
        numWritten = sprintf(bufferPointer, "{%OutCmd=0x%08X} ", _inboundMessage.m_header.m_cmd);
    }
    else
    {
        numWritten = sprintf(bufferPointer, "[%InCmd=0x%08X] ", _inboundMessage.m_header.m_cmd);
    }

    // int numWritten = sprintf(
    //     bufferPointer,
    //     "%scmd=0x%08X, flags=0x%08X, hCRC=0x%08X, pCRC=0x%08X, seq=0x%04X replySeq=0x%04X len=%d%s\n",
    //     start,
    //     _inboundMessage.m_header.m_cmd,
    //     _inboundMessage.m_header.m_flags,
    //     _inboundMessage.m_header.m_crcHeader,
    //     _inboundMessage.m_header.m_crcData,
    //     _inboundMessage.m_header.m_seq,
    //     _inboundMessage.m_header.m_seqReply,
    //     _inboundMessage.m_header.m_size,
    //     end);
    bufferPointer += numWritten;
}
