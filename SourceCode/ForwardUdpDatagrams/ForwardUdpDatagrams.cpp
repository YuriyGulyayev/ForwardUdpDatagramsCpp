#pragma region

#include "StdAfx.h"

#pragma endregion
#pragma region {AbortIfFalse}.

inline void AbortIfFalse
   ( bool bool1
   )
{
   if( bool1 )
   {
   }
   else
   {
      abort();
   }
}

#pragma endregion
#pragma region {main}.

//int _tmain
//   ( int argc,
//     _TCHAR * argv[]
//   )
int main
   ( int argc,
// TODO 3 yg? Should this be something like {char const * const []}?
     char * argv[]
   )
{
   #pragma region

   if( argc >= 2 + 1 * 3 && argc <= 2 + 8 * 3 && static_cast< unsigned int >( argc - 2 ) % 3U == 0U )
   {
   }
   else
   {
      _cputws( L"Usage: ForwardUdpDatagrams.exe local_port local_IP_1 remote_IP_1 remote_port_1 local_IP_2 remote_IP_2 remote_port_2 ...\n"
                     L"You may specify up to 8 groups of local IP + remote IP + remote port.\n"
                   );
      return EXIT_FAILURE;
   }

   #pragma endregion
   #pragma region

   {
      WSADATA wsaData;
      AbortIfFalse( WSAStartup( MAKEWORD( 2, 2 ), ( & wsaData ) ) == 0 );
   }

   #pragma endregion
   #pragma region

   // yg? We probably don't need to configure the {SIO_ENABLE_CIRCULAR_QUEUEING} opcode for this socket.
   SOCKET receivingSocketHandle( socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) );

   AbortIfFalse( receivingSocketHandle != INVALID_SOCKET );

   {
      sockaddr_in sin;
      sin.sin_family = AF_INET;
      int localPortNumber( atoi( argv[ 1U ] ) );
      sin.sin_port = htons( static_cast< u_short >( localPortNumber ) );

      // yg? Any IP address. We don't support binding to a specific IP address.
      sin.sin_addr.S_un.S_addr = INADDR_ANY;

      // yg? Not explicitly enforcing exclusive address use.
      if( bind( receivingSocketHandle, reinterpret_cast< sockaddr const * >( & sin ), static_cast< int >( sizeof( sin ) ) ) == 0 )

      {
      }
      else
      {
         // We don't need to bother to close sockets.

         _cputws( L"{bind} failed\n" );
         return EXIT_FAILURE;
      }
   }

   #pragma endregion
   #pragma region

   SOCKET sendingSocketsHandle[ 8U ];
   SOCKET * sendingSocketsHandleEnd( sendingSocketsHandle );

   {
      int counter1( 2 );
      do
      {
         ( * sendingSocketsHandleEnd ) = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
         AbortIfFalse( ( * sendingSocketsHandleEnd ) != INVALID_SOCKET );

         {
            DWORD dword1( static_cast< DWORD >( TRUE ) );
            AbortIfFalse( setsockopt( ( * sendingSocketsHandleEnd ),
                                                    IPPROTO_UDP,
                                                    UDP_NOCHECKSUM,
                                                    reinterpret_cast< char const * >( & dword1 ),
                                                    static_cast< int >( sizeof( dword1 ) )
                                                  ) ==
                                 0
                               );
         }

         //{
         //   u_long u_long1( 1U );
         //   AbortIfFalse( ioctlsocket( ( * sendingSocketsHandleEnd ), FIONBIO, ( & u_long1 ) ) == 0 );
         //}

         {
            sockaddr_in sin;
            sin.sin_family = AF_INET;
            
            // No need to call {htons}.
            sin.sin_port = static_cast< u_short >( 0U );

            // yg? Bug:
            // <Quote>
            // In some applications, you may want to explicitly check the input string for the broadcast address "255.255.255.255,"
            // since the return value from inet_addr() for this address is the same as SOCKET_ERROR.
            // </Quote>
            sin.sin_addr.S_un.S_addr = inet_addr( argv[ static_cast< unsigned int >( counter1 ) ] );

            if( bind( ( * sendingSocketsHandleEnd ), reinterpret_cast< sockaddr const * >( & sin ), static_cast< int >( sizeof( sin ) ) ) == 0 )
            {
            }
            else
            {
               // We don't need to bother to close sockets.

               _cputws( L"{bind} failed\n" );
               return EXIT_FAILURE;
            }
         }

         {
            sockaddr_in sin;
            sin.sin_family = AF_INET;
            int remotePortNumber( atoi( argv[ static_cast< unsigned int >( counter1 + 2 ) ] ) );
            sin.sin_port = htons( static_cast< u_short >( remotePortNumber ) );

            // yg? Bug:
            // <Quote>
            // In some applications, you may want to explicitly check the input string for the broadcast address "255.255.255.255,"
            // since the return value from inet_addr() for this address is the same as SOCKET_ERROR.
            // </Quote>
            sin.sin_addr.S_un.S_addr = inet_addr( argv[ static_cast< unsigned int >( counter1 + 1 ) ] );
      
            if( connect( ( * sendingSocketsHandleEnd ), reinterpret_cast< sockaddr const * >( & sin ), static_cast< int >( sizeof( sin ) ) ) ==
                0
              )
            {
            }
            else
            {
               // We don't need to bother to close sockets.

               _cputws( L"{connect} failed\n" );
               return EXIT_FAILURE;
            }
         }

         // This won't exceed array bounds as we validated {argc}.
         ++ sendingSocketsHandleEnd;

         counter1 += 3;
      }
      while( counter1 < argc );
   }

   _cputws( L"started successfully\n" );

   #pragma endregion
   #pragma region

   for( ; ; )
   {
      // yg? Let's hope this is big enough.
      char buffer1[ 64U * 1024U ];

      // Using default receive timeout, which is infinity.
      // After a timeout the socket is left in an indeterminate state.
      int n1( recv( receivingSocketHandle, buffer1, static_cast< int >( sizeof( buffer1 ) ), 0 ) );

      if( n1 >= 0 )
      {
      }
      else
      {
         _cputws( L"{recv} failed\n" );
         Sleep( 1U );
         continue;
      }

      {
#if( ! defined( NDEBUG ) )
         unsigned __int64 timeSpan1( __rdtsc() );
#endif

         {
            SOCKET * sendingSocketsHandleItemPointer( sendingSocketsHandle );
            do
            {
               int n2( send( ( * sendingSocketsHandleItemPointer ), buffer1, n1, 0 ) );

               if( n2 == n1 )
               {
               }
               else
               {
                  _cputws( L"{send} failed\n" );
               }
            }
            while( ( ++ sendingSocketsHandleItemPointer ) != sendingSocketsHandleEnd );
         }

#if( ! defined( NDEBUG ) )
         timeSpan1 = __rdtsc() - timeSpan1;

         // yg? Using current thread locale.
         _cwprintf
            ( L"%u\n",
              static_cast< unsigned int >( ( timeSpan1 * 1000000U + 2400000000U / 2U ) / 2400000000U )
            );
#endif
      }
   }

   #pragma endregion
}

#pragma endregion
