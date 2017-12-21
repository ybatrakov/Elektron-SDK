/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#ifndef __RTR_RSSL_SOCKET_TRANSPORT_IMPL_H
#define __RTR_RSSL_SOCKET_TRANSPORT_IMPL_H

/* Contains function declarations necessary for the
 * bi-directional socket (and HTTP/HTTPS) connection type 
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "rtr/cutildfltcbuffer.h"

#include "rtr/rsslTypes.h"
#include "rtr/rsslChanManagement.h"
#include "rtr/ripch.h"
#include "rtr/ripc_int.h"
#include "rtr/ripcutils.h"
#include "rtr/custmem.h"
#include "rtr/rwfNet.h"
#include "rtr/rwfNetwork.h"
#include "rtr/rtrpipe.h"
#include "rtr/application_signing.h"
#include "rtr/cutilsmplcbuffer.h"
#include <stdio.h>


rtr_mutex		ripcMutex;
rtr_qtool_list	activeSessionList;


#define RSSL_RSSL_SOCKET_IMPL_FAST(ret)		ret RTR_FASTCALL

#define RIPC_RWF_PROTOCOL_TYPE 0 /* TODO must match definition for RWF in RSSL */
#define RIPC_TRWF_PROTOCOL_TYPE 1 /* TODO must match definition for TRWF in RSSL */

/* Current number of bytes in the compression bitmap */
#define RIPC_COMP_BITMAP_SIZE 1

#define RIPC_SOCKET_TRANSPORT   0
#define RIPC_OPENSSL_TRANSPORT  1
#define RIPC_WININET_TRANSPORT  2
#define RIPC_EXT_LINE_SOCKET_TRANSPORT 5
#define RIPC_MAX_TRANSPORTS     RIPC_EXT_LINE_SOCKET_TRANSPORT + 1
#define RIPC_MAX_SSL_PROTOCOLS  3		/* TLSv1, TLSv1.1, TLSv1.2 */

typedef enum {
	RIPC_SSL_TLS_V1 = 0,
	RIPC_SSL_TLS_V1_1 = 1,
	RIPC_SSL_TLS_V1_2 = 2
} ripcSSLProtocolIndex;

#define IPC_MUTEX_LOCK(session) \
	{					\
		rtr_mutex_error mterr = 0; \
		if ((session)->mutex) \
		RTR_MUTEX_LOCK(*((session)->mutex), mterr); \
	}

#define IPC_MUTEX_UNLOCK(session) \
		{					\
		rtr_mutex_error mterr = 0; \
		if ((session)->mutex) \
		RTR_MUTEX_UNLOCK(*((session)->mutex),mterr); \
	}

typedef enum {
	RIPC_NO_TUNNELING = 0,
	RIPC_TUNNEL_INIT = 1,
	RIPC_TUNNEL_ACTIVE = 2,
	RIPC_TUNNEL_REMOVE_SESSION = 7
} ripcTunnelState;

typedef enum {
	RIPC_INT_WORK_NONE = 0,
	RIPC_INT_READ_THR = 0x01,
	RIPC_INT_SOCK_CLOSED = 0x02,
	RIPC_INT_SHTDOWN_PEND = 0x04
} ripcWorkState;

typedef enum {
	RIPC_INT_CS_FLAG_NONE = 0x0,
	RIPC_INT_CS_FLAG_BLOCKING = 0x1,
	RIPC_INT_CS_FLAG_TCP_NODELAY = 0x2,
	RIPC_INT_CS_FLAG_TUNNEL_NO_ENCRYPTION = 0x4
} ripcConnectSocketFlags;


typedef enum {
	RIPC_INT_ST_INACTIVE = 0,
	RIPC_INT_ST_READ_HDR = 1,
	RIPC_INT_ST_COMPLETE = 2,
	RIPC_INT_ST_ACTIVE = 3,
	RIPC_INT_ST_CONNECTING = 4,
	RIPC_INT_ST_ACCEPTING = 5,
	RIPC_INT_ST_WAIT_ACK = 6,
	RIPC_INT_ST_TRANSPORT_INIT = 11,
	RIPC_INT_ST_CLIENT_TRANSPORT_INIT = 12,
	RIPC_INT_ST_PROXY_CONNECTING = 13,
	RIPC_INT_ST_CLIENT_WAIT_PROXY_ACK = 14,
	RIPC_INT_ST_CLIENT_WAIT_HTTP_ACK = 15,  /* wininet client is waiting for our HTTP connection response with session ID */
	RIPC_INT_ST_WAIT_CLIENT_KEY = 16,  /* we have sent our server key exchange info, waiting on client side before we can go active */
	RIPC_INT_ST_SEND_CLIENT_KEY = 17	/* client is in the third phase of handshake, needs to have a state to know it should send the client key if interrupted. */
} ripcIntState;

#if defined(_WIN32) 
#define RIPC_INVALID_SOCKET INVALID_SOCKET
#define ripcValidSocket(s) ((s) != RIPC_INVALID_SOCKET)
#else
#define RIPC_INVALID_SOCKET -1
#define ripcValidSocket(s) ((s) >= 0)
#endif

