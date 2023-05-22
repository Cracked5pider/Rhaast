#pragma once

#include <Common.h>

/**
 * KSocket implementation taken from https://github.com/wbenny/KSOCKET/
 * full credit goes to the original author
 */

typedef int       socklen_t;
typedef intptr_t  ssize_t;

uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);

int getaddrinfo(const char* node, const char* service, const struct addrinfo* hints, struct addrinfo** res);
void freeaddrinfo(struct addrinfo* res);

int socket_connection(int domain, int type, int protocol);
int socket_listen(int domain, int type, int protocol);
int socket_datagram(int domain, int type, int protocol);
int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
int listen(int sockfd, int backlog);
int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
int send(int sockfd, const void* buf, size_t len, int flags);
int sendto(int sockfd, const void* buf, size_t len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen);
int recv(int sockfd, void* buf, size_t len, int flags);
int recvfrom(int sockfd, void* buf, size_t len, int flags, struct sockaddr* src_addr, socklen_t* addrlen);
int closesocket(int sockfd);

#define socket  socket_connection

typedef struct _KSOCKET KSOCKET, * PKSOCKET;

NTSTATUS
NTAPI
KsInitialize(
    VOID
);

VOID
NTAPI
KsDestroy(
    VOID
);

NTSTATUS
NTAPI
KsGetAddrInfo(
    _In_ PUNICODE_STRING NodeName,
    _In_ PUNICODE_STRING ServiceName,
    _In_ PADDRINFOEXW Hints,
    _Out_ PADDRINFOEXW* Result
);

VOID
NTAPI
KsFreeAddrInfo(
    _In_ PADDRINFOEXW AddrInfo
);

NTSTATUS
NTAPI
KsCreateSocket(
    _Out_ PKSOCKET* Socket,
    _In_ ADDRESS_FAMILY AddressFamily,
    _In_ USHORT SocketType,
    _In_ ULONG Protocol,
    _In_ ULONG Flags
);

NTSTATUS
NTAPI
KsCreateConnectionSocket(
    _Out_ PKSOCKET* Socket,
    _In_ ADDRESS_FAMILY AddressFamily,
    _In_ USHORT SocketType,
    _In_ ULONG Protocol
);

NTSTATUS
NTAPI
KsCreateListenSocket(
    _Out_ PKSOCKET* Socket,
    _In_ ADDRESS_FAMILY AddressFamily,
    _In_ USHORT SocketType,
    _In_ ULONG Protocol
);

NTSTATUS
NTAPI
KsCreateDatagramSocket(
    _Out_ PKSOCKET* Socket,
    _In_ ADDRESS_FAMILY AddressFamily,
    _In_ USHORT SocketType,
    _In_ ULONG Protocol
);

NTSTATUS
NTAPI
KsCloseSocket(
    _In_ PKSOCKET Socket
);

NTSTATUS
NTAPI
KsBind(
    _In_ PKSOCKET Socket,
    _In_ PSOCKADDR LocalAddress
);

NTSTATUS
NTAPI
KsAccept(
    _In_ PKSOCKET Socket,
    _Out_ PKSOCKET* NewSocket,
    _Out_opt_ PSOCKADDR LocalAddress,
    _Out_opt_ PSOCKADDR RemoteAddress
);

NTSTATUS
NTAPI
KsConnect(
    _In_ PKSOCKET Socket,
    _In_ PSOCKADDR RemoteAddress
);

NTSTATUS
NTAPI
KsSendRecv(
    _In_ PKSOCKET Socket,
    _In_ PVOID Buffer,
    _Inout_ PULONG Length,
    _In_ ULONG Flags,
    _In_ BOOLEAN Send
);

NTSTATUS
NTAPI
KsSendRecvUdp(
    _In_ PKSOCKET Socket,
    _In_ PVOID Buffer,
    _Inout_ PULONG Length,
    _In_ ULONG Flags,
    _In_ PSOCKADDR RemoteAddress,
    _In_ BOOLEAN Send
);

NTSTATUS
NTAPI
KsSend(
    _In_ PKSOCKET Socket,
    _In_ PVOID Buffer,
    _Inout_ PULONG Length,
    _In_ ULONG Flags
);

NTSTATUS
NTAPI
KsRecv(
    _In_ PKSOCKET Socket,
    _In_ PVOID Buffer,
    _Inout_ PULONG Length,
    _In_ ULONG Flags
);

NTSTATUS
NTAPI
KsSendTo(
    _In_ PKSOCKET Socket,
    _In_ PVOID Buffer,
    _Inout_ PULONG Length,
    _In_ ULONG Flags,
    _In_ PSOCKADDR RemoteAddress
);

NTSTATUS
NTAPI
KsRecvFrom(
    _In_ PKSOCKET Socket,
    _In_ PVOID Buffer,
    _Inout_ PULONG Length,
    _In_ ULONG Flags,
    _In_ PSOCKADDR RemoteAddress
);
