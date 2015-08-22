/*************************************************************************/
/*				Copyright (c) 2000-2010 NT Kernel Resources.		     */
/*                           All Rights Reserved.                        */
/*                          http://www.ntkernel.com                      */
/*                           ndisrd@ntkernel.com                         */
/*																		 */
/* Description: Main dll module											 */
/*************************************************************************/

#include "stdafx.h"

// pointer truncation
#pragma warning(disable:4311)
#pragma warning(disable:4312)

#define DEVICE_NDISWANIP "\\DEVICE\\NDISWANIP"
#define USER_NDISWANIP "WAN Network Interface (IP)"
#define DEVICE_NDISWANBH "\\DEVICE\\NDISWANBH"
#define USER_NDISWANBH "WAN Network Interface (BH)"
#define DEVICE_NDISWANIPV6 "\\DEVICE\\NDISWANIPV6"
#define USER_NDISWANIPV6 "WAN Network Interface (IPv6)"
#define REGSTR_VAL_CONNECTION "\\Connection"
#define REGSTR_NAME "Name"
#define REGSTR_VAL_SERVICE_NAME "ServiceName"
#define REGSTR_VAL_DRIVER_DESC "DriverDesc"
#define REGSTR_VAL_TITLE "Title"


#define REGSTR_NETWORK_CONTROL_KEY "SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\"
#define REGSTR_NETWORK_CARDS TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards")
#define REGSTR_MSTCP_CLASS_NET "SYSTEM\\CurrentControlSet\\Services\\Class\\Net\\"

#define OID_GEN_CURRENT_PACKET_FILTER			0x0001010E

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

//
// This is the constructor of a class that has been exported.
// see ndisapi.h for the class definition
//
CNdisApi::CNdisApi (const TCHAR* pszFileName)
{
	TCHAR pszFullName [ FILE_NAME_SIZE ];

	// Format full file name
	_tcscpy ( pszFullName, _T("\\\\.\\") );
	_tcscat ( pszFullName, pszFileName );

	m_bIsLoadSuccessfully = FALSE;
	 
	// We open driver for overlapped I/O, though none of current driver services are processed asynchronously
	m_hFileHandle = ::CreateFile ( pszFullName, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0 );
     
	// If driver is opened succesfully we initialize our OVERLAPPED structure
	if   ( m_hFileHandle == INVALID_HANDLE_VALUE )
	{
		m_ovlp.hEvent = 0;
	}
	else
	{
		m_ovlp.hEvent = CreateEvent ( 0, TRUE, FALSE, NULL );
		if ( m_ovlp.hEvent )
		{
			m_bIsLoadSuccessfully = TRUE;
		}
	}

	m_BytesReturned = 0;

	//
	// Check if we are running in WOW64
	//
	m_bIsWow64Process = FALSE;
	m_Version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&m_Version);
	if (((m_Version.dwMajorVersion == 5)&&(m_Version.dwMinorVersion >= 1))/*Windows XP/2003*/ || (m_Version.dwMajorVersion > 5)/*Windows Vista or later*/)
	{
		 HMODULE hKernel32 = ::GetModuleHandle(TEXT("kernel32.dll"));
		 if (hKernel32)
		 {
			m_pfnIsWow64Process = (IsWow64ProcessPtr)::GetProcAddress(hKernel32, "IsWow64Process");
			if (m_pfnIsWow64Process)
			{
				m_pfnIsWow64Process ( GetCurrentProcess(), &m_bIsWow64Process );
			}
		 }
	}
}

CNdisApi::~CNdisApi()
{
	if ( m_hFileHandle != INVALID_HANDLE_VALUE )
	{
		::CloseHandle ( m_hFileHandle );
	}

    if ( m_ovlp.hEvent )
	{
		::CloseHandle ( m_ovlp.hEvent );
	}
}

BOOL CNdisApi::DeviceIoControl(DWORD dwService, void *BuffIn, int SizeIn, void *BuffOut, int SizeOut, unsigned long *SizeRet, LPOVERLAPPED povlp)
{
     BOOL Ret = 0;

	 // Supports overlapped and nonoverlapped IO

	 if ( !SizeRet ) SizeRet = &m_BytesReturned;

     if   ( m_hFileHandle != INVALID_HANDLE_VALUE )
     {
		 if( povlp == NULL )
			 Ret = ::DeviceIoControl ( m_hFileHandle, dwService, BuffIn, SizeIn, BuffOut, SizeOut, SizeRet, &m_ovlp);
		 else 
			 Ret = ::DeviceIoControl ( m_hFileHandle, dwService, BuffIn, SizeIn, BuffOut, SizeOut, SizeRet, povlp);

     }

     return Ret;
}

ULONG CNdisApi::GetVersion()
{
	 ULONG nDriverAPIVersion = 0xFFFFFFFF;
	 
	 BOOL bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_GET_VERSION,
					  &nDriverAPIVersion,
					  sizeof(ULONG),
					  &nDriverAPIVersion,
					  sizeof(ULONG),
                      NULL,   // Bytes Returned
                      NULL
					  );

	 return nDriverAPIVersion;
}

BOOL CNdisApi::GetTcpipBoundAdaptersInfo ( PTCP_AdapterList pAdapters )
{
	BOOL bIOResult = FALSE;

	if (m_bIsWow64Process)
	{
		// In case of WOW64 process pass our own structure to driver
		bIOResult = DeviceIoControl(
						  IOCTL_NDISRD_GET_TCPIP_INTERFACES,
						  &m_AdaptersList,
						  sizeof(TCP_AdapterList_WOW64),
						  &m_AdaptersList,
						  sizeof(TCP_AdapterList_WOW64),
						  NULL,   // Bytes Returned
						  NULL
						  );
		if (bIOResult)
		{
			// Simply copy all the fields except adapters handles
			pAdapters->m_nAdapterCount = m_AdaptersList.m_nAdapterCount;
			memmove(pAdapters->m_szAdapterNameList, m_AdaptersList.m_szAdapterNameList, sizeof(pAdapters->m_szAdapterNameList));
			memmove(pAdapters->m_nAdapterMediumList, m_AdaptersList.m_nAdapterMediumList, sizeof(pAdapters->m_nAdapterMediumList));
			memmove(pAdapters->m_czCurrentAddress, m_AdaptersList.m_czCurrentAddress, sizeof(pAdapters->m_czCurrentAddress));
			memmove(pAdapters->m_usMTU, m_AdaptersList.m_usMTU, sizeof(pAdapters->m_usMTU));
			for (unsigned i = 0; i < m_AdaptersList.m_nAdapterCount; ++i)
			{
				pAdapters->m_nAdapterHandle[i] = (HANDLE)&m_AdaptersList.m_nAdapterHandle[i];
			}
		}
	}
	else
	{
		bIOResult = DeviceIoControl(
						  IOCTL_NDISRD_GET_TCPIP_INTERFACES,
						  pAdapters,
						  sizeof(TCP_AdapterList),
						  pAdapters,
						  sizeof(TCP_AdapterList),
						  NULL,   // Bytes Returned
						  NULL
						  );
	}

	 return bIOResult;
}

