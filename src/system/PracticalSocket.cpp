/*
 *   C++ sockets on Unix and Windows
 *   Copyright (C) 2002
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 * USA
 */

#include "config.h"
#include "PracticalSocket.h"

std::string
itos (double i)
{
  ostringstream os;
  os << i;
  return os.str ();
}

#ifdef _MSC_VER
#include <winsock2.h> // For socket(), connect(), send(), and recv()
#include <ws2tcpip.h>
typedef int socklen_t;
typedef char raw_type; // Type used for raw data on this platform
#else
#include <sys/types.h>  // For data types
#include <sys/socket.h> // For socket(), connect(), send(), and recv()
#include <netdb.h>      // For gethostbyname()
#include <arpa/inet.h>  // For inet_addr()
#include <unistd.h>     // For close()
#include <netinet/in.h> // For sockaddr_in
#include <net/if.h>     // For ifconf e ifreq
#include <sys/ioctl.h>  // For ioctl()
typedef void raw_type; // Type used for raw data on this platform
#endif

using namespace std;

#ifdef _MSC_VER
static bool initialized = false;
#endif

// SocketException Code

GINGA_SYSTEM_BEGIN

SocketException::SocketException (const string &message,
                                  bool inclSysMsg) throw ()
    : userMessage (message)
{
  if (inclSysMsg)
    {
      userMessage.append (": ");
      userMessage.append (strerror (errno));
    }
}

SocketException::~SocketException () throw () {}

const char *
SocketException::what () const throw ()
{
  return userMessage.c_str ();
}

// Function to fill in address structure given an address and port
static void
fillAddr (const string &address, unsigned short port, sockaddr_in &addr)
{
  memset (&addr, 0, sizeof (addr)); // Zero out address structure
  addr.sin_family = AF_INET;        // Internet address

  hostent *host; // Resolve name
  if ((host = gethostbyname (address.c_str ())) == NULL)
    {
      // strerror() will not work for gethostbyname() and hstrerror()
      // is supposedly obsolete
      throw SocketException ("Failed to resolve name (gethostbyname())");
    }
  addr.sin_addr.s_addr = *((unsigned long *)host->h_addr_list[0]);

  addr.sin_port = htons (port); // Assign port in network byte order
}

// Socket Code

Socket::Socket (int type, int protocol) throw (SocketException)
{
#ifdef _MSC_VER
  if (!initialized)
    {
      WORD wVersionRequested;
      WSADATA wsaData;

      wVersionRequested = MAKEWORD (2, 0); // Request WinSock v2.0
      if (WSAStartup (wVersionRequested, &wsaData) != 0)
        { // Load WinSock DLL
          throw SocketException ("Unable to load WinSock DLL");
        }
      initialized = true;
    }
#endif

  // Make a new socket
  if ((sockDesc = socket (PF_INET, type, protocol)) < 0)
    {
      throw SocketException ("Socket creation failed (socket())", true);
    }
}

Socket::Socket (int sockDesc) { this->sockDesc = sockDesc; }

Socket::~Socket ()
{
#ifdef _MSC_VER
  ::closesocket (sockDesc);
#else
  ::close (sockDesc);
#endif
  sockDesc = -1;
}

string
Socket::getLocalAddress () throw (SocketException)
{
  sockaddr_in addr;
  unsigned int addr_len = sizeof (addr);

  if (getsockname (sockDesc, (sockaddr *)&addr, (socklen_t *)&addr_len) < 0)
    {
      throw SocketException (
          "Fetch of local address failed (getsockname())", true);
    }
  return inet_ntoa (addr.sin_addr);
}

unsigned short
Socket::getLocalPort () throw (SocketException)
{
  sockaddr_in addr;
  unsigned int addr_len = sizeof (addr);

  if (getsockname (sockDesc, (sockaddr *)&addr, (socklen_t *)&addr_len) < 0)
    {
      throw SocketException ("Fetch of local port failed (getsockname())",
                             true);
    }
  return ntohs (addr.sin_port);
}

void
Socket::setLocalPort (unsigned short localPort) throw (SocketException)
{
  // Bind the socket to its port
  sockaddr_in localAddr;
  memset (&localAddr, 0, sizeof (localAddr));
  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = htonl (INADDR_ANY);
  localAddr.sin_port = htons (localPort);

  if (bind (sockDesc, (sockaddr *)&localAddr, sizeof (sockaddr_in)) < 0)
    {
      throw SocketException ("Set of local port failed (bind())", true);
    }
}