typedef struct {
	rtr_qtool_list priorityQueue;
	RsslInt32 queueLength;
	rtr_msgb_t		*tempList[RIPC_MAXIOVLEN + 1];
	RsslInt32	tempIndex;
} RIPC_PRIORITY_WRITE;

//typedef struct {
//	RsslSocket		stream;
//	char			*buff;
//	int				buffLength;
//	int				buffMaxLength;
//	void			*transportInfo; /* For normal cases, the transport is
//									* the OS calls read/write. For Secure
//									* Sockets we use SSLRead/SSLWrite calls.
//									*/
//} RIPC_TRANS_SESSION;

typedef struct {
	RsslUInt32		connVersion;
	RsslUInt32		ipcVersion;
	RsslUInt16		dataHeaderLen;
	RsslUInt16		footerLen;
	RsslUInt16		firstFragHdrLen;
	RsslUInt16		subsequentFragHdrLen;
} RIPC_SESS_VERS;

RIPC_SESS_VERS ripc10Ver;
RIPC_SESS_VERS ripc11Ver;
RIPC_SESS_VERS ripc11WinInetVer;
RIPC_SESS_VERS ripc12Ver;
RIPC_SESS_VERS ripc12WinInetVer;
RIPC_SESS_VERS ripc13Ver;
RIPC_SESS_VERS ripc13WinInetVer;
RIPC_SESS_VERS ripc14Ver;
RIPC_SESS_VERS ripc14WinInetVer;

typedef enum {
	RIPC_PROTO_SSL_NONE = 0,
	RIPC_PROTO_SSL_TLS_V1 = 0x1,
	RIPC_PROTO_SSL_TLS_V1_1 = 0x2,
	RIPC_PROTO_SSL_TLS_V1_2 = 0x4
} ripcSSLProtocolFlags;


typedef struct {

	int(*bindSrvr)(rsslServerImpl *srvr, RsslError *error);
	/*  Binds the server with the options set in the USER_SERVER structure,
	*	and opens up a listening port.
	*	Returns: < = on failure
	*			 = 0 on success
	*/

	void*   (*newSrvrConnection)(void * srvr, RsslSocket fd, int *initComplete, void* userSpecPtr, RsslError* error);
	/* A new server connection has been setup. Returns
	* the new transport information. Return of 0 implies
	* a failure. Upon successful return, initComplete tells
	* if the transport initialization is complete. If not,
	* the initializeTransport func must be called in order
	* to finish the transport initialization.
	*/

	RsslSocket(*connectSocket)(int *portnum, void *opts, int flags, void** userSpecPtr, RsslError *error);
	/* Creates the socket and returns the file descriptor -
	* for WinInet this will create the pipe and return it.
	* The userSpecPtr is for returning some additonal piece of information
	* that should be passed in on the newClientConnection call
	*/

	void*   (*newClientConnection)(RsslSocket fd, int *initComplete, void* userSpecPtr, RsslError* error);
	/* A new client connection has been setup. Returns
	* the new transport information. Return of 0 implies
	* a failure. Upon successful return, initComplete tells
	* if the transport initialization is complete. If not,
	* the initializeTransport func must be called in order
	* to finish the transport initialization.
	*/

	int(*initializeTransport)(void *transport, ripcSessInProg *inPr, RsslError *error);
	/* Continue with transport initialization.
	* Returns : < 0 on failure.
	*           = 0 on success, but transport init not complete.
	*           > 0 on success, transport init complete.
	*/

	int(*shutdownTransport)(void *transport);
	/* Shutdown the transport */

	int(*readTransport)(void *transport, char *buf, int max_len, ripcRWFlags flags, RsslError *error);
	/* Read from the transport.
	* Returns : < 0 on failure.
	*           = 0 on ewouldblock.
	*           > 0 is the number of bytes successfully read.
	*/

	int(*writeTransport)(void *transport, char *buf, int outLen, ripcRWFlags flags, RsslError *error);
	/* Write to the transport.
	* Returns : < 0 on failure.
	*           = 0 on ewouldblock.
	*           > 0 is the number of bytes successfully written.
	*/

	int(*writeVTransport)(void *transport, ripcIovType *iov,
		int iovcnt, int outLen, ripcRWFlags flags, RsslError *error);
	/* Write vector to the transport. This function may not be supported
	* Returns : < 0 on failure.
	*           = 0 on ewouldblock.
	*           > 0 is the number of bytes successfully written.
	*/

	int(*reconnectClient)(void *transport, RsslError *error);
	/* used for tunneling solutions to reconnect and bridge connections -
	this will keep connections alive through proxy servers */

	// Remove the second parameter from RsslSocket (*acceptSocket)(	RIPC_USER_SERVER *usrvr, RIPC_SRECV_OPTS *opts, void** userSpecPtr, RIPC_ERROR *error);
	RsslSocket(*acceptSocket)(rsslServerImpl *srvr, void** userSpecPtr, RsslError *error);

	void(*shutdownSrvrError)(rsslServerImpl* srvr);
	/* Used to shutdown a server if an error happens during initialization */

	int(*sessIoctl)(void *transport, int code, int value, RsslError *error);
	/* used for tunneling solutions to modify select options */

	int (*getSockName)(RsslSocket fd, struct sockaddr *address, int *address_len, void* transport);
	int (*setSockOpts)(RsslSocket fd, ripcSocketOption *option, void* transport);
	int (*getSockOpts)(RsslSocket fd, int code, int* value, void* transport, RsslError *error);
	int (*connected)(RsslSocket fd, void* transport);
	int (*shutdownServer)(void *srvr);
	void (*uninitialize)();
} ripcTransportFuncs;

