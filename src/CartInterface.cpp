#include "CartInterface.h"

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

using namespace boost::interprocess;

CartInterface::CartInterface()
{
    InitPointer(m_pSharedMemory);
    InitPointer(m_pCommandRegion);
    InitPointer(m_pROMRegion);
}

int CartInterface::Open()
{
    m_pSharedMemory = new shared_memory_object(open_or_create, "gearboy-cart-interface", read_write);
    m_pSharedMemory->truncate(m_CommandSize + m_ROMSize + m_RAMSize);

    m_pCommandRegion = new mapped_region(m_pSharedMemory, read_write, 0, m_CommandSize);
    m_pROMRegion = new mapped_region(m_pSharedMemory, read_only, m_CommandSize, m_ROMSize);
    m_pRAMRegion = new mapped_region(m_pSharedMemory, read_only, m_CommandSize + m_ROMSize, m_RAMSize);

    m_Command = static_cast<u8*>(m_pCommandRegion->get_address());
    m_ROM = static_cast<u8*>(m_pROMRegion->get_address());
    m_RAM = static_cast<u8*>(m_pRAMRegion->get_address());

    SendCommand(command_init, true);
    m_bAwaitRead = m_Command[m_indexData] != 0x00;

    return 0;
}

int CartInterface::Close()
{
    if (m_pSharedMemory)
    {
        SendCommand(command_quit);
        shared_memory_object::remove("gearboy-cart-interface");
    }

    return 0;
}

u8 CartInterface::Read(const u16 address) const
{
    SendAddress(address);
    SendCommand(command_read, m_bAwaitRead);

    if (address >= 0x8000) return m_RAM[address - 0xA000];

    return m_ROM[address];
}

void CartInterface::Write(const u16 address, const u8 data) const
{
    SendAddress(address);
    SendData(data);
    SendCommand(command_write, true);
}

void CartInterface::SendCommand(const Command command, bool await) const
{
    m_Command[m_indexIn] = command_none;
    m_Command[m_indexOut] = command;
    if (await) AwaitCommand(command);
}

void CartInterface::AwaitCommand(const Command command) const
{
    while (m_Command[m_indexIn] != command) {}
    m_Command[m_indexIn] = command_none;
}

void CartInterface::SendAddress(const u16 address) const
{
    m_Command[m_indexAddrHi] = static_cast<u8>(address >> 8);
    m_Command[m_indexAddrLo] = static_cast<u8>(address & 0xFF);
}

void CartInterface::SendData(const u8 data) const
{
    m_Command[m_indexData] = data;
}
