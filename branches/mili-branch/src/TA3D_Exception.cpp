/*  TA3D, a remake of Total Annihilation
    Copyright (C) 2006  Roland BROCHARD

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
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA*/

/*
**  File: TA3D_Exception.cpp
** Notes:
**   Cire: See notes in TA3D_Exception.h
*/
#include "stdafx.h"
#include "TA3D_NameSpace.h"
#include "TA3D_Exception.h"
#include "paths.h"

#ifdef TA3D_PLATFORM_LINUX
	#include <errno.h>
#endif

namespace TA3D
{
	namespace EXCEPTION
	{
#ifdef TA3D_PLATFORM_WINDOWS
		typedef   DWORD         cErrorType;
		#define   GETSYSERROR()   GetLastError()
#else
		typedef   int            cErrorType;
		#define   GETSYSERROR()   errno
#endif
		static String      g_szGuardLog = "";
		static bool         ExceptionStatus = false;
		static uint16      IndentLevel = 0;

		const std::string GetGuardLog( void ) { return g_szGuardLog; }

		void GuardIncrementIndent( void ) { IndentLevel++; }


		const char *GuardIndentPadding(void)
		{
			static std::string x;

			x = "";
			for( uint16 i = 0; i < IndentLevel; i++ )
				x = x + "   ";

			return x.c_str();
		}

		String GuardGetSysError()
		{
			cErrorType m_LastError = GETSYSERROR();
#ifdef TA3D_PLATFORM_WINDOWS
			char szError[512];
			String Result;

			if( !FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM,
				0, m_LastError, 0, &szError[0], 511, NULL) )
				Result = String( "Unable to extract error code from system." );
			else
				Result = szError;
#else
			String Result = String( strerror( m_LastError ) );
#endif
			return ( Result );
		}

		void GuardDecrementIndent (void)
		{
			if (IndentLevel != 0)
				IndentLevel--;

			return;
		}

		void GuardReset( void )
		{
			IndentLevel = 0;
			g_szGuardLog = "";
		}

		void GuardLog( const std::string &szLog )
		{
			g_szGuardLog += szLog;
		}


		bool IsExceptionInProgress (void)
		{
			return (ExceptionStatus);
		}

		void SetExceptionStatus (bool E)
		{
			ExceptionStatus = E;
			return;
		}

		void GuardDisplayAndLogError()
		{
			String szErrReport;
			std::ofstream   m_File;
			String FileName = TA3D::Paths::Logs + "error.log";
			bool m_Logged = false;

			set_uformat( U_ASCII );		// Switch to good string format

			m_File.open( FileName.c_str(), std::ios::out | std::ios::trunc );
			if( m_File.is_open())
			{
				m_Logged = true;
				szErrReport = format( "***** Error Report *****\n%s", g_szGuardLog.c_str() );

				m_File << szErrReport;

				m_File.flush();
				m_File.close();
			}

			szErrReport = format( "A error has just occured within the application.\n%s\nPlease report this to our forums so that we might fix it.\n\nError:\n%s",
				( (m_Logged) ? "It was trapped and logged to error.txt." : "It was trapped but NOT logged." ),
				g_szGuardLog.c_str() );

	#ifdef TA3D_PLATFORM_WINDOWS
			::MessageBoxA( NULL, szErrReport.c_str(), "TA3D Application Error", MB_OK  | MB_TOPMOST | MB_ICONERROR );
	#else
			allegro_message( szErrReport.c_str() );
	#endif
		}
	} // namespace EXCEPTION
} // namespace TA3D 