typedef struct {
	char	*next_in;			/* Buffer to compress */
	size_t	avail_in;			/* Number of bytes to compress */
	char	*next_out;			/* Buffer to place compressed data */
	size_t		avail_out;			/* Number of bytes left in compressed data buffer */

	int		bytes_in_used;		/* Upon return, the number of bytes used from next_in */
	int		bytes_out_used;		/* Upon return, the number of bytes used from next_out */
} ripcCompBuffer;

typedef struct {
	void*	(*compressInit)(int compressionLevel, RsslError*);
	void*	(*decompressInit)(RsslError*);
	void(*compressEnd)(void *compressInfo);
	void(*decompressEnd)(void *compressInfo);
	int(*compress)(void *compressInfo, ripcCompBuffer *buf, RsslError *error);
	int(*decompress)(void *compressInfo, ripcCompBuffer *buf, RsslError *error);
} ripcCompFuncs;

/* This structure represents the function entry points for the different
* Secure Sockets Layer implementations.
* -- OpenSSL Secure Sockets
*/
typedef struct RsslSSLSrvrFuncs {

	void*	(*newSSLServer)(RsslSocket fd, char *name, RsslError*);
	/* A new server has been created */

	int(*freeSSLServer)(void *server, RsslError*);
	/* Free the SSL server */

} ipcSSLFuncs;

ipcSSLFuncs*		getSSLTransFuncs();

typedef struct {
	char		*serverName;		/* portName or port number */
	char		*interfaceName;		/* Inteface hostName or ip address */
	RsslUInt32	maxMsgSize;			/* Maximum Message Size */
	RsslUInt32	maxUserMsgSize;		/* Maximum User Message Size without IPC header */
	RsslUInt32	maxGuarMsgs;		/* Guar. number output messages per session */
	RsslUInt32	maxNumMsgs;			/* Max number of output messages per session */
	RsslUInt32	numInputBufs;
	RsslUInt32  compressionSupported;	/* a bitmask of The types of compression supported by this server. 0 means no compression */
	RsslUInt32	zlibCompressionLevel;
	RsslBool	forcecomp;			/* Force compression */
	RsslBool	server_blocking;	/* Perform server blocking operations */
	RsslBool	session_blocking;	/* Perform session blocking operations */
	RsslBool	tcp_nodelay;		/* Disable Nagle Algorithm */
	RsslInt32	connType;			/* Controls the connection type */
	RsslUInt32	rsslFlags;			/* this flag keeps track of client to server and server to client ping*/
	RsslUInt8	pingTimeout; 		/* ping timeout - this is for rssl */
	RsslUInt8	minPingTimeout; 	/* minimum ping timeout - this is for rssl */
	RsslUInt8	majorVersion;		/* major version of RSSL */
	RsslUInt8	minorVersion;		/* minor version of RSSL */
	RsslUInt8	protocolType;
	rtr_bufferpool_t	*sharedBufPool;			/* Pointer to the buffer pool to be used */
	rtr_mutex	*mutex;
	RsslUInt32 sendBufSize;
	RsslUInt32 recvBufSize;
	RsslBool	mountNak;

	// Copied from RIPC_SERVER
	RsslSocket	stream;
	RsslChannelState	state;

	/* Different Transport information */
	void			*transportInfo; /* For normal cases, the transport is
									* the OS calls read/write. For Secure
									* Sockets we use SSLRead/SSLWrite calls.
									*/
} RsslServerSocketChannel;

#define RSSL_INIT_SERVER_SOCKET_Bind { 0, 0, 0, 0, 0, 0, 0, RSSL_COMP_NONE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