BOOL CNdisApi::SendPacketToMstcp ( PETH_REQUEST pPacket )
{
	BOOL bIOResult = FALSE;

	if (m_bIsWow64Process)
	{
		ETH_REQUEST_WOW64 EthRequest;
		INTERMEDIATE_BUFFER_WOW64 Buffer;

		// Initialize INTERMEDIATE_BUFFER_WOW64
		Buffer.m_dwDeviceFlags = pPacket->EthPacket.Buffer->m_dwDeviceFlags;
		Buffer.m_Flags = pPacket->EthPacket.Buffer->m_Flags;
		Buffer.m_Length = pPacket->EthPacket.Buffer->m_Length;
		memmove(Buffer.m_IBuffer, pPacket->EthPacket.Buffer->m_IBuffer, Buffer.m_Length);

		// Initialize ETH_REQUEST_WOW64
		EthRequest.hAdapterHandle = *((ULARGE_INTEGER*)pPacket->hAdapterHandle);
		EthRequest.EthPacket.Buffer.HighPart = 0;
		EthRequest.EthPacket.Buffer.LowPart = (DWORD)&Buffer;

		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_SEND_PACKET_TO_MSTCP,
					  &EthRequest,
					  sizeof(ETH_REQUEST_WOW64),
					  NULL,
					  0,
                      NULL,   // Bytes Returned
                      NULL
					  );

	}
	else
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_SEND_PACKET_TO_MSTCP,
					  pPacket,
					  sizeof(ETH_REQUEST),
					  NULL,
					  0,
                      NULL,   // Bytes Returned
                      NULL
					  );
	}

	 return bIOResult;
}

BOOL CNdisApi::SendPacketToAdapter ( PETH_REQUEST pPacket )
{
	BOOL bIOResult = FALSE;

	if (m_bIsWow64Process)
	{
		ETH_REQUEST_WOW64 EthRequest;
		INTERMEDIATE_BUFFER_WOW64 Buffer;

		// Initialize INTERMEDIATE_BUFFER_WOW64
		Buffer.m_dwDeviceFlags = pPacket->EthPacket.Buffer->m_dwDeviceFlags;
		Buffer.m_Flags = pPacket->EthPacket.Buffer->m_Flags;
		Buffer.m_Length = pPacket->EthPacket.Buffer->m_Length;
		memmove(Buffer.m_IBuffer, pPacket->EthPacket.Buffer->m_IBuffer, Buffer.m_Length);

		// Initialize ETH_REQUEST_WOW64
		EthRequest.hAdapterHandle = *((ULARGE_INTEGER*)pPacket->hAdapterHandle);
		EthRequest.EthPacket.Buffer.HighPart = 0;
		EthRequest.EthPacket.Buffer.LowPart = (DWORD)&Buffer;

		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_SEND_PACKET_TO_ADAPTER,
					  &EthRequest,
					  sizeof(ETH_REQUEST_WOW64),
					  NULL,
					  0,
                      NULL,   // Bytes Returned
                      NULL
					  );

	}
	else
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_SEND_PACKET_TO_ADAPTER,
					  pPacket,
					  sizeof(ETH_REQUEST),
					  NULL,
					  0,
                      NULL,   // Bytes Returned
                      NULL
					  );
	}

	 return bIOResult;
}

BOOL CNdisApi::ReadPacket ( PETH_REQUEST pPacket )
{
	BOOL bIOResult = FALSE;

	if (m_bIsWow64Process)
	{
		ETH_REQUEST_WOW64 EthRequest;
		INTERMEDIATE_BUFFER_WOW64 Buffer;

		// Initialize ETH_REQUEST_WOW64
		EthRequest.hAdapterHandle = *((ULARGE_INTEGER*)pPacket->hAdapterHandle);
		EthRequest.EthPacket.Buffer.HighPart = 0;
		EthRequest.EthPacket.Buffer.LowPart = (DWORD)&Buffer;

		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_READ_PACKET,
					  &EthRequest,
					  sizeof(ETH_REQUEST_WOW64),
					  &EthRequest,
					  sizeof(ETH_REQUEST_WOW64),
                      NULL,   // Bytes Returned
                      NULL
					  );

		if ( bIOResult )
		{
			// Copy the resulting packet from INTERMEDIATE_BUFFER_WOW64 to INTERMEDIATE_BUFFER
			pPacket->EthPacket.Buffer->m_dwDeviceFlags = Buffer.m_dwDeviceFlags;
			pPacket->EthPacket.Buffer->m_Flags = Buffer.m_Flags;
			pPacket->EthPacket.Buffer->m_Length = Buffer.m_Length;
			memmove(pPacket->EthPacket.Buffer->m_IBuffer, Buffer.m_IBuffer, Buffer.m_Length);
		}
	}
	else
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_READ_PACKET,
					  pPacket,
					  sizeof(ETH_REQUEST),
					  pPacket,
					  sizeof(ETH_REQUEST),
                      NULL,   // Bytes Returned
                      NULL
					  );
	}

	 return bIOResult;
}

BOOL CNdisApi::SendPacketsToMstcp (PETH_M_REQUEST pPackets)
{
	BOOL bIOResult = FALSE;

	if (m_bIsWow64Process)
	{
		PETH_M_REQUEST_WOW64 pEthRequest = (PETH_M_REQUEST_WOW64)malloc(sizeof(ETH_M_REQUEST_WOW64) + sizeof(NDISRD_ETH_Packet_WOW64)*(pPackets->dwPacketsNumber - 1));
		PINTERMEDIATE_BUFFER_WOW64 Buffers = (PINTERMEDIATE_BUFFER_WOW64)malloc(pPackets->dwPacketsNumber*sizeof(INTERMEDIATE_BUFFER_WOW64));
		memset (pEthRequest, 0, sizeof(ETH_M_REQUEST_WOW64) + sizeof(NDISRD_ETH_Packet_WOW64)*(pPackets->dwPacketsNumber - 1));
		memset (Buffers, 0, pPackets->dwPacketsNumber*sizeof(INTERMEDIATE_BUFFER_WOW64));

		if (Buffers && pEthRequest)
		{
			pEthRequest->hAdapterHandle = *((ULARGE_INTEGER*)pPackets->hAdapterHandle);
			pEthRequest->dwPacketsNumber = pPackets->dwPacketsNumber;

			for (unsigned i = 0; i < pPackets->dwPacketsNumber; ++i)
			{
				// Initialize INTERMEDIATE_BUFFER_WOW64
				Buffers[i].m_dwDeviceFlags = pPackets->EthPacket[i].Buffer->m_dwDeviceFlags;
				Buffers[i].m_Flags = pPackets->EthPacket[i].Buffer->m_Flags;
				Buffers[i].m_Length = pPackets->EthPacket[i].Buffer->m_Length;
				memmove(Buffers[i].m_IBuffer, pPackets->EthPacket[i].Buffer->m_IBuffer, Buffers[i].m_Length);

				// Initialize ETH_REQUEST_WOW64
				pEthRequest->EthPacket[i].Buffer.HighPart = 0;
				pEthRequest->EthPacket[i].Buffer.LowPart = (DWORD)&Buffers[i];
			}

			bIOResult = DeviceIoControl(
						  IOCTL_NDISRD_SEND_PACKETS_TO_MSTCP,
						  pEthRequest,
						  sizeof(ETH_M_REQUEST_WOW64) + sizeof(NDISRD_ETH_Packet_WOW64)*(pPackets->dwPacketsNumber - 1),
						  NULL,
						  0,
						  NULL,   // Bytes Returned
						  NULL
						  );

			free(Buffers);
			free(pEthRequest);
		}
		else
		{
			if ( Buffers )
				free (Buffers);

			if ( pEthRequest )
				free (pEthRequest);

			return FALSE;
		}
	}
	else
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_SEND_PACKETS_TO_MSTCP,
					  pPackets,
					  sizeof(ETH_M_REQUEST) + sizeof(NDISRD_ETH_Packet)*(pPackets->dwPacketsNumber - 1),
					  NULL,
					  0,
                      NULL,   // Bytes Returned
                      NULL
					  );
	}

	 return bIOResult;
}

