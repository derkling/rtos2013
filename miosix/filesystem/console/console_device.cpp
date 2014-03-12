/***************************************************************************
 *   Copyright (C) 2013 by Terraneo Federico                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   As a special exception, if other files instantiate templates or use   *
 *   macros or inline functions from this file, or you compile this file   *
 *   and link it with other works to produce a work based on this file,    *
 *   this file does not by itself cause the resulting work to be covered   *
 *   by the GNU General Public License. However the source code for this   *
 *   file must still be made available in accordance with the GNU General  *
 *   Public License. This exception does not invalidate any other reasons  *
 *   why a work based on this file might be covered by the GNU General     *
 *   Public License.                                                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#include "console_device.h"
#include <errno.h>
#include <interfaces/console.h> //FIXME: remove

namespace miosix {

//
// class ConsoleDevice
//

ssize_t ConsoleDevice::write(const void *data, size_t len)
{
    return len;
}

ssize_t ConsoleDevice::read(void *data, size_t len)
{
    return -EBADF;
}

off_t ConsoleDevice::lseek(off_t pos, int whence)
{
    switch(whence)
    {
        case SEEK_SET:
        case SEEK_CUR:
        case SEEK_END:
            return -EBADF;
        default:
            return -EINVAL;
    }
}

int ConsoleDevice::fstat(struct stat *pstat) const
{
    //TODO: st_dev and st_ino
    memset(pstat,0,sizeof(struct stat));
    pstat->st_mode=S_IFCHR | 0755;//crwxr-xr-x Character device
    pstat->st_nlink=1;
    pstat->st_blksize=0; //If zero means file buffer equals to BUFSIZ
    return 0;
}

int ConsoleDevice::isatty() const { return 1; }

void ConsoleDevice::IRQwrite(const char *str) {}

//
// class ConsoleAdapter
//

// FIXME temporary -- begin
ssize_t ConsoleAdapter::write(const void *data, size_t len)
{
    Console::write(reinterpret_cast<const char*>(data),len);
    return len;
}
ssize_t ConsoleAdapter::read(void *data, size_t len)
{
    char *d=reinterpret_cast<char*>(data);
    for(size_t i=0;i<len;i++) *d++=Console::readChar();
    return len;
}
void ConsoleAdapter::IRQwrite(const char* str)
{
    Console::IRQwrite(str);
    while(!Console::IRQtxComplete()) ;
}
// FIXME temporary -- end

//
// class TerminalDevice
//

TerminalDevice::TerminalDevice(intrusive_ref_ptr<FileBase> device)
        : FileBase(intrusive_ref_ptr<FilesystemBase>()), device(device),
          mutex(), echo(true), binary(false), skipNewline(false) {}

ssize_t TerminalDevice::write(const void *data, size_t len)
{
    if(binary) return device->write(data,len);
    //No mutex here to avoid blocking writes while reads are in progress
    const char *dataStart=static_cast<const char*>(data);
    const char *buffer=dataStart;
    const char *chunkstart=dataStart;
    //Try to write data in chunks, stop at every \n to replace with \r\n
    for(size_t i=0;i<len;i++,buffer++)
    {
        if(*buffer=='\n')
        {
            size_t chunklen=buffer-chunkstart;
            if(chunklen>0)
            {
                ssize_t r=device->write(chunkstart,chunklen);
                if(r<=0) return chunkstart==dataStart ? r : chunkstart-dataStart;
            }
            ssize_t r=device->write("\r\n",2);//Add \r\n
            if(r<=0) return chunkstart==dataStart ? r : chunkstart-dataStart;
            chunkstart=buffer+1;
        }
    }
    if(chunkstart!=buffer)
    {
        ssize_t r=device->write(chunkstart,buffer-chunkstart);
        if(r<=0) return chunkstart==dataStart ? r : chunkstart-dataStart;
    }
    return len;
}

ssize_t TerminalDevice::read(void *data, size_t len)
{
    if(binary)
    {
        ssize_t result=device->read(data,len);
        if(echo && result>0) device->write(data,result); //Ignore write errors
        return result;
    }
    Lock<FastMutex> l(mutex); //Reads are serialized
    char *dataStart=static_cast<char*>(data);
    char *buffer=dataStart;
    //Trying to be compatible with terminals that output \r, \n or \r\n
    //When receiving \r skipNewline is set to true so we skip the \n
    //if it comes right after the \r
    for(int i=0;i<static_cast<int>(len);i++,buffer++)
    {
        ssize_t r=device->read(buffer,1);
        if(r<=0) return buffer==dataStart ? r : buffer-dataStart;
        switch(*buffer)
        {
            case '\r':
                *buffer='\n';
                if(echo) device->write("\r\n",2);
                skipNewline=true;
                return i+1;    
            case '\n':
                if(skipNewline)
                {
                    skipNewline=false;
                    //Discard the \n because comes right after \r
                    i--; //Note that i may become -1, but it is acceptable.
                    buffer--;
                } else {
                    if(echo) device->write("\r\n",2);
                    return i+1;
                }
                break;
            case 0x7f: //Unix backspace
            case 0x08: //DOS backspace
            {
                int backward= i==0 ? 1 : 2;
                i-=backward; //Note that i may become -1, but it is acceptable.
                buffer-=backward;
                if(echo) device->write("\033[1D \033[1D",9);
                break;
            }
            default:
                skipNewline=false;
                if(echo) device->write(buffer,1);
        }
    }
    return len;
}

off_t TerminalDevice::lseek(off_t pos, int whence)
{
    return device->lseek(pos,whence);
}

int TerminalDevice::fstat(struct stat *pstat) const
{
    return device->fstat(pstat);
}

int TerminalDevice::isatty() const { return device->isatty(); }

int TerminalDevice::sync() { return device->sync(); }

//
// class DefaultConsole 
//

DefaultConsole& DefaultConsole::instance()
{
    static DefaultConsole singleton;
    return singleton;
}

void DefaultConsole::IRQset(intrusive_ref_ptr<ConsoleDevice> console)
{
    //Note: should be safe to be called also outside of IRQ as set() calls
    //IRQset()
    atomic_store(&this->console,console);
    #ifndef WITH_FILESYSTEM
    atomic_store(&terminal,
        intrusive_ref_ptr<TerminalDevice>(new TerminalDevice(console)));
    #endif //WITH_FILESYSTEM
}

DefaultConsole::DefaultConsole() : console(new ConsoleDevice)
#ifndef WITH_FILESYSTEM
, terminal(new TerminalDevice(console))
#endif //WITH_FILESYSTEM      
{}

} //namespace miosix