RTR_C_INLINE void rsslClearRsslServerSocketChannel(RsslServerSocketChannel *rsslServerSocketChannel)
{
	rsslServerSocketChannel->serverName = 0;
	rsslServerSocketChannel->interfaceName = 0;
	rsslServerSocketChannel->maxMsgSize = 0;
	rsslServerSocketChannel->maxUserMsgSize = 0;
	rsslServerSocketChannel->maxGuarMsgs = 0;
	rsslServerSocketChannel->maxNumMsgs = 0;
	rsslServerSocketChannel->numInputBufs = 0;
	rsslServerSocketChannel->compressionSupported = RSSL_COMP_NONE;
	rsslServerSocketChannel->zlibCompressionLevel = 0;
	rsslServerSocketChannel->forcecomp = 0;
	rsslServerSocketChannel->server_blocking = 0;
	rsslServerSocketChannel->session_blocking = 0;
	rsslServerSocketChannel->tcp_nodelay = 0;
	rsslServerSocketChannel->connType = 0;
	rsslServerSocketChannel->rsslFlags = 0;
	rsslServerSocketChannel->pingTimeout = 0;
	rsslServerSocketChannel->minPingTimeout = 0;
	rsslServerSocketChannel->majorVersion = 0;
	rsslServerSocketChannel->minorVersion = 0;
	rsslServerSocketChannel->protocolType = 0; // RIPC_RWF_PROTOCOL_TYPE;
	rsslServerSocketChannel->sharedBufPool = 0;
	rsslServerSocketChannel->mutex = 0;
	rsslServerSocketChannel->sendBufSize = 0;
	rsslServerSocketChannel->recvBufSize = 0;
	rsslServerSocketChannel->transportInfo = 0;
	rsslServerSocketChannel->mountNak = 0;
}

RTR_C_INLINE void freeRsslServerSocketChannel(RsslServerSocketChannel* rsslServerSocketChannel)
{
	if (rsslServerSocketChannel)
	{
		rtr_mutex_error mterr = 0;

		if (multiThread)
			IPC_MUTEX_LOCK(rsslServerSocketChannel);

		if (rsslServerSocketChannel->sharedBufPool)
		{
			rtrBufferPoolDropRef(rsslServerSocketChannel->sharedBufPool);
			rsslServerSocketChannel->sharedBufPool = 0;
		}

		if (rsslServerSocketChannel->serverName != 0)
		{
			_rsslFree((void*)rsslServerSocketChannel->serverName);
			rsslServerSocketChannel->serverName = 0;
		}

		if (multiThread)
			IPC_MUTEX_UNLOCK(rsslServerSocketChannel);

		_rsslFree(rsslServerSocketChannel);
		rsslServerSocketChannel = 0;
	}
}