BOOL CNdisApi::SendPacketsToAdapter(PETH_M_REQUEST pPackets)
{
	BOOL bIOResult = FALSE;

	if (m_bIsWow64Process)
	{
		PETH_M_REQUEST_WOW64 pEthRequest = (PETH_M_REQUEST_WOW64)malloc(sizeof(ETH_M_REQUEST_WOW64) + sizeof(NDISRD_ETH_Packet_WOW64)*(pPackets->dwPacketsNumber - 1));
		PINTERMEDIATE_BUFFER_WOW64 Buffers = (PINTERMEDIATE_BUFFER_WOW64)malloc(pPackets->dwPacketsNumber*sizeof(INTERMEDIATE_BUFFER_WOW64));
		memset (pEthRequest, 0, sizeof(ETH_M_REQUEST_WOW64) + sizeof(NDISRD_ETH_Packet_WOW64)*(pPackets->dwPacketsNumber - 1));
		memset (Buffers, 0, pPackets->dwPacketsNumber*sizeof(INTERMEDIATE_BUFFER_WOW64));

		if (Buffers && pEthRequest)
		{
			pEthRequest->hAdapterHandle = *((ULARGE_INTEGER*)pPackets->hAdapterHandle);
			pEthRequest->dwPacketsNumber = pPackets->dwPacketsNumber;

			for (unsigned i = 0; i < pPackets->dwPacketsNumber; ++i)
			{
				// Initialize INTERMEDIATE_BUFFER_WOW64
				Buffers[i].m_dwDeviceFlags = pPackets->EthPacket[i].Buffer->m_dwDeviceFlags;
				Buffers[i].m_Flags = pPackets->EthPacket[i].Buffer->m_Flags;
				Buffers[i].m_Length = pPackets->EthPacket[i].Buffer->m_Length;
				memmove(Buffers[i].m_IBuffer, pPackets->EthPacket[i].Buffer->m_IBuffer, Buffers[i].m_Length);

				// Initialize ETH_REQUEST_WOW64
				pEthRequest->EthPacket[i].Buffer.HighPart = 0;
				pEthRequest->EthPacket[i].Buffer.LowPart = (DWORD)&Buffers[i];
			}

			bIOResult = DeviceIoControl(
						  IOCTL_NDISRD_SEND_PACKETS_TO_ADAPTER,
						  pEthRequest,
						  sizeof(ETH_M_REQUEST_WOW64) + sizeof(NDISRD_ETH_Packet_WOW64)*(pPackets->dwPacketsNumber - 1),
						  NULL,
						  0,
						  NULL,   // Bytes Returned
						  NULL
						  );

			free(Buffers);
			free(pEthRequest);
		}
		else
		{
			if ( Buffers )
				free (Buffers);

			if ( pEthRequest )
				free (pEthRequest);

			return FALSE;
		}
	}
	else
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_SEND_PACKETS_TO_ADAPTER,
					  pPackets,
					  sizeof(ETH_M_REQUEST) + sizeof(NDISRD_ETH_Packet)*(pPackets->dwPacketsNumber - 1),
					  NULL,
					  0,
                      NULL,   // Bytes Returned
                      NULL
					  );
	}

	 return bIOResult;
}

BOOL CNdisApi::ReadPackets(PETH_M_REQUEST pPackets)
{
	BOOL bIOResult = FALSE;
	unsigned i = 0;

	if (m_bIsWow64Process)
	{
		PETH_M_REQUEST_WOW64 pEthRequest = (PETH_M_REQUEST_WOW64)malloc(sizeof(ETH_M_REQUEST_WOW64) + sizeof(NDISRD_ETH_Packet_WOW64)*(pPackets->dwPacketsNumber - 1));
		PINTERMEDIATE_BUFFER_WOW64 Buffers = (PINTERMEDIATE_BUFFER_WOW64)malloc(pPackets->dwPacketsNumber*sizeof(INTERMEDIATE_BUFFER_WOW64));
		memset (pEthRequest, 0, sizeof(ETH_M_REQUEST_WOW64) + sizeof(NDISRD_ETH_Packet_WOW64)*(pPackets->dwPacketsNumber - 1));
		memset (Buffers, 0, pPackets->dwPacketsNumber*sizeof(INTERMEDIATE_BUFFER_WOW64));

		if (Buffers && pEthRequest)
		{
			pEthRequest->hAdapterHandle = *((ULARGE_INTEGER*)pPackets->hAdapterHandle);
			pEthRequest->dwPacketsNumber = pPackets->dwPacketsNumber;

			for (i = 0; i < pPackets->dwPacketsNumber; ++i)
			{
				// Initialize ETH_REQUEST_WOW64
				pEthRequest->EthPacket[i].Buffer.HighPart = 0;
				pEthRequest->EthPacket[i].Buffer.LowPart = (DWORD)&Buffers[i];
			}

			bIOResult = DeviceIoControl(
						  IOCTL_NDISRD_READ_PACKETS,
						  pEthRequest,
						  sizeof(ETH_M_REQUEST_WOW64) + sizeof(NDISRD_ETH_Packet_WOW64)*(pPackets->dwPacketsNumber - 1),
						  pEthRequest,
						  sizeof(ETH_M_REQUEST_WOW64) + sizeof(NDISRD_ETH_Packet_WOW64)*(pPackets->dwPacketsNumber - 1),
						  NULL,   // Bytes Returned
						  NULL
						  );

			pPackets->dwPacketsSuccess = pEthRequest->dwPacketsSuccess;

			for (i = 0; i < pEthRequest->dwPacketsSuccess; ++i)
			{
				// Copy back
				pPackets->EthPacket[i].Buffer->m_dwDeviceFlags = Buffers[i].m_dwDeviceFlags;
				pPackets->EthPacket[i].Buffer->m_Flags = Buffers[i].m_Flags;
				pPackets->EthPacket[i].Buffer->m_Length = Buffers[i].m_Length;
				memmove(pPackets->EthPacket[i].Buffer->m_IBuffer, Buffers[i].m_IBuffer, Buffers[i].m_Length);
			}

			free(Buffers);
			free(pEthRequest);
		}
		else
		{
			if ( Buffers )
				free (Buffers);

			if ( pEthRequest )
				free (pEthRequest);

			return FALSE;
		}
	}
	else
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_READ_PACKETS,
					  pPackets,
					  sizeof(ETH_M_REQUEST) + sizeof(NDISRD_ETH_Packet)*(pPackets->dwPacketsNumber - 1),
					  pPackets,
					  sizeof(ETH_M_REQUEST) + sizeof(NDISRD_ETH_Packet)*(pPackets->dwPacketsNumber - 1),
                      NULL,   // Bytes Returned
                      NULL
					  );
	}

	 return bIOResult;
}