void
Socket::setLocalAddressAndPort (
    const string &localAddress,
    unsigned short localPort) throw (SocketException)
{
  // Get the address of the requested host
  sockaddr_in localAddr;
  fillAddr (localAddress, localPort, localAddr);

  if (bind (sockDesc, (sockaddr *)&localAddr, sizeof (sockaddr_in)) < 0)
    {
      throw SocketException (
          "Set of local address and port failed (bind())", true);
    }
}

void
Socket::cleanUp () throw (SocketException)
{
#ifdef _MSC_VER
  if (WSACleanup () != 0)
    {
      throw SocketException ("WSACleanup() failed");
    }
#endif
}

unsigned short
Socket::resolveService (const string &service, const string &protocol)
{
  struct servent *serv; /* Structure containing service information */

  if ((serv = getservbyname (service.c_str (), protocol.c_str ())) == NULL)
    return atoi (service.c_str ()); /* Service is port number */
  else
    return ntohs (
        serv->s_port); /* Found port (network byte order) by name */
}

// CommunicatingSocket Code

CommunicatingSocket::CommunicatingSocket (int type, int protocol) throw (
    SocketException)
    : Socket (type, protocol)
{
}

CommunicatingSocket::CommunicatingSocket (int newConnSD)
    : Socket (newConnSD)
{
}

void
CommunicatingSocket::connect (
    const string &foreignAddress,
    unsigned short foreignPort) throw (SocketException)
{
  // Get the address of the requested host
  sockaddr_in destAddr;
  fillAddr (foreignAddress, foreignPort, destAddr);

  // Try to connect to the given port
  int aux = ::connect (sockDesc, (sockaddr *)&destAddr, sizeof (destAddr));
  if (aux < 0)
    {
      clog << "====================> CommunicatingSocket::connect errno="
           << strerror (errno) << endl;
      throw SocketException ("Connect failed (connect())", true);
    }
}

void
CommunicatingSocket::send (const void *buffer,
                           int bufferLen) throw (SocketException)
{
  if (::send (sockDesc, (raw_type *)buffer, bufferLen, 0) < 0)
    {
      throw SocketException ("Send failed (send())", true);
    }
}

int
CommunicatingSocket::recv (void *buffer,
                           int bufferLen) throw (SocketException)
{
  int rtn;
  if ((rtn = ::recv (sockDesc, (raw_type *)buffer, bufferLen, 0)) < 0)
    {
      throw SocketException ("Received failed (recv())", true);
    }

  return rtn;
}

string
CommunicatingSocket::getForeignAddress () throw (SocketException)
{
  sockaddr_in addr;
  unsigned int addr_len = sizeof (addr);

  if (getpeername (sockDesc, (sockaddr *)&addr, (socklen_t *)&addr_len) < 0)
    {
      throw SocketException (
          "Fetch of foreign address failed (getpeername())", true);
    }
  return inet_ntoa (addr.sin_addr);
}

unsigned short
CommunicatingSocket::getForeignPort () throw (SocketException)
{
  sockaddr_in addr;
  unsigned int addr_len = sizeof (addr);

  if (getpeername (sockDesc, (sockaddr *)&addr, (socklen_t *)&addr_len) < 0)
    {
      throw SocketException ("Fetch of foreign port failed (getpeername())",
                             true);
    }
  return ntohs (addr.sin_port);
}

// TCPSocket Code

TCPSocket::TCPSocket () throw (SocketException)
    : CommunicatingSocket (SOCK_STREAM, IPPROTO_TCP)
{
}

TCPSocket::TCPSocket (const string &foreignAddress,
                      unsigned short foreignPort) throw (SocketException)
    : CommunicatingSocket (SOCK_STREAM, IPPROTO_TCP)
{
  connect (foreignAddress, foreignPort);
}

TCPSocket::TCPSocket (int newConnSD) : CommunicatingSocket (newConnSD) {}

// TCPServerSocket Code

TCPServerSocket::TCPServerSocket (unsigned short localPort,
                                  int queueLen) throw (SocketException)
    : Socket (SOCK_STREAM, IPPROTO_TCP)
{
  setLocalPort (localPort);
  setListen (queueLen);
}

TCPServerSocket::TCPServerSocket (const string &localAddress,
                                  unsigned short localPort,
                                  int queueLen) throw (SocketException)
    : Socket (SOCK_STREAM, IPPROTO_TCP)
{
  setLocalAddressAndPort (localAddress, localPort);
  setListen (queueLen);
}