typedef struct
{
	char				*hostName;			/* hostName or ip address */
	char				*serverName;		/* portName or port number */
	char				*interfaceName;		/* local interface name to bind to */
	char				*objectName;		/* object name, used with tunneling */
	char				*proxyHostName;		/* hostname of the proxy */
	char				*proxyPort;			/* proxy port number */
	RsslBool			blocking : 1;		/* Perform blocking operations */
	RsslBool			tcp_nodelay : 1;	/* Disable Nagle Algorithm */
	RsslUInt32			compression;		/* Use compression defined by server, otherwise none */
	RsslUInt32			numConnections;		/* Number of concurrent connections for an extended line connection */
	RsslUInt32			numGuarOutputBufs;
	RsslUInt32			numMaxOutputBufs;
	RsslUInt32			numInputBufs;
	RsslUInt32			pingTimeout;
	RsslUInt32			rsslFlags;
	RsslUInt32			dbgFlags;
	RsslUInt32			connType;			/* Controls connection type. 0:Socket 1:ENCRYPTED 2:HTTP 3:ELSocket */
	RsslUInt8			majorVersion;
	RsslUInt8			minorVersion;
	RsslUInt8			protocolType;
	rtr_bufferpool_t		*bufPool;
	rtr_dfltcbufferpool_t	*guarBufPool;
	rtr_mutex			*mutex;
	RsslUInt32			sendBufSize;
	RsslUInt32			recvBufSize;

	RsslUInt32 			encryptionProtocolFlags;

	/* SSL/TLS Encryption information */
	ripcSSLProtocolFlags sslProtocolBitmap;		/* Represents the protocols supported by the dynamically loaded openSSL library */
	ripcSSLProtocolFlags sslCurrentProtocol;

	RsslInt32			ripcVersion;

	RsslSocket			newStream;

	RsslUInt32			mountNak : 1;

	RsslServerSocketChannel		*server;	/* The RsslServerSocketChannel for this channel*/
	RsslUInt32			sessionID;
	RsslUInt8			intState;
	RsslUInt8			workState;
	RsslUInt16			inDecompress;
	RsslCompTypes		outCompression;
	ripcCompFuncs		*inDecompFuncs;
	ripcCompFuncs		*outCompFuncs;
	void				*c_stream_in;
	void				*c_stream_out;		/* Compression stream information */

	RsslUInt32			httpHeaders : 1;
	RsslUInt32			isJavaTunnel : 1;	/* Is this a single-channel HTTP connection (i.e. from a java client) */
	RsslUInt32			sentControlAck : 1;

	RsslUInt32			zlibCompLevel;
	RsslUInt32			lowerCompressionThreshold;					// dont compress any buffers smaller than this
	RsslUInt32			upperCompressionThreshold;					// dont compress any buffers larger than this
	RsslUInt32			high_water_mark;
	RsslUInt32			safeLZ4 : 1;	/* limits LZ4 compression to only packets that wont span multiple buffers */

	ripcTransportFuncs	*transportFuncs; /* The transport functions to use */

	RsslUInt32			keyExchange : 1; /* indicates whether we should be exchanging encryption keys on the handshake */
	RsslUInt8			usingWinInet;	/* indicates whether we are using wininet - needed for proxy override */
	RsslUInt64			P;				/* key exchange p */
	RsslUInt64			G;				/* key exchange g */
	RsslUInt64			random_key;		/* calculated random key */
	RsslUInt64			shared_key;		/* calculated shared key */
	RsslUInt64			send_key;		/* key for sending; need to store on both sides */
	RsslUInt8			encryptionType;
	RIPC_PRIORITY_WRITE	priorityQueues[RIPC_MAX_PRIORITY_QUEUE];
	RsslInt8			flushStrategy[RIPC_MAX_FLUSH_STRATEGY + 1];  /* the flush strategy  */
	RsslInt32			currentOutList;		/* points into the flush strategy so we know which out list we are on */
	RsslInt8			compressQueue;      /* since we should only allow compression on one queue - or reinitialize
												to avoid potential zlib dictionary issues, we keep track of the first
												queue compression was done on and only allow it on that queue */
	RsslInt8			nextOutBuf;		/* used to keep track of next out buffer in case of partial write */

	rtr_msgb_t			*decompressBuf;
	rtr_msgb_t			*tempDecompressBuf;	/* temporary buffer to use when decompressing with compression types that dont effectively handle data growth (LZ4) */
	rtr_msgb_t			*tempCompressBuf;	/* temporary buffer to use when compressing with compression types that dont effectively handle data growth (LZ4) */
	RsslInt32			clientSession : 1;	/* identify if this is a client based session */
	RsslUInt32			maxMsgSize;
	RsslInt32			maxUserMsgSize;
	rtr_msgb_t			*inputBuffer;
	rtr_msgb_t			*curInputBuf;
	RsslInt32			inputBufCursor;
	RsslInt32			readSize;
	RsslUInt32			bytesOutLastMsg;	/* # of bytes in the last sent message */
	RIPC_SESS_VERS		*version;			/* Session version information */
	rtr_pipe			sessPipe;  // this is temporararily used for tunneling solutions for reconnection periods and initial handshake
	RsslUInt32			ipAddress;
	RsslUInt16			pID;				/* process ID for tunneling */
	char				tunnelingState;		/* used for basic state management of connection handshake for tunneling management */
	void				*tunnelStreamFd;	/* keeps track of the streamingFD for wininet based tunnel solutions */
	void				*newTunnelStreamFd; /* keeps track of streamingFD for winInet based tunnel solutions during the reconnect period */
	void				*oldTunnelStreamFd; /* keeps track of the previous streamingFD for wininet based tunnel solutions */
	RsslUInt8			compressionBitmap[RIPC_COMP_BITMAP_SIZE];	// why make an array of only 1 byte?
	RsslInt32			minPingTimeout;
	RsslUInt32			srvrcomp;			/* compression types allowed by the server */
	RsslUInt32			forcecomp : 1;

	//start from ripcSocket
	RsslSocket			stream;
	RsslSocket			oldStream;
	RsslChannelState	state;
	char				*clientHostname;
	char				*clientIP;
	char				*componentVer;		/* component version set by user - we dont own this memory */
	RsslUInt32			componentVerLen;
	char				*outComponentVer;	/* component version from wire - we own this memory */	
	RsslUInt32			outComponentVerLen;
	//end from ripcSocket

	/* Different Transport information */
	void				*transportInfo;		/* For normal cases, the transport is
											* the OS calls read/write. For Secure
											* Sockets we use SSLRead/SSLWrite calls.
											*/
} RsslSocketChannel;


RSSL_RSSL_SOCKET_IMPL_FAST(void) ripcClearRsslSocketChannel(RsslSocketChannel *rsslSocketChannel);