BOOL CNdisApi::SetAdapterMode ( PADAPTER_MODE pMode )
{
	BOOL bIOResult = FALSE;

	if (m_bIsWow64Process)
	{
		ADAPTER_MODE_WOW64 AdapterMode;
		AdapterMode.dwFlags = pMode->dwFlags;
		AdapterMode.hAdapterHandle = *((ULARGE_INTEGER*)pMode->hAdapterHandle);

		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_SET_ADAPTER_MODE,
					  &AdapterMode,
					  sizeof(ADAPTER_MODE_WOW64),
					  NULL,
					  0,
                      NULL,   // Bytes Returned
                      NULL
					  );
	}
	else
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_SET_ADAPTER_MODE,
					  pMode,
					  sizeof(ADAPTER_MODE),
					  NULL,
					  0,
                      NULL,   // Bytes Returned
                      NULL
					  );
	}
	
	return bIOResult;
}

BOOL CNdisApi::GetAdapterMode ( PADAPTER_MODE pMode )
{
	BOOL bIOResult = FALSE;

	if (m_bIsWow64Process)
	{
		ADAPTER_MODE_WOW64 AdapterMode;
		AdapterMode.hAdapterHandle = *((ULARGE_INTEGER*)pMode->hAdapterHandle);

		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_GET_ADAPTER_MODE,
					  &AdapterMode,
					  sizeof(ADAPTER_MODE_WOW64),
					  &AdapterMode,
					  sizeof(ADAPTER_MODE_WOW64),
                      NULL,   // Bytes Returned
                      NULL
					  );

		pMode->dwFlags = AdapterMode.dwFlags;
		
	}
	else
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_GET_ADAPTER_MODE,
					  pMode,
					  sizeof(ADAPTER_MODE),
					  pMode,
					  sizeof(ADAPTER_MODE),
                      NULL,   // Bytes Returned
                      NULL
					  );
	}

	 return bIOResult;
}

BOOL CNdisApi::FlushAdapterPacketQueue ( HANDLE hAdapter )
{
	BOOL bIOResult = FALSE;

	if (m_bIsWow64Process)
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_FLUSH_ADAPTER_QUEUE,
					  (ULARGE_INTEGER*)hAdapter,
					  sizeof(ULARGE_INTEGER),
					  NULL,
					  0,
                      NULL,   // Bytes Returned
                      NULL
					  );
	}
	else
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_FLUSH_ADAPTER_QUEUE,
					  &hAdapter,
					  sizeof(HANDLE),
					  NULL,
					  0,
                      NULL,   // Bytes Returned
                      NULL
					  );
	}

	 return bIOResult;
}

BOOL CNdisApi::GetAdapterPacketQueueSize ( HANDLE hAdapter, PDWORD pdwSize )
{
	BOOL bIOResult = FALSE;

	if (m_bIsWow64Process)
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_ADAPTER_QUEUE_SIZE,
					  (ULARGE_INTEGER*)hAdapter,
					  sizeof(ULARGE_INTEGER),
					  pdwSize,
					  sizeof(DWORD),
                      NULL,   // Bytes Returned
                      NULL
					  );
	}
	else
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_ADAPTER_QUEUE_SIZE,
					  &hAdapter,
					  sizeof(HANDLE),
					  pdwSize,
					  sizeof(DWORD),
                      NULL,   // Bytes Returned
                      NULL
					  );
	}

	 return bIOResult;
}

BOOL CNdisApi::SetPacketEvent (HANDLE hAdapter, HANDLE hWin32Event)
{
	HANDLE			hRing0Event = NULL;
	OSVERSIONINFO	verInfo;
	ADAPTER_EVENT	AdapterEvent;

	AdapterEvent.hAdapterHandle = hAdapter;
	
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx ( &verInfo );

	if (verInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		// Windows NT
		hRing0Event = hWin32Event;
	}
	else
	{
		// Windows 9x/ME
		HANDLE (WINAPI *pfOpenVxDHandle)(HANDLE);
		HINSTANCE hKernel32Dll = LoadLibrary(TEXT("kernel32.dll"));
		
		if(!hKernel32Dll)
			return FALSE;

		pfOpenVxDHandle = (HANDLE (WINAPI *)(HANDLE))GetProcAddress(hKernel32Dll, "OpenVxDHandle");

		if (!pfOpenVxDHandle )
			return FALSE;

		if (hWin32Event)
			hRing0Event = pfOpenVxDHandle(hWin32Event);
		else
			hRing0Event = NULL;

		FreeLibrary(hKernel32Dll);
	}

	AdapterEvent.hEvent = hRing0Event;

	BOOL bIOResult = FALSE;

	if (m_bIsWow64Process)
	{
		ADAPTER_EVENT_WOW64 AdapterEvent64;
		AdapterEvent64.hAdapterHandle = *((ULARGE_INTEGER*)AdapterEvent.hAdapterHandle);
		AdapterEvent64.hEvent.HighPart = 0;
		AdapterEvent64.hEvent.LowPart = (DWORD)AdapterEvent.hEvent;

		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_SET_EVENT,
					  &AdapterEvent64,
					  sizeof(ADAPTER_EVENT_WOW64),
					  NULL,
					  0,
                      NULL,   // Bytes Returned
                      NULL
					  );
	}
	else
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_SET_EVENT,
					  &AdapterEvent,
					  sizeof(ADAPTER_EVENT),
					  NULL,
					  0,
                      NULL,   // Bytes Returned
                      NULL
					  );
	}

	 return bIOResult;
}

BOOL CNdisApi::SetWANEvent (HANDLE hWin32Event)
{
	HANDLE			hRing0Event = NULL;
	OSVERSIONINFO	verInfo;
		
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx ( &verInfo );

	if (verInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		// Windows NT
		hRing0Event = hWin32Event;
	}
	else
	{
		// Windows 9x/ME
		HANDLE (WINAPI *pfOpenVxDHandle)(HANDLE);
		HINSTANCE hKernel32Dll = LoadLibrary(TEXT("kernel32.dll"));
		
		if(!hKernel32Dll)
			return FALSE;

		pfOpenVxDHandle = (HANDLE (WINAPI *)(HANDLE))GetProcAddress(hKernel32Dll, "OpenVxDHandle");

		if (!pfOpenVxDHandle )
			return FALSE;

		if (hWin32Event)
			hRing0Event = pfOpenVxDHandle(hWin32Event);
		else
			hRing0Event = NULL;

		FreeLibrary(hKernel32Dll);
	}

	BOOL bIOResult = FALSE;

	if (m_bIsWow64Process)
	{
		ULARGE_INTEGER ulRing0Event;
		ulRing0Event.HighPart = 0;
		ulRing0Event.LowPart = (DWORD)hRing0Event;

		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_SET_WAN_EVENT,
					  &ulRing0Event,
					  sizeof(ULARGE_INTEGER),
					  NULL,
					  0,
                      NULL,   // Bytes Returned
                      NULL
					  );
	}
	else
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_SET_WAN_EVENT,
					  &hRing0Event,
					  sizeof(HANDLE),
					  NULL,
					  0,
                      NULL,   // Bytes Returned
                      NULL
					  );
	}

	 return bIOResult;
}