TCPSocket *
TCPServerSocket::accept () throw (SocketException)
{
  int newConnSD;
  if ((newConnSD = ::accept (sockDesc, NULL, 0)) < 0)
    {
      throw SocketException ("Accept failed (accept())", true);
    }

  return new TCPSocket (newConnSD);
}

void
TCPServerSocket::setListen (int queueLen) throw (SocketException)
{
  if (listen (sockDesc, queueLen) < 0)
    {
      throw SocketException ("Set listening socket failed (listen())",
                             true);
    }
}

// UDPSocket Code

UDPSocket::UDPSocket () throw (SocketException)
    : CommunicatingSocket (SOCK_DGRAM, IPPROTO_UDP)
{
  setBroadcast ();
  setNonBlocking (false);
}

UDPSocket::UDPSocket (unsigned short localPort) throw (SocketException)
    : CommunicatingSocket (SOCK_DGRAM, IPPROTO_UDP)
{
  setLocalPort (localPort);
  setBroadcast ();
  setNonBlocking (false);
}

UDPSocket::UDPSocket (const string &localAddress,
                      unsigned short localPort) throw (SocketException)
    : CommunicatingSocket (SOCK_DGRAM, IPPROTO_UDP)
{
  setLocalAddressAndPort (localAddress, localPort);
  setBroadcast ();
  setNonBlocking (false);
}

void
UDPSocket::setBroadcast ()
{
  // If this fails, we'll hear about it when we try to send.  This will
  // allow
  // system that cannot broadcast to continue if they don't plan to
  // broadcast
  int broadcastPermission = 1;
  setsockopt (sockDesc, SOL_SOCKET, SO_BROADCAST,
              (raw_type *)&broadcastPermission,
              sizeof (broadcastPermission));
}