RTR_C_INLINE void ripcRelSocketChannel(RsslSocketChannel *rsslSocketChannel)
{
	rtr_mutex_error mterr=0;

	if (multiThread)
		RTR_MUTEX_LOCK(ripcMutex, mterr);
	if (rtr_qtool_headActive(&activeSessionList,rsslSocketChannel))
	{
		rtr_qtool_remqe(&activeSessionList,rsslSocketChannel);
	}

	if (rsslSocketChannel->curInputBuf)
	{
		rtr_smplcFreeMsg(rsslSocketChannel->curInputBuf);
		rsslSocketChannel->curInputBuf = 0;
	}

	if (rsslSocketChannel->inputBuffer)
	{
		rtr_smplcFreeMsg(rsslSocketChannel->inputBuffer);
		rsslSocketChannel->inputBuffer = 0;
	}

	if (rsslSocketChannel->decompressBuf)
	{
		rtr_smplcFreeMsg(rsslSocketChannel->decompressBuf);
		rsslSocketChannel->decompressBuf = 0;
	}

	if (rsslSocketChannel->tempCompressBuf)
	{
		rtr_smplcFreeMsg(rsslSocketChannel->tempCompressBuf);
		rsslSocketChannel->tempCompressBuf = 0;
	}

	if (rsslSocketChannel->tempDecompressBuf)
	{
		rtr_smplcFreeMsg(rsslSocketChannel->tempDecompressBuf);
		rsslSocketChannel->tempDecompressBuf = 0;
	}

//	ripcFreeOutputBufNew(rsslSocketChannel);

	if (rsslSocketChannel->c_stream_out && rsslSocketChannel->outCompFuncs)
		(*(rsslSocketChannel->outCompFuncs->compressEnd))(rsslSocketChannel->c_stream_out);

	if (rsslSocketChannel->c_stream_in && rsslSocketChannel->inDecompFuncs)
		(*(rsslSocketChannel->inDecompFuncs->decompressEnd))(rsslSocketChannel->c_stream_in);

	if (rsslSocketChannel->hostName != 0)
	{
		_rsslFree((void*)rsslSocketChannel->hostName);
		rsslSocketChannel->hostName = 0;
	}
	if (rsslSocketChannel->serverName != 0)
	{
		_rsslFree((void*)rsslSocketChannel->serverName);
		rsslSocketChannel->serverName = 0;
	}
	if (rsslSocketChannel->objectName != 0)
	{
		_rsslFree((void*)rsslSocketChannel->objectName);
		rsslSocketChannel->objectName = 0;
	}
	if (rsslSocketChannel->interfaceName != 0)
	{
		_rsslFree((void*)rsslSocketChannel->interfaceName);
		rsslSocketChannel->interfaceName = 0;
	}
	if (rsslSocketChannel->proxyHostName != 0)
	{
		_rsslFree((void*)rsslSocketChannel->proxyHostName);
		rsslSocketChannel->proxyHostName = 0;
	}
	if (rsslSocketChannel->proxyPort != 0)
	{
		_rsslFree((void*)rsslSocketChannel->proxyPort);
		rsslSocketChannel->proxyPort = 0;
	}

	rtr_pipe_close(&rsslSocketChannel->sessPipe);

	ripcClearRsslSocketChannel(rsslSocketChannel);
	/* dont clear sessionID here */
//	rtr_qtool_insqt(&freeSessionList,rsslSocketChannel);
	
	if (multiThread)
		RTR_MUTEX_UNLOCK(ripcMutex, mterr);
}

/* Session should be locked before call */
//static void ripcUserSessClose(RIPC_USER_SESSION *usersess)
RTR_C_INLINE void rsslSocketChannelClose(RsslSocketChannel *rsslSocketChannel)
{
//	RIPC_SESSION *sess = usersess->intsession;

	/* When we are setting up for renegotiation a new socket
	 * is created and the old is closed. We need to keep
	 * track of both in case of an error for feedback to user.
	 */
	if (rsslSocketChannel->newStream != RIPC_INVALID_SOCKET)
	{
		if (!(rsslSocketChannel->workState & RIPC_INT_SOCK_CLOSED))
		{
			if (rsslSocketChannel->tunnelingState != RIPC_TUNNEL_REMOVE_SESSION)
				sock_close(rsslSocketChannel->newStream);
			rsslSocketChannel->newStream = RIPC_INVALID_SOCKET;
			rsslSocketChannel->workState |= RIPC_INT_SOCK_CLOSED;
		}
	}
	
	if (rsslSocketChannel->stream != RIPC_INVALID_SOCKET)
	{
		if (!(rsslSocketChannel->workState & RIPC_INT_SOCK_CLOSED))
		{
			/* If we're in a proxy connecting state, transportInfo is 0.  Send the FD instead.  Don't cast as a pointer it since it's just getting cast to a RsslSocket. */
			if(rsslSocketChannel->intState == RIPC_INT_ST_PROXY_CONNECTING || rsslSocketChannel->intState == RIPC_INT_ST_CLIENT_WAIT_PROXY_ACK)
				(*(rsslSocketChannel->transportFuncs->shutdownTransport))(rsslSocketChannel->transportInfo);
			/* if we are doing tunneling, this state means we dont want to close the real fd */
			else if (rsslSocketChannel->tunnelingState != RIPC_TUNNEL_REMOVE_SESSION)
				(*(rsslSocketChannel->transportFuncs->shutdownTransport))(rsslSocketChannel->transportInfo);
			rsslSocketChannel->stream = RIPC_INVALID_SOCKET;
			rsslSocketChannel->workState |= RIPC_INT_SOCK_CLOSED;
		}
	}

	/* if we are tunneling, these may be set */
	if (rsslSocketChannel->tunnelStreamFd != 0)
	{
		if (rsslSocketChannel->tunnelingState != RIPC_TUNNEL_REMOVE_SESSION)
			(*(rsslSocketChannel->transportFuncs->shutdownTransport))(rsslSocketChannel->tunnelStreamFd);
		rsslSocketChannel->tunnelStreamFd = 0;
	}

	if (rsslSocketChannel->newTunnelStreamFd != 0)
	{
		if (rsslSocketChannel->oldTunnelStreamFd == rsslSocketChannel->newTunnelStreamFd)
			rsslSocketChannel->oldTunnelStreamFd = 0;
		if (rsslSocketChannel->tunnelingState != RIPC_TUNNEL_REMOVE_SESSION)
			(*(rsslSocketChannel->transportFuncs->shutdownTransport))(rsslSocketChannel->newTunnelStreamFd);
		rsslSocketChannel->newTunnelStreamFd = 0;
	}

	if(rsslSocketChannel->oldTunnelStreamFd != 0)
	{
//		if (rtrUnlikely(conndebug))
//			printf("Cleanup old reconnect Stream FD: %i\n", rsslSocketChannel->oldTunnelStreamFd);

		(*(rsslSocketChannel->transportFuncs->shutdownTransport))(rsslSocketChannel->oldTunnelStreamFd);
		rsslSocketChannel->oldTunnelStreamFd = 0;
	}

	if (!(rsslSocketChannel->workState & RIPC_INT_SHTDOWN_PEND))
		rsslSocketChannel->workState |= RIPC_INT_SHTDOWN_PEND;

	if (rsslSocketChannel->state != RSSL_CH_STATE_INACTIVE)
		rsslSocketChannel->state = RSSL_CH_STATE_INACTIVE;
}

