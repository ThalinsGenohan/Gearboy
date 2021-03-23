#ifndef CARTINTERFACE_H
#define CARTINTERFACE_H

#include "definitions.h"

namespace boost
{
    namespace interprocess
    {
        class shared_memory_object;
        class mapped_region;
    }
}

class CartInterface
{
public:
    enum Command : u8
    {
        command_none = 0x00,
        command_read = 0x01,
        command_write = 0x02,
        command_init = 0x08,
        command_quit = 0xff,
    };

    CartInterface();
    ~CartInterface()
    {
        Close();
    }

    void Init()
    {
        
    }

    int Open();
    int Close();
    u8 Read(u16 address) const;
    void Write(u16 address, u8 data) const;

private:
    const int m_indexIn = 1;
    const int m_indexOut = 0;
    const int m_indexAddrHi = 2;
    const int m_indexAddrLo = 3;
    const int m_indexData = 4;

    const size_t m_CommandSize = 5;
    const size_t m_ROMSize = 32 * 1024;
    const size_t m_RAMSize = 8 * 1024;

    boost::interprocess::shared_memory_object* m_pSharedMemory;
    boost::interprocess::mapped_region* m_pCommandRegion;
    boost::interprocess::mapped_region* m_pROMRegion;
    boost::interprocess::mapped_region* m_pRAMRegion;

    u8* m_Command;
    u8* m_ROM;
    u8* m_RAM;

    bool m_bAwaitRead;

    void SendCommand(Command command, bool await = false) const;
    void AwaitCommand(Command command) const;
    void SendAddress(u16 address) const;
    void SendData(u8 data) const;
};

#endif
