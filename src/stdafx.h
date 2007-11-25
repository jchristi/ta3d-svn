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
**  File: stdafx.h
** Notes:  **** PLEASE SEE README.txt FOR LICENCE AGREEMENT ****
**  Cire: *stdafx.h and stdafx.cpp will generate a pch file
**         (pre compiled headers).
**        *All other cpp files MUST include this file as its first include.
**        *No .h files should include this file.
**        *The goal is to include everything that we need from system, and
**          game libiries, ie everything we need external to our project.
*/

#pragma once

// Cire:
//   Right off the bat, lets get everything we will need from STL includes.
#include <cstdio>
#include <cstdlib>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

// Now lets get platform specific stuff.
#include "TA3D_Platform.h"

#if defined TA3D_PLATFORM_WINDOWS && defined TA3D_PLATFORM_MSVC
	#pragma warning(disable : 4554) 
	#pragma warning(disable : 4996)

	#pragma comment( lib, "tools/win32/libs/alleg.lib" )
	#pragma comment( lib, "tools/win32/libs/agl.lib" )
	#pragma comment( lib, "opengl32.lib" )
	#pragma comment( lib, "glu32.lib" )
	#pragma comment( lib, "glaux.lib" )
	#pragma comment( lib, "tools/win32/libs/glew32.lib" )

	#include "tools/win32/include/gl/glew.h"

// Cire:
//   I had to setup a pragma on c4312, 4005 warnings, because allgero include
//     was generating alot of compiler noise.
	#pragma warning( disable : 4312 )
	#pragma warning( disable : 4005 )

	#include <alleggl.h> // alleggl also includes allegro

// Cire:
//  restore warning states
	#pragma warning( default : 4005 )
	#pragma warning( default : 4312 )
#else 

		// Include the config options generated by the configure script
	#include "../config.h"

// Cire:
//   Other platfroms may wish to adjust how allegro is included, for
//   now its this way.
	#include <alleggl.h>
#endif

// Now lets get gl/glu support.
#include <GL/glu.h>

#include "jpeg/ta3d_jpg.h"

// Math include.
#include <math.h>
// Cire:
//   Malloc should not be platform specfic right???
#include <malloc.h>


// Cire:
//   string and wstring typedefs to make life easier.
typedef std::string		String;
typedef std::wstring	WString;
#define List			std::list
#define Vector			std::vector

// Cire:
//   Since byte seems to be common throughout the project we'll typedef here.
typedef uint8         byte;
typedef unsigned char uchar;
typedef signed char   schar;

// Floating point types, defined for consistencies sakes.
typedef float  real32;
typedef double real64;

#ifdef max
	#undef max
#endif

#ifdef min
	#undef min
#endif

template<class T> inline T max(T a, T b)	{	return (a > b) ? a : b;	}
template<class T> inline T min(T a, T b)	{	return (a > b) ? b : a;	}

#if defined TA3D_PLATFORM_WINDOWS && defined TA3D_PLATFORM_MSVC
// Cire:
//  The below functions don't exists within windows math routines.
	#define strcasecmp(x,xx) _stricmp( x, xx )

	static inline const real32 asinh( const real32 f )
	{
		return (real32)log( (real32)(f + sqrt( f * f + 1)) );
	}

	static inline const real32 acosh( const real32 f )
	{
		return (real32)log( (real32)( f + sqrt( f * f- 1)) );
	}

	static inline const real32 atanh( const real32 f)
	{
		return (real32)log( (real32)( (real32)(1.0f / f + 1.0f) /
			(real32)(1.0f / f - 1.0f))  ) / 2.0f;
	}
#endif

#if ((MAKE_VERSION(4, 2, 1) == MAKE_VERSION(ALLEGRO_VERSION, ALLEGRO_SUB_VERSION, ALLEGRO_WIP_VERSION)) && ALLEGRO_WIP_VERSION>0) \
	|| (MAKE_VERSION(4, 2, 1) < MAKE_VERSION(ALLEGRO_VERSION, ALLEGRO_SUB_VERSION, ALLEGRO_WIP_VERSION))
	#define FILE_SIZE	file_size_ex
#else
	#define FILE_SIZE	file_size
#endif