BOOL CNdisApi::SetAdapterListChangeEvent (HANDLE hWin32Event)
{
	HANDLE			hRing0Event = NULL;
	OSVERSIONINFO	verInfo;
		
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx ( &verInfo );

	if (verInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		// Windows NT
		hRing0Event = hWin32Event;
	}
	else
	{
		// Windows 9x/ME
		HANDLE (WINAPI *pfOpenVxDHandle)(HANDLE);
		HINSTANCE hKernel32Dll = LoadLibrary(TEXT("kernel32.dll"));
		
		if(!hKernel32Dll)
			return FALSE;

		pfOpenVxDHandle = (HANDLE (WINAPI *)(HANDLE))GetProcAddress(hKernel32Dll, "OpenVxDHandle");

		if (!pfOpenVxDHandle )
			return FALSE;

		if (hWin32Event)
			hRing0Event = pfOpenVxDHandle(hWin32Event);
		else
			hRing0Event = NULL;

		FreeLibrary(hKernel32Dll);
	}

	BOOL bIOResult = FALSE;

	if (m_bIsWow64Process)
	{
		ULARGE_INTEGER ulRing0Event;
		ulRing0Event.HighPart = 0;
		ulRing0Event.LowPart = (DWORD)hRing0Event;

		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_SET_ADAPTER_EVENT,
					  &ulRing0Event,
					  sizeof(ULARGE_INTEGER),
					  NULL,
					  0,
                      NULL,   // Bytes Returned
                      NULL
					  );
	}
	else
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_SET_ADAPTER_EVENT,
					  &hRing0Event,
					  sizeof(HANDLE),
					  NULL,
					  0,
                      NULL,   // Bytes Returned
                      NULL
					  );
	}
	
	return bIOResult;
}

BOOL CNdisApi::NdisrdRequest ( PPACKET_OID_DATA OidData, BOOL Set )
{
	OVERLAPPED Overlap;
	DWORD dwLength = OidData->Length;

	Overlap.Offset = 0;
	Overlap.OffsetHigh = 0;
    Overlap.hEvent = CreateEvent (
						NULL,
						FALSE,
						FALSE,
						NULL
						);

    if (Overlap.hEvent == NULL)
    {
		return FALSE;
    }

	if (!ResetEvent(Overlap.hEvent))
	{
		::CloseHandle(Overlap.hEvent);
		return FALSE;
	}

	BOOL bIOResult = FALSE;

	if (m_bIsWow64Process)
	{
		PPACKET_OID_DATA_WOW64 OidData64 = (PPACKET_OID_DATA_WOW64)malloc (sizeof(PACKET_OID_DATA_WOW64) + OidData->Length - 1);
		if (OidData64)
		{
			OidData64->hAdapterHandle = *((ULARGE_INTEGER*)OidData->hAdapterHandle);
			OidData64->Oid = OidData->Oid;
			OidData64->Length = OidData->Length;
			if (Set)
				memmove (OidData64->Data, OidData->Data, OidData->Length);

			bIOResult = DeviceIoControl(
						  (DWORD) Set ? IOCTL_NDISRD_NDIS_SET_REQUEST : IOCTL_NDISRD_NDIS_GET_REQUEST,
						  OidData64,
						  sizeof (PACKET_OID_DATA_WOW64) - 1 + OidData->Length,
						  OidData64,
						  sizeof (PACKET_OID_DATA_WOW64) - 1 + OidData->Length,
						  NULL,   // Bytes Returned
						  &Overlap
						  );

			if ((!bIOResult)&&(ERROR_IO_PENDING == GetLastError()))
				WaitForSingleObject ( Overlap.hEvent, INFINITE );
			else
				if(!bIOResult)
				{
					::CloseHandle(Overlap.hEvent);
					delete[] (char*)OidData64;
					return FALSE;
				}

			if (!Set)
			{
				memmove (OidData->Data, OidData64->Data, OidData64->Length);
				OidData->Length = OidData64->Length;
			}

			free(OidData64);
		}
	}
	else
	{
		bIOResult = DeviceIoControl(
					  (DWORD) Set ? IOCTL_NDISRD_NDIS_SET_REQUEST : IOCTL_NDISRD_NDIS_GET_REQUEST,
					  OidData,
					  sizeof (PACKET_OID_DATA) - 1 + OidData->Length,
					  OidData,
					  sizeof (PACKET_OID_DATA) - 1 + OidData->Length,
                      NULL,   // Bytes Returned
                      &Overlap
					  );

		if ((!bIOResult)&&(ERROR_IO_PENDING == GetLastError()))
			WaitForSingleObject ( Overlap.hEvent, INFINITE );
		else
			if(!bIOResult)
			{
				::CloseHandle(Overlap.hEvent);
				return FALSE;
			}
	}

	::CloseHandle(Overlap.hEvent);

	if (dwLength == OidData->Length)
		return TRUE;
	else
		return FALSE;
}

BOOL CNdisApi::GetRasLinks ( HANDLE hAdapter, PRAS_LINKS pLinks )
{
	BOOL bIOResult = FALSE;

	if (m_bIsWow64Process)
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_GET_RAS_LINKS,
					  (ULARGE_INTEGER*)hAdapter,
					  sizeof(ULARGE_INTEGER),
					  pLinks,
					  sizeof(RAS_LINKS),
                      NULL,   // Bytes Returned
                      NULL
					  );
	}
	else
	{
		bIOResult = DeviceIoControl(
					  IOCTL_NDISRD_GET_RAS_LINKS,
					  &hAdapter,
					  sizeof(HANDLE),
					  pLinks,
					  sizeof(RAS_LINKS),
                      NULL,   // Bytes Returned
                      NULL
					  );
	}

	return bIOResult;
}

BOOL CNdisApi::SetHwPacketFilter ( HANDLE hAdapter, DWORD Filter)
{
	BOOL bRet = FALSE;
	PPACKET_OID_DATA pPacket = (PPACKET_OID_DATA)malloc (sizeof(PACKET_OID_DATA) + sizeof(DWORD) - 1);

	if (pPacket)
	{
		pPacket->Length = sizeof(DWORD);
		pPacket->Oid = OID_GEN_CURRENT_PACKET_FILTER;
		pPacket->hAdapterHandle = hAdapter;

		*((PDWORD)pPacket->Data) = Filter;
		bRet = NdisrdRequest (pPacket, TRUE);

		free(pPacket);
	}
	
	return bRet;
}

BOOL CNdisApi::GetHwPacketFilter ( HANDLE hAdapter, PDWORD pFilter)
{
	BOOL bRet = FALSE;

	PPACKET_OID_DATA pPacket = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA) + sizeof(DWORD) - 1);

	if (pPacket)
	{
		pPacket->Length = sizeof(DWORD);
		pPacket->Oid = OID_GEN_CURRENT_PACKET_FILTER;
		pPacket->hAdapterHandle = hAdapter;

		if (NdisrdRequest(pPacket, FALSE))
		{
			*pFilter = *((PDWORD)pPacket->Data);

			bRet = TRUE;
		}

		free(pPacket);
	}
	return bRet;
}

BOOL CNdisApi::SetPacketFilterTable (PSTATIC_FILTER_TABLE pFilterList)
{
	BOOL bIOResult = FALSE;

	if (m_bIsWow64Process)
	{
		// Adapter handle values in the table contain values which are not valid for the driver
		// and we need to pre-process filter table before passing it to the driver

		for (unsigned i = 0; i < pFilterList->m_TableSize; ++i)
		{
			if(pFilterList->m_StaticFilters[i].m_Adapter.QuadPart)
				pFilterList->m_StaticFilters[i].m_Adapter = 
					*((PULARGE_INTEGER)(pFilterList->m_StaticFilters[i].m_Adapter.LowPart));
		}
	}

	bIOResult = DeviceIoControl(
				  IOCTL_NDISRD_SET_PACKET_FILTERS,
				  pFilterList,
				  sizeof(STATIC_FILTER_TABLE) + (pFilterList->m_TableSize - ANY_SIZE)*sizeof(STATIC_FILTER),
				  NULL,
				  0,
                  NULL,   // Bytes Returned
                  NULL
				  );

	 return bIOResult;
}

