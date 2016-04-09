/*
Copyright (C) 2003 by Matthias Braun <matze@braunis.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include <cstdlib>
#include <string>
#include <stdexcept>
#include <ctype.h>

#include "ProxyServer.hpp"

void ProxyServer::makeBase64(std::string &base64, const std::string &str)
{
    static const char base64_chars[65]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const char *auth_ptr;
    char auth_ptr_dest[5];
                                                                                
    auth_ptr_dest[4]=0;
    base64="";
    for(auth_ptr=str.c_str(); *auth_ptr; auth_ptr+=3) {
        auth_ptr_dest[0]='=';
        auth_ptr_dest[1]='=';
        auth_ptr_dest[2]='=';
        auth_ptr_dest[3]='=';
                                                                                
        auth_ptr_dest[0]=base64_chars[(auth_ptr[0]&0xfc) >>2];
        auth_ptr_dest[1]=base64_chars[((auth_ptr[0]&0x3) <<4)|((auth_ptr[1]&0xf0)>>4)];
        if(auth_ptr[1]==0) { base64+=auth_ptr_dest; break; }
        auth_ptr_dest[2]=base64_chars[((auth_ptr[1]&0xf) <<2)|((auth_ptr[2]&0xc0)>>6)];
        if(auth_ptr[2]==0) { base64+=auth_ptr_dest; break; }
        auth_ptr_dest[3]=base64_chars[((auth_ptr[2]&0x3f))];
        base64+=auth_ptr_dest;
    }
}

void ProxyServer::getProxyConnect(std::stringstream &buffer,const std::string &serveraddress) {
    if(((const std::string &)proxyserver).size()>0) {
        buffer << "CONNECT " << serveraddress << " HTTP/1.0\r\n";
        if(((const std::string &)proxyserveruser).size()>0) {
            std::string base64;
            std::string userpass( ((const std::string &)proxyserveruser) +":"+((const std::string &)proxyserverpass) );
            makeBase64(base64, userpass);
            buffer << "Authorization: Basic " << base64 << "\r\n";
        }
        buffer << "\r\n";
    }
}

void
ProxyServer::sendProxyConnect(network::TCPSocket& socket,
        const std::string& serveraddress)
{
    std::stringstream buffer;

    getProxyConnect(buffer, serveraddress);
            
    std::string tbuf = buffer.str();
    socket.send(tbuf.c_str(), tbuf.size());
    int lfs=0;
    char buf[1024];
    char *b=buf;
    int line=0;
    while(1) {
        char ch=0;
        //XXX
        //if(socket.recv(&ch, 1)!=1)
            //break;
        *b++=ch;
        if(ch=='\r') 
            continue;
        if(ch=='\n') {
            *b=0;
            lfs++;
            b=buf; 
            if(line==0) {
                while(!isspace(*b) && *b) b++;
                while(isspace(*b)) b++;
                if(atoi(b)!=200) {
                    std::stringstream msg;
                    msg << "Error when authenticating at proxy: " << b;
                    throw std::runtime_error(msg.str());
                }
            }
            line++;
        }
        else { lfs=0; }
        if(lfs>=2) { break; }
    }
}