namespace TA3D
{
	// Below functions should make life easier.
	static inline String Lowercase( const String &szString )
	{
		static int (*pf)(int)= tolower;
		String szResult;

		szResult.resize( szString.length() );
		std::transform( szString.begin(), szString.end(), szResult.begin(), pf );

		return (szResult);
	}

	static inline String Uppercase( const String &szString )
	{
		static int (*pf)(int)= toupper;
		String szResult;

		szResult.resize( szString.length() );
		std::transform( szString.begin(), szString.end(), szResult.begin(), pf );

		return (szResult);
	}

	static inline String format( const char *fmt, ... )
	{
		if( !fmt )
			return "";

		int   result = -1, length = 256;
		char *buffer = 0;

		while( result == -1 || result > length )
		{
			va_list args;
			va_start(args, fmt);

			length <<= 1;

			if( buffer )
				delete [] buffer;

			buffer = new char [length + 1];

			memset(buffer, 0, length + 1);

#if defined TA3D_PLATFORM_WINDOWS && defined TA3D_PLATFORM_MSVC
			result = _vsnprintf(buffer, length, fmt, args);
#else
			result = vsnprintf(buffer, length, fmt, args);
#endif
			va_end(args);
		}

		String s( buffer );
		delete [] buffer;

		return s;
	}

	static inline String TrimString( String &sstring, String TrimChars = String( " \t\n\r") )
	{
		int nPos, rPos;
		String Result = String( sstring );

		// trim left
		nPos = (int)Result.find_first_not_of( TrimChars );

		if ( nPos > 0 )
			Result.erase(0, nPos);

		// trim right and return
		nPos = (int)Result.find_last_not_of( TrimChars );
		rPos = (int)Result.find_last_of( TrimChars );

		if ( rPos > nPos && rPos > -1)
			Result.erase( nPos+1, rPos-nPos );

		return Result;
	}

	static sint32 SearchString( const String &Search, const String &StringToSearch, bool IgnoreCase )
	{
		static const std::basic_string <char>::size_type NotFound = -1;
		std::basic_string <char>::size_type iFind;
		String sz1, sz2;

		if( IgnoreCase )
		{
			sz1 = Uppercase( Search );
			sz2 = Uppercase( StringToSearch );
		}
		else
		{
			sz1 = Search;
			sz2 = StringToSearch;
		}

		iFind = sz1.find( sz2 );

		return (( iFind == NotFound ) ? -1 : (sint32)iFind );
	}

	static String ReplaceString( const String &Search, const String &StringToSearch, const String &StringToReplace, bool IgnoreCase )
	{
		String Result = Search;
		int f = 0;
		
		while( (f = SearchString( Result, StringToSearch, IgnoreCase )) != -1 )
			Result = Result.replace( f, StringToSearch.size(), StringToReplace );

		return Result;
	}

	static inline bool StartsWith( const String &a, const String &b )
	{
		String y = Lowercase( a );
		String z = Lowercase( b );
		uint16 ai, bi;

		ai = (uint16)y.length();
		bi = (uint16)z.length();

		if( ai > bi )
			return  ( (y.compare( 0, bi, z ) == 0 ) ? true : false );
		else
			return ( (z.compare( 0, ai, y ) == 0 ) ? true : false );
	}

#if defined TA3D_PLATFORM_WINDOWS && defined TA3D_PLATFORM_MSVC
	static void ExtractPathFile( const String &szFullFileName, String &szFile, String &szDir )
	{
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		// Below extracts our path where the application is being run.
		::_splitpath_s( szFullFileName.c_str(), drive, dir, fname, ext );

		szFile = fname;
		szDir = drive;
		szDir += dir;
	}
#endif
	static inline String GetClientPath( void )
	{
		static bool bName = false;
		static String szName = "";

		if (!bName)
		{
#if defined TA3D_PLATFORM_WINDOWS && defined TA3D_PLATFORM_MSVC
			char fPath[ MAX_PATH ];
			String Tmp;

			::GetModuleFileNameA( NULL, fPath, MAX_PATH );

			ExtractPathFile( fPath, Tmp, szName );
#endif
			bName = true;
		}
     
		return szName;
	}
} 

// zuzuf: to prevent some warnings
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_TARNAME
#undef PACKAGE_STRING
#undef PACKAGE_VERSION