BOOL CNdisApi::ResetPacketFilterTable ()
{
	BOOL bIOResult = FALSE;

	bIOResult = DeviceIoControl(
				  IOCTL_NDISRD_RESET_PACKET_FILTERS,
				  NULL,
				  0,
				  NULL,
				  0,
                  NULL,   // Bytes Returned
                  NULL
				  );

	 return bIOResult;
}

BOOL CNdisApi::GetPacketFilterTableSize ( PDWORD pdwTableSize )
{
	BOOL bIOResult = FALSE;

	bIOResult = DeviceIoControl(
				  IOCTL_NDISRD_GET_PACKET_FILTERS_TABLESIZE,
				  NULL,
				  0,
				  pdwTableSize,
				  sizeof (DWORD),
                  NULL,   // Bytes Returned
                  NULL
				  );

	 return bIOResult;
}

BOOL CNdisApi::GetPacketFilterTable ( PSTATIC_FILTER_TABLE pFilterList )
{
	BOOL bIOResult = FALSE;

	bIOResult = DeviceIoControl(
				  IOCTL_NDISRD_GET_PACKET_FILTERS,
				  NULL,
				  0,
				  pFilterList,
				  sizeof(STATIC_FILTER_TABLE) + (pFilterList->m_TableSize - ANY_SIZE)*sizeof(STATIC_FILTER),
                  NULL,   // Bytes Returned
                  NULL
				  );

	if (m_bIsWow64Process)
	{
		// Adapter handle values in the table contain values which are not valid for the client
		// and we need to post-process filter table before passing it to the client

		for (unsigned i = 0; i < pFilterList->m_TableSize; ++i)
		{
			for (unsigned j = 0; j < m_AdaptersList.m_nAdapterCount; ++j)
			{
				if (m_AdaptersList.m_nAdapterHandle[j].QuadPart == pFilterList->m_StaticFilters[i].m_Adapter.QuadPart)
				{
					pFilterList->m_StaticFilters[i].m_Adapter.HighPart = 0;
					pFilterList->m_StaticFilters[i].m_Adapter.LowPart = (DWORD)&m_AdaptersList.m_nAdapterHandle[j];
					break;
				}
			}
		}
	}

	 return bIOResult;
}

BOOL CNdisApi::GetPacketFilterTableResetStats ( PSTATIC_FILTER_TABLE pFilterList )
{
	BOOL bIOResult = FALSE;

	bIOResult = DeviceIoControl(
				  IOCTL_NDISRD_GET_PACKET_FILTERS_RESET_STATS,
				  NULL,
				  0,
				  pFilterList,
				  sizeof(STATIC_FILTER_TABLE) + (pFilterList->m_TableSize - ANY_SIZE)*sizeof(STATIC_FILTER),
                  NULL,   // Bytes Returned
                  NULL
				  );

	if (m_bIsWow64Process)
	{
		// Adapter handle values in the table contain values which are not valid for the client
		// and we need to post-process filter table before passing it to the client

		for (unsigned i = 0; i < pFilterList->m_TableSize; ++i)
		{
			for (unsigned j = 0; j < m_AdaptersList.m_nAdapterCount; ++j)
			{
				if (m_AdaptersList.m_nAdapterHandle[j].QuadPart == pFilterList->m_StaticFilters[i].m_Adapter.QuadPart)
				{
					pFilterList->m_StaticFilters[i].m_Adapter.HighPart = 0;
					pFilterList->m_StaticFilters[i].m_Adapter.LowPart = (DWORD)&m_AdaptersList.m_nAdapterHandle[j];
					break;
				}
			}
		}
	}

	 return bIOResult;
}

BOOL CNdisApi::IsDriverLoaded ()
{
	return m_bIsLoadSuccessfully;
}

DWORD CNdisApi::GetBytesReturned ()
{
	return m_BytesReturned;
}