/* Contains code necessary for binding a listening socket */
RsslRet rsslSocketBind(rsslServerImpl* rsslSrvrImpl, RsslBindOptions *opts, RsslError *error );

/* Contains code necessary for accepting inbound socket connections to a listening socket */
rsslChannelImpl* rsslSocketAccept(rsslServerImpl *rsslSrvrImpl, RsslAcceptOptions *opts, RsslError *error);

/* Contains code necessary for creating an outbound socket connectioni */
RsslRet rsslSocketConnect(rsslChannelImpl* rsslChnlImpl, RsslConnectOptions *opts, RsslError *error);

/* Contains code necessary to reconnect socket connections and bridge data flow (mainly for HTTP/HTTPS)
RsslRet rsslSocketReconnect(rsslChannelImpl *rsslChnlImpl, RsslError *error); */

/* Contains code necessary for non-blocking socket connections (client or server side) to perform and complete handshake process */
RsslRet rsslSocketInitChannel(rsslChannelImpl* rsslChnlImpl, RsslInProgInfo *inProg, RsslError *error);

/* Contains code necessary to close a socket connection (client or server side) */
RsslRet rsslSocketCloseChannel(rsslChannelImpl* rsslChnlImpl, RsslError *error);

/* Contains code necessary to read from a socket connection (client or server side) */
RSSL_RSSL_SOCKET_IMPL_FAST(RsslBuffer*) rsslSocketRead(rsslChannelImpl* rsslChnlImpl, RsslReadOutArgs *readOutArgs, RsslRet *readRet, RsslError *error);

/* Contains code necessary to write/queue data going to a socket connection (client or server side) */
RSSL_RSSL_SOCKET_IMPL_FAST(RsslRet) rsslSocketWrite(rsslChannelImpl *rsslChnlImpl, rsslBufferImpl *rsslBufImpl, RsslWriteInArgs *writeInArgs, RsslWriteOutArgs *writeOutArgs, RsslError *error);

/* Contains code necessary to flush queued data to socket connection (client or server side) */
RSSL_RSSL_SOCKET_IMPL_FAST(RsslRet) rsslSocketFlush(rsslChannelImpl *rsslChnlImpl, RsslError *error);

/* Contains code necessary to obtain a buffer to put data in for writing to socket connection (client or server side) */
RSSL_RSSL_SOCKET_IMPL_FAST(rsslBufferImpl*) rsslSocketGetBuffer(rsslChannelImpl *rsslChnlImpl, RsslUInt32 size, RsslBool packedBuffer, RsslError *error);

/* Contains code necessary to release an unused/unsuccessfully written buffer to socket connection (client or server) */
RSSL_RSSL_SOCKET_IMPL_FAST(RsslRet) rsslSocketReleaseBuffer(rsslChannelImpl *rsslChnlImpl, rsslBufferImpl *rsslBufImpl, RsslError *error);

/* Contains code necessary to query number of used output buffers for socket connection (client or server side) */
RSSL_RSSL_SOCKET_IMPL_FAST(RsslRet) rsslSocketBufferUsage(rsslChannelImpl *rsslChnlImpl, RsslError *error);

/* Contains code necessary to query number of used buffers by the server (shared pool buffers typically (server only) */
RSSL_RSSL_SOCKET_IMPL_FAST(RsslRet) rsslSocketSrvrBufferUsage(rsslServerImpl *rsslSrvrImpl, RsslError *error);

/* Contains code necessary for buffer packing with socket connection buffer (client or server side) */
RSSL_RSSL_SOCKET_IMPL_FAST(RsslBuffer*) rsslSocketPackBuffer(rsslChannelImpl *rsslChnlImpl, rsslBufferImpl *rsslBufImpl, RsslError *error);

