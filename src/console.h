/*  TA3D, a remake of Total Annihilation
    Copyright (C) 2005  Roland BROCHARD

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

/*----------------------------------------------------------------------\
|                               console.h                               |
|      contient les classes n�cessaires � la gestion d'une console dans |
| programme utilisant Allegro avec ou sans AllegroGL. La console        |
| dispose de sa propre proc�dure d'entr�e et d'affichage mais celle-ci  |
| n�cessite d'�tre appell�e manuellement pour �viter les probl�mes      |
| d�coulant d'un appel automatique par un timer.                        |
\----------------------------------------------------------------------*/

#ifndef __CLASSE_CONSOLE
#define __CLASSE_CONSOLE

#pragma once

#define CONSOLE_MAX_LINE	100

namespace TA3D
{
	namespace TA3D_DEBUG
	{
		class cConsole : protected cCriticalSection
		{
		private:
			std::list<String>	m_LastEntries;
			bool				m_Recording;
			FILE				*m_log;
			String				m_RecordFilename;
			uint16				m_MaxDisplayItems;
			real32				m_Visible;
			bool				m_Show;

			char				m_InputText[200];
			uint16				m_CurrentLine;
			uint32				m_CurrentTimer;
			bool				m_std_output;
			bool				m_log_close;

		private:
			void DumpStartupInfo();

		public:
			cConsole();
			cConsole( const String &file );
			cConsole( const FILE *file );

			~cConsole();

			void ToggleShow();

			bool activated();

			void StopRecording( void );
			void StartRecording( const char *file );
			void StartRecording( const FILE *file );

			void stdout_on()	{	m_std_output = true;	}
			void stdout_off()	{	m_std_output = false;	}

			void AddEntry(char *txt, ...);
			void AddEntry( String NewEntry );

			char *draw( TA3D::INTERFACES::GFX_FONT fnt, float dt, float fsize=8, bool force_show=false );
		}; //class cConsole
	} // namespace TA3D_DEBUG
} // namespace TA3D

#endif