BOOL CNdisApi::SetMTUDecrement(DWORD dwMTUDecrement)
{
	OSVERSIONINFO osVer;
	HKEY hKey;

	osVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

	GetVersionEx ( &osVer );

	if (osVer.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		// Windows NT, 2000 or XP
		if (ERROR_SUCCESS == RegCreateKey (HKEY_LOCAL_MACHINE, WINNT_REG_PARAM, &hKey))
		{
			if( ERROR_SUCCESS == RegSetValueEx (hKey, TEXT("MTUDecrement"), NULL, REG_DWORD, (CONST BYTE*)&dwMTUDecrement, sizeof(DWORD)))
			{
				RegCloseKey(hKey);
				return TRUE;
			}

			RegCloseKey(hKey);
			return FALSE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		// Windows 9x/ME
		if (ERROR_SUCCESS == RegCreateKeyA (HKEY_LOCAL_MACHINE, WIN9X_REG_PARAM, &hKey))
		{
			if( ERROR_SUCCESS == RegSetValueEx (hKey, TEXT("MTUDecrement"), NULL, REG_DWORD, (CONST BYTE*)&dwMTUDecrement, sizeof(DWORD)))
			{
				RegCloseKey(hKey);
				return TRUE;
			}

			RegCloseKey(hKey);
			return FALSE;
		}
		else
		{
			return FALSE;
		}
	}
}

DWORD CNdisApi::GetMTUDecrement ()
{
	OSVERSIONINFO osVer;
	HKEY hKey;
	DWORD dwMTUDecrement;
	DWORD dwSize = sizeof(DWORD);

	osVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

	GetVersionEx ( &osVer );

	if (osVer.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		// Windows NT, 2000 or XP
		if (ERROR_SUCCESS == RegCreateKey (HKEY_LOCAL_MACHINE, WINNT_REG_PARAM, &hKey))
		{
			if( ERROR_SUCCESS == RegQueryValueEx (hKey, TEXT("MTUDecrement"), NULL, NULL, (BYTE*)&dwMTUDecrement, &dwSize))
			{
				RegCloseKey(hKey);
				return dwMTUDecrement;
			}

			RegCloseKey(hKey);
			return 0;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		// Windows 9x/ME
		if (ERROR_SUCCESS == RegCreateKeyA (HKEY_LOCAL_MACHINE, WIN9X_REG_PARAM, &hKey))
		{
			if( ERROR_SUCCESS == RegQueryValueEx (hKey, TEXT("MTUDecrement"), NULL, NULL, (BYTE*)&dwMTUDecrement, &dwSize))
			{
				RegCloseKey(hKey);
				return dwMTUDecrement;
			}

			RegCloseKey(hKey);
			return 0;
		}
		else
		{
			return 0;
		}
	}

}

BOOL CNdisApi::SetAdaptersStartupMode(DWORD dwStartupMode)
{
	OSVERSIONINFO osVer;
	HKEY hKey;

	osVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

	GetVersionEx ( &osVer );

	if (osVer.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		// Windows NT, 2000 or XP
		if (ERROR_SUCCESS == RegCreateKey (HKEY_LOCAL_MACHINE, WINNT_REG_PARAM, &hKey))
		{
			if( ERROR_SUCCESS == RegSetValueEx (hKey, TEXT("StartupMode"), NULL, REG_DWORD, (CONST BYTE*)&dwStartupMode, sizeof(DWORD)))
			{
				RegCloseKey(hKey);
				return TRUE;
			}

			RegCloseKey(hKey);
			return FALSE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		// Windows 9x/ME
		if (ERROR_SUCCESS == RegCreateKeyA (HKEY_LOCAL_MACHINE, WIN9X_REG_PARAM, &hKey))
		{
			if( ERROR_SUCCESS == RegSetValueEx (hKey, TEXT("StartupMode"), NULL, REG_DWORD, (CONST BYTE*)&dwStartupMode, sizeof(DWORD)))
			{
				RegCloseKey(hKey);
				return TRUE;
			}

			RegCloseKey(hKey);
			return FALSE;
		}
		else
		{
			return FALSE;
		}
	}
}

DWORD CNdisApi::GetAdaptersStartupMode ()
{
	OSVERSIONINFO osVer;
	HKEY hKey;
	DWORD dwStartupMode;
	DWORD dwSize = sizeof(DWORD);

	osVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

	GetVersionEx ( &osVer );

	if (osVer.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		// Windows NT, 2000 or XP
		if (ERROR_SUCCESS == RegCreateKey (HKEY_LOCAL_MACHINE, WINNT_REG_PARAM, &hKey))
		{
			if( ERROR_SUCCESS == RegQueryValueEx (hKey, TEXT("StartupMode"), NULL, NULL, (BYTE*)&dwStartupMode, &dwSize))
			{
				RegCloseKey(hKey);
				return dwStartupMode;
			}

			RegCloseKey(hKey);
			return 0;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		// Windows 9x/ME
		if (ERROR_SUCCESS == RegCreateKeyA (HKEY_LOCAL_MACHINE, WIN9X_REG_PARAM, &hKey))
		{
			if( ERROR_SUCCESS == RegQueryValueEx (hKey, TEXT("StartupMode"), NULL, NULL, (BYTE*)&dwStartupMode, &dwSize))
			{
				RegCloseKey(hKey);
				return dwStartupMode;
			}

			RegCloseKey(hKey);
			return 0;
		}
		else
		{
			return 0;
		}
	}

}

BOOL
	CNdisApi::ConvertWindowsNTAdapterName (
		LPCSTR szAdapterName,
		LPSTR szUserFriendlyName,
		DWORD dwUserFriendlyNameLength
		)
{
	HKEY		hKeyAdapters, hKeyAdapter;
	DWORD		dwType, dwIndex = 0;
	FILETIME	time;
	char		szSubKey[MAX_PATH*2], szServiceName[MAX_PATH*2];
	DWORD		dwSubKeyLength = MAX_PATH*2;
	DWORD		dwServiceNameLength = MAX_PATH*2;
	BOOL		bRet = TRUE;
				
	LONG lResult = RegOpenKeyEx (
						HKEY_LOCAL_MACHINE,
						REGSTR_NETWORK_CARDS,
						0,
						KEY_READ,
						&hKeyAdapters
						);

	if (lResult == ERROR_SUCCESS)
	{
		// Enumerate bound interfaces
		while (RegEnumKeyExA(hKeyAdapters, dwIndex, szSubKey, &dwSubKeyLength, NULL, NULL, NULL, &time) == ERROR_SUCCESS)
		{
			// Subkey retrieved, open subkey
			lResult = RegOpenKeyExA (
						hKeyAdapters,
						szSubKey,
						0,
						KEY_READ,
						&hKeyAdapter
						);

			if (lResult == ERROR_SUCCESS)
			{
				lResult = RegQueryValueExA(hKeyAdapter, REGSTR_VAL_SERVICE_NAME, NULL, &dwType, (LPBYTE)szServiceName, &dwServiceNameLength);
				if (lResult == ERROR_SUCCESS)
				{			
					if (_stricmp (szServiceName, &szAdapterName[strlen("\\Device\\")]) == 0)
					{
						lResult = RegQueryValueExA(hKeyAdapter, REGSTR_VAL_TITLE, NULL, &dwType, (LPBYTE)szUserFriendlyName, &dwUserFriendlyNameLength);		

						RegCloseKey (hKeyAdapter);
						RegCloseKey (hKeyAdapters);

						if (lResult == ERROR_SUCCESS)
						{
							return TRUE;
						}
						else
						{
							return FALSE;
						}
					}
				}
				else
				{
					bRet = FALSE;
				}
				RegCloseKey (hKeyAdapter);
				dwServiceNameLength = MAX_PATH*2;
			}
			else
			{
				bRet = FALSE;
			}

			dwIndex++;
			dwSubKeyLength = MAX_PATH;
		}
		
		RegCloseKey (hKeyAdapters);

	}
	else
	{
		bRet = FALSE;
	}

	return bRet;
}

BOOL
	CNdisApi::ConvertWindows2000AdapterName (
		LPCSTR szAdapterName,
		LPSTR szUserFriendlyName,
		DWORD dwUserFriendlyNameLength
		)
{
	HKEY		hKey;
	char		szFriendlyNameKey[MAX_PATH*2];
	DWORD		dwType;

	if (_stricmp(szAdapterName, DEVICE_NDISWANIP) == 0)
	{
		strcpy (szUserFriendlyName, USER_NDISWANIP);
		return TRUE;
	}

	if (_stricmp(szAdapterName, DEVICE_NDISWANBH) == 0)
	{
		strcpy (szUserFriendlyName, USER_NDISWANBH);
		return TRUE;
	}

	if (_stricmp(szAdapterName, DEVICE_NDISWANIPV6) == 0)
	{
		strcpy (szUserFriendlyName, USER_NDISWANIPV6);
		return TRUE;
	}

	strcpy (szFriendlyNameKey, REGSTR_NETWORK_CONTROL_KEY);
	strcpy ((char*)szFriendlyNameKey + strlen(szFriendlyNameKey), &szAdapterName[strlen("\\Device\\")]);
	strcpy ((char*)szFriendlyNameKey + strlen(szFriendlyNameKey), REGSTR_VAL_CONNECTION);

	LONG lResult = RegOpenKeyExA (
						HKEY_LOCAL_MACHINE,
						szFriendlyNameKey,
						0,
						KEY_READ,
						&hKey
						);

	if (lResult == ERROR_SUCCESS)
	{
		lResult = RegQueryValueExA(hKey, REGSTR_NAME, NULL, &dwType, (LPBYTE)szUserFriendlyName, &dwUserFriendlyNameLength);
							
		RegCloseKey(hKey);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL
	CNdisApi::ConvertWindows9xAdapterName (
		LPCSTR szAdapterName,
		LPSTR szUserFriendlyName,
		DWORD dwUserFriendlyNameLength
		)
{
	HKEY		hKey;
	char		szFriendlyNameKey[MAX_PATH*2];
	DWORD		dwType;
	BOOL		bRet = TRUE;

	strcpy (szFriendlyNameKey, REGSTR_MSTCP_CLASS_NET);
	strcpy ((PCHAR)szFriendlyNameKey + strlen(szFriendlyNameKey), szAdapterName);

	LONG lResult = RegOpenKeyExA (
					HKEY_LOCAL_MACHINE,
					szFriendlyNameKey,
					0,
					KEY_READ,
					&hKey
					);

	if (lResult == ERROR_SUCCESS)
	{
		lResult = RegQueryValueExA(hKey, REGSTR_VAL_DRIVER_DESC, NULL, &dwType, (LPBYTE)szUserFriendlyName, &dwUserFriendlyNameLength);
		if (lResult != ERROR_SUCCESS)
		{
			bRet = FALSE;
		}
		RegCloseKey(hKey);
	}
	else
	{
		bRet = FALSE;
	}

	return bRet;
}

HANDLE __stdcall OpenFilterDriver ( const TCHAR* pszFileName )
{
	return (HANDLE)(new CNdisApi (pszFileName));
}
 
VOID __stdcall CloseFilterDriver ( HANDLE hOpen )
{
	delete (CNdisApi*)hOpen;
}

DWORD __stdcall GetDriverVersion ( HANDLE hOpen )
{
	if (!hOpen)
		return 0;

	CNdisApi* pApi = (CNdisApi*)(hOpen);
	
	return pApi->GetVersion ();
}

BOOL __stdcall GetTcpipBoundAdaptersInfo ( HANDLE hOpen, PTCP_AdapterList pAdapters )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen); 

	return pApi->GetTcpipBoundAdaptersInfo ( pAdapters );
}

BOOL __stdcall SendPacketToMstcp ( HANDLE hOpen, PETH_REQUEST pPacket )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen); 

	return pApi->SendPacketToMstcp ( pPacket );
}

BOOL __stdcall SendPacketToAdapter ( HANDLE hOpen, PETH_REQUEST pPacket )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen); 

	return pApi->SendPacketToAdapter ( pPacket );
}