void
UDPSocket::disconnect () throw (SocketException)
{
  sockaddr_in nullAddr;
  memset (&nullAddr, 0, sizeof (nullAddr));
  nullAddr.sin_family = AF_UNSPEC;

  // Try to disconnect
  if (::connect (sockDesc, (sockaddr *)&nullAddr, sizeof (nullAddr)) < 0)
    {
#ifdef _MSC_VER
      if (errno != WSAEAFNOSUPPORT)
        {
#else
      if (errno != EAFNOSUPPORT)
        {
#endif
          throw SocketException ("Disconnect failed (connect())", true);
        }
    }
}

void
UDPSocket::sendTo (const void *buffer, int bufferLen,
                   const string &foreignAddress,
                   unsigned short foreignPort) throw (SocketException)
{
  sockaddr_in destAddr;
  fillAddr (foreignAddress, foreignPort, destAddr);

  // Write out the whole buffer as a single message.
  if (sendto (sockDesc, (raw_type *)buffer, bufferLen, 0,
              (sockaddr *)&destAddr, sizeof (destAddr))
      != bufferLen)
    {
      throw SocketException ("Send failed (sendto())", true);
    }
}

int
UDPSocket::recvFrom (void *buffer, int bufferLen, string &sourceAddress,
                     unsigned short &sourcePort) throw (SocketException)
{
  sockaddr_in clntAddr;
  socklen_t addrLen = sizeof (clntAddr);
  int rtn;
#ifdef _MSC_VER
  if ((rtn = recvfrom (sockDesc, (raw_type *)buffer, bufferLen, 0,
                       (sockaddr *)&clntAddr, (socklen_t *)&addrLen))
      < 0)
    {
      throw SocketException ("Receive failed (recvfrom())", true);
    }
#else
  if ((rtn = recvfrom (sockDesc, (raw_type *)buffer, bufferLen,
                       this->BLOCKING_MODE, (sockaddr *)&clntAddr,
                       (socklen_t *)&addrLen))
      < 0)
    {
      throw SocketException ("Receive failed (recvfrom())", true);
    }
#endif

  sourceAddress = inet_ntoa (clntAddr.sin_addr);
  sourcePort = ntohs (clntAddr.sin_port);

  return rtn;
}

string
UDPSocket::getBroadcastAddress () throw (SocketException)
{
#ifdef _MSC_VER
  INTERFACE_INFO interfaceList[20];
  unsigned long nBytesReturned;
  if (WSAIoctl (sockDesc, SIO_GET_INTERFACE_LIST, 0, 0, &interfaceList,
                sizeof (interfaceList), &nBytesReturned, 0, 0)
      == SOCKET_ERROR)
    {
      throw SocketException ("getBroadcastAddress failed", true);
      return ""; // TODO: throw exception
    }

  int nNumInterfaces = nBytesReturned / sizeof (INTERFACE_INFO);

  for (int i = 0; i < nNumInterfaces; ++i)
    {
      cout << endl;

      sockaddr_in *pAddress;
      pAddress = (sockaddr_in *)&(interfaceList[i].iiBroadcastAddress);
      u_long nFlags = interfaceList[i].iiFlags;
      if ((nFlags & IFF_UP) && (nFlags & IFF_BROADCAST))
        {
          return string (inet_ntoa (pAddress->sin_addr));
        }
    }

#else
  struct ifconf interfaces;
  struct ifreq *netInterface;
  struct sockaddr_in *myAddr;
  int numOfInterfaces, result, i;
  string interfaceName;
  bool validInterface;
  char buffer[1024000];
  ///
  struct sockaddr_in domain_addr;
  int domain_addr_len;
  struct sockaddr_in broadcast_addr;
  socklen_t broadcast_addr_len;
  ///

  interfaces.ifc_len = sizeof (buffer);
  interfaces.ifc_buf = buffer;
  result = ioctl (sockDesc, SIOCGIFCONF, (char *)&interfaces);
  netInterface = interfaces.ifc_req;
  numOfInterfaces = interfaces.ifc_len / sizeof (struct ifreq);

  for (i = 0; i < numOfInterfaces; netInterface++)
    {
      interfaceName = netInterface->ifr_name;

      // TODO: Change the ATS bellow
      validInterface = ("lo" != interfaceName);

      if (validInterface && netInterface->ifr_addr.sa_family == AF_INET
          && (netInterface->ifr_flags & IFF_BROADCAST))
        {

          //			result = ioctl(sockDesc, SIOCGIFBRDADDR, (char
          //*)
          // netInterface);
          result = ioctl (sockDesc, SIOCGIFBRDADDR, netInterface);

          if (result >= 0)
            {
              myAddr = (struct sockaddr_in *)&(netInterface->ifr_broadaddr);
              return inet_ntoa (myAddr->sin_addr);
            }
        }
      i++;
    }
  throw SocketException ("getBroadcastAddress failed", true);
#endif
  return "";
}

unsigned int
UDPSocket::getLocalIPAddress () throw (SocketException)
{
#ifdef _MSC_VER
  INTERFACE_INFO interfaceList[20];
  unsigned long nBytesReturned;
  if (WSAIoctl (sockDesc, SIO_GET_INTERFACE_LIST, 0, 0, &interfaceList,
                sizeof (interfaceList), &nBytesReturned, 0, 0)
      == SOCKET_ERROR)
    {
      throw SocketException ("getLocalIPAddress failed", true);
      return 0;
    }

  int nNumInterfaces = nBytesReturned / sizeof (INTERFACE_INFO);

  sockaddr_in *pAddress;
  pAddress = (sockaddr_in *)&(interfaceList[0].iiAddress);
  if (nNumInterfaces == 1)
    return pAddress->sin_addr.S_un.S_addr;

  for (int i = 0; i < nNumInterfaces; ++i)
    {
      sockaddr_in *pAddress;
      pAddress = (sockaddr_in *)&(interfaceList[i].iiAddress);
      if (pAddress->sin_family == AF_INET
          && pAddress->sin_addr.S_un.S_addr != 16777343)
        {
          return (unsigned int)pAddress->sin_addr.S_un.S_addr;
        }

      /*int aux = pAddress->sin_addr.S_un.S_addr;
      std::string pAddress_str = itos(aux& 0xFF) + "." +
                                      itos((aux & 0xFF00) >> 8) + "." +
                                      itos((aux & 0xFF0000) >> 16) + "." +
                                      itos((aux & 0xFF000000) >> 24);
      clog << "==============> pAddress->sin_addr.S_un.S_addr="  << aux <<
      "which means" << pAddress_str << endl;
      clog << endl;*/
      if (pAddress->sin_family == AF_INET)
        {

          //			return (unsigned int)
          // pAddress->sin_addr.S_un.S_addr;
          /*
          #else
                                  return (unsigned int)
          pAddress->sin_addr.S_addr;

          #endif
          */
        }
    }
  return -1;
#else
  struct ifconf interfaces;
  struct ifreq *netInterface;
  struct sockaddr_in *myAddr;
  int numOfInterfaces, result, i;
  string interfaceName;
  bool validInterface;
  char buffer[1024000];

  interfaces.ifc_len = sizeof (buffer);
  interfaces.ifc_buf = buffer;
  result = ioctl (sockDesc, SIOCGIFCONF, (char *)&interfaces);
  netInterface = interfaces.ifc_req;
  numOfInterfaces = interfaces.ifc_len / sizeof (struct ifreq);

  for (i = 0; i < numOfInterfaces; netInterface++)
    {
      interfaceName = netInterface->ifr_name;

      // TODO: Change the ATS bellow
      validInterface = ("lo" != interfaceName);

      if (validInterface && netInterface->ifr_addr.sa_family == AF_INET
          && (netInterface->ifr_flags & IFF_BROADCAST))
        {

          result = ioctl (sockDesc, SIOCGIFADDR, netInterface);
          if (result >= 0)
            {
              myAddr = (struct sockaddr_in *)&(netInterface->ifr_addr);
              return (unsigned int)(myAddr->sin_addr.s_addr);
            }
        }
      i++;
    }
  throw SocketException ("getLocalIPAddress failed", true);
  return -1;
#endif
}

void
UDPSocket::setMulticastTTL (unsigned char multicastTTL) throw (
    SocketException)
{
  if (setsockopt (sockDesc, IPPROTO_IP, IP_MULTICAST_TTL,
                  (raw_type *)&multicastTTL, sizeof (multicastTTL))
      < 0)
    {
      throw SocketException ("Multicast TTL set failed (setsockopt())",
                             true);
    }
}

void
UDPSocket::joinGroup (const string &multicastGroup) throw (SocketException)
{
  struct ip_mreq multicastRequest;

  multicastRequest.imr_multiaddr.s_addr
      = inet_addr (multicastGroup.c_str ());
  multicastRequest.imr_interface.s_addr = htonl (INADDR_ANY);
  if (setsockopt (sockDesc, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                  (raw_type *)&multicastRequest, sizeof (multicastRequest))
      < 0)
    {
      throw SocketException ("Multicast group join failed (setsockopt())",
                             true);
    }
}

void
UDPSocket::leaveGroup (const string &multicastGroup) throw (SocketException)
{
  struct ip_mreq multicastRequest;

  multicastRequest.imr_multiaddr.s_addr
      = inet_addr (multicastGroup.c_str ());
  multicastRequest.imr_interface.s_addr = htonl (INADDR_ANY);
  if (setsockopt (sockDesc, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                  (raw_type *)&multicastRequest, sizeof (multicastRequest))
      < 0)
    {
      throw SocketException ("Multicast group leave failed (setsockopt())",
                             true);
    }
}

void
UDPSocket::setReuseAddr (bool reuse) throw (SocketException)
{
  int value = (reuse) ? 1 : 0;

  int ret = setsockopt (sockDesc, SOL_SOCKET, SO_REUSEADDR,
                        (raw_type *)&value, sizeof (value));

  if (ret < 0)
    {
      throw SocketException ("Set Reused Addr failed (setsockopt())", true);
    }
}

void
UDPSocket::setMulticastLoop (bool loop) throw (SocketException)
{
  int value = (loop) ? 1 : 0;

  int ret = setsockopt (sockDesc, IPPROTO_IP, IP_MULTICAST_LOOP,
                        (raw_type *)&value, sizeof (value));

  if (ret < 0)
    {
      throw SocketException ("Set Multicast loop failed (setsockopt())",
                             true);
    }
}

int
UDPSocket::select_t (int sec, int usec)
{
  int nfds;
  fd_set fdset;
  struct timeval tv_timeout;

  FD_ZERO (&fdset);
  FD_SET (sockDesc, &fdset);

  nfds = sockDesc + 1;
  tv_timeout.tv_sec = sec;
  tv_timeout.tv_usec = usec;

  return select (nfds, &fdset, NULL, NULL, &tv_timeout);
}

void
UDPSocket::setNonBlocking (bool nonblock)
{
#ifdef _MSC_VER
  u_long iMode = nonblock ? 1 : 0;
  ioctlsocket (sockDesc, FIONBIO, &iMode);
  this->BLOCKING_MODE = (nonblock ? 1 : 0);
#else
  this->BLOCKING_MODE = (nonblock ? MSG_DONTWAIT : 0);
#endif
}

GINGA_SYSTEM_END