/* Contains code necessary to send a ping message (or flush queued data) on socket connection (client or server side) */
RSSL_RSSL_SOCKET_IMPL_FAST(RsslRet) rsslSocketPing(rsslChannelImpl *rsslChnlImpl, RsslError *error);

/* Contains code necessary to query socket channel for more detailed connection info (client or server side) */
RSSL_RSSL_SOCKET_IMPL_FAST(RsslRet) rsslSocketGetChannelInfo(rsslChannelImpl *rsslChnlImpl, RsslChannelInfo *info, RsslError *error);

/* Contains code necessary to query server for more detailed connection info (server only) */
RSSL_RSSL_SOCKET_IMPL_FAST(RsslRet) rsslSocketGetSrvrInfo(rsslServerImpl *rsslSrvrImpl, RsslServerInfo *info, RsslError *error);

/* Contains code necessary to do ioctl on a client (client side only) */
RSSL_RSSL_SOCKET_IMPL_FAST(RsslRet) rsslSocketIoctl(rsslChannelImpl *rsslChnlImpl, RsslIoctlCodes code, void *value, RsslError *error);

/* Contains code necessary to do ioctl on a server socket (server side only) */
RSSL_RSSL_SOCKET_IMPL_FAST(RsslRet) rsslSocketSrvrIoctl(rsslServerImpl *rsslSrvrImpl, RsslIoctlCodes code, void *value, RsslError *error);

/* Contains code necessary to cleanup the server socket channel */
RSSL_RSSL_SOCKET_IMPL_FAST(RsslRet) rsslCloseSocketSrvr(rsslServerImpl *rsslSrvrImpl, RsslError *error);

RsslInt32 ipcSessSetMode(RsslSocket sock_fd, RsslInt32 blocking, RsslInt32 tcp_nodelay, RsslError *error, RsslInt32 line);

RsslInt32 getProtocolNumber();

RsslUInt8 getConndebug();

#define IPC_NULL_PTR(ptr,func,ptrname,err) \
	(( ptr == 0 ) ? ipcNullPtr(func,ptrname,__FILE__,__LINE__,err) : 0 )

extern RsslRet ipcNullPtr(char*, char*, char*, int, RsslError*);
//extern void setRipcError(RIPC_ERROR*,RIPC_SOCKET*,ripcError,int);
extern rtr_msgb_t *ipcIntReadSess(RsslSocketChannel*, RsslRet*, int*, int*, int*, int*, int*, int*, RsslError*);
extern ipcSessInit ipcIntSessInit(RsslSocketChannel*, ripcSessInProg*, RsslError*);
extern RsslRet ipcFlushSession(RsslSocketChannel*, RsslError*);

//extern void ipcSessFail(RsslSocketChannel*, RsslError*, int, int);
//extern void ipcSessFail(RsslError*,int,int);

extern ipcSessInit ipcProcessHdr(RsslSocketChannel*, ripcSessInProg*, RsslError*, int cc);
extern ipcSessInit ipcWaitAck(RsslSocketChannel*, ripcSessInProg*, RsslError*, char*, int);
extern RsslRet ipcSetCompFunc(int, ripcCompFuncs*);
extern RsslRet ipcSetTransFunc(int, ripcTransportFuncs*);
extern RsslRet ipcSetSSLFuncs(ipcSSLFuncs*);
extern RsslRet ipcSetSSLTransFunc(int, ripcTransportFuncs*);

extern RsslRet ripcSetCompFunc(int, ripcCompFuncs*);
extern RsslRet ripcSetTransFunc(int, ripcTransportFuncs*);
extern RsslRet ripcSetSSLFuncs(ipcSSLFuncs*);
extern RsslRet ripcSetSSLTransFunc(int, ripcTransportFuncs*);

extern RsslRet ipcShutdownServer(RsslServerSocketChannel* socket, RsslError *error);
extern RsslRet ipcSrvrDropRef(RsslServerSocketChannel *rsslServerSocketChannel, RsslError *error);
extern void ipcCloseActiveSrvr(RsslServerSocketChannel *rsslServerSocketChannel);

// For compilation with others transport type. Will be removed
extern void setRipcError(RIPC_ERROR *error, RIPC_SOCKET *sck, ripcError serr, int syserr);

// Contains code necessary to set the debug func pointers for Socket transport
RsslRet rsslSetSocketDebugFunctions(
	void(*dumpIpcIn)(const char *functionName, char *buffer, RsslUInt32 length, RsslUInt64 opaque), 
	void(*dumpIpcOut)(const char *functionName, char *buffer, RsslUInt32 length, RsslUInt64 opaque),
	void(*dumpRsslIn)(const char *functionName, char *buffer, RsslUInt32 length, RsslSocket socketId),
	void(*dumpRsslOut)(const char *functionName, char *buffer, RsslUInt32 length, RsslSocket socketId),
	RsslError *error);

#ifdef __cplusplus
};
#endif


#endif