BOOL __stdcall ReadPacket ( HANDLE hOpen, PETH_REQUEST pPacket )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen);

	return pApi->ReadPacket ( pPacket );
}

BOOL __stdcall SendPacketsToMstcp ( HANDLE hOpen, PETH_M_REQUEST pPackets )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen);

	return pApi->SendPacketsToMstcp ( pPackets );
}

BOOL __stdcall SendPacketsToAdapter ( HANDLE hOpen, PETH_M_REQUEST pPackets )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen);

	return pApi->SendPacketsToAdapter ( pPackets );
}

BOOL __stdcall ReadPackets ( HANDLE hOpen, PETH_M_REQUEST pPackets )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen);

	return pApi->ReadPackets ( pPackets );
}

BOOL __stdcall SetAdapterMode ( HANDLE hOpen, PADAPTER_MODE pMode )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen); 

	return pApi->SetAdapterMode ( pMode );
}

BOOL __stdcall GetAdapterMode ( HANDLE hOpen, PADAPTER_MODE pMode )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen); 

	return pApi->GetAdapterMode ( pMode );
}

BOOL __stdcall FlushAdapterPacketQueue ( HANDLE hOpen, HANDLE hAdapter )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen);
	
	return pApi->FlushAdapterPacketQueue ( hAdapter );
}

BOOL __stdcall GetAdapterPacketQueueSize ( HANDLE hOpen, HANDLE hAdapter , PDWORD pdwSize)

{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen);

	return pApi->GetAdapterPacketQueueSize ( hAdapter, pdwSize ); 
}

BOOL __stdcall SetPacketEvent ( HANDLE hOpen, HANDLE hAdapter, HANDLE hWin32Event )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen);
	
	return pApi->SetPacketEvent ( hAdapter, hWin32Event );
}

BOOL __stdcall SetWANEvent ( HANDLE hOpen, HANDLE hWin32Event )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen);
	
	return pApi->SetWANEvent ( hWin32Event );
}

BOOL __stdcall SetAdapterListChangeEvent ( HANDLE hOpen, HANDLE hWin32Event )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen);
	
	return pApi->SetAdapterListChangeEvent ( hWin32Event );
}

BOOL __stdcall NdisrdRequest ( HANDLE hOpen, PPACKET_OID_DATA OidData, BOOL Set )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen); 

	return pApi->NdisrdRequest ( OidData, Set );
}

BOOL __stdcall GetRasLinks ( HANDLE hOpen, HANDLE hAdapter, PRAS_LINKS pLinks )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen); 

	return pApi->GetRasLinks ( hAdapter, pLinks );
}

BOOL __stdcall SetHwPacketFilter ( HANDLE hOpen, HANDLE hAdapter, DWORD Filter )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen); 

	return pApi->SetHwPacketFilter ( hAdapter, Filter );
}

BOOL __stdcall GetHwPacketFilter ( HANDLE hOpen, HANDLE hAdapter, PDWORD pFilter )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen); 

	return pApi->GetHwPacketFilter ( hAdapter, pFilter );
}


BOOL __stdcall SetPacketFilterTable ( HANDLE hOpen, PSTATIC_FILTER_TABLE pFilterList )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen); 

	return pApi->SetPacketFilterTable (pFilterList);
}

BOOL __stdcall ResetPacketFilterTable ( HANDLE hOpen )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen); 

	return pApi->ResetPacketFilterTable ();
}

BOOL __stdcall GetPacketFilterTableSize ( HANDLE hOpen, PDWORD pdwTableSize )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen); 

	return pApi->GetPacketFilterTableSize (pdwTableSize);
}

BOOL __stdcall GetPacketFilterTable ( HANDLE hOpen, PSTATIC_FILTER_TABLE pFilterList )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen); 

	return pApi->GetPacketFilterTable (pFilterList);
}

BOOL __stdcall GetPacketFilterTableResetStats ( HANDLE hOpen, PSTATIC_FILTER_TABLE pFilterList )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen); 

	return pApi->GetPacketFilterTableResetStats (pFilterList);
}

BOOL __stdcall SetMTUDecrement ( DWORD dwMTUDecrement )
{
	return CNdisApi::SetMTUDecrement ( dwMTUDecrement );
}

DWORD __stdcall GetMTUDecrement ()
{
	return CNdisApi::GetMTUDecrement();	
}

BOOL __stdcall SetAdaptersStartupMode ( DWORD dwStartupMode )
{
	return CNdisApi::SetAdaptersStartupMode ( dwStartupMode );
}

DWORD __stdcall GetAdaptersStartupMode ()
{
	return CNdisApi::GetAdaptersStartupMode();	
}

BOOL __stdcall IsDriverLoaded ( HANDLE hOpen )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen); 

	return pApi->IsDriverLoaded();
}

DWORD __stdcall GetBytesReturned ( HANDLE hOpen )
{
	if (!hOpen)
		return FALSE;

	CNdisApi* pApi = (CNdisApi*)(hOpen); 

	return pApi->GetBytesReturned ();
}

// Helper routines
BOOL
	__stdcall
		ConvertWindowsNTAdapterName (
			LPCSTR szAdapterName,
			LPSTR szUserFriendlyName,
			DWORD dwUserFriendlyNameLength
			)
{
	return CNdisApi::ConvertWindowsNTAdapterName (
						szAdapterName,
						szUserFriendlyName,
						dwUserFriendlyNameLength
						);
}


BOOL
	__stdcall 
		ConvertWindows2000AdapterName (
			LPCSTR szAdapterName,
			LPSTR szUserFriendlyName,
			DWORD dwUserFriendlyNameLength
			)
{
	return CNdisApi::ConvertWindows2000AdapterName (
						szAdapterName,
						szUserFriendlyName,
						dwUserFriendlyNameLength
						);
}

BOOL
	__stdcall 
		ConvertWindows9xAdapterName (
			LPCSTR szAdapterName,
			LPSTR szUserFriendlyName,
			DWORD dwUserFriendlyNameLength
			)
{
	return CNdisApi::ConvertWindows9xAdapterName (
						szAdapterName,
						szUserFriendlyName,
						dwUserFriendlyNameLength
						);
}

