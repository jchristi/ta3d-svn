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
#ifndef __TA3D_INGAME_MENUS_BASE_H__
# define __TA3D_INGAME_MENUS_BASE_H__

# include "../../stdafx.h"
# include <string>
# include <memory>
# include "../../gui.h"
# include "../../gfx/gui/area.h"
# include "../../misc/string.h"
# include <yuni/core/smartptr.h>


# define TA3D_MENUS_RECOMMENDED_TIME_MS_FOR_RESTING  8

namespace TA3D
{

/*!
** \brief All kinds of menu that a player might encounter
*/
namespace Menus
{


	/*!
	** \brief Abstract class to manage a single menu
	*/
	class Abstract
	{
	public:
		//! \name Constructor & Destructor
		//@{
		//! Default constructor
		Abstract();
		//! Destructor
		virtual ~Abstract() {}
		//@}


		/*!
		** \brief Execute the menu
		**
		** \return True if the operation succeeded, false otherwise
		**
		** \see doInitialize()
		** \see doExecute()
		** \see doFinalize()
		*/
		bool execute();

	protected:
		/*!
		** \brief Reset all internal variables
		** \return True if the operation succeeded, False otherwise
		** \see doExecute
		** \see doFinalize()
		*/
		virtual bool doInitialize() = 0;

		/*!
		** \brief Execute the local implementation for the menu
		** \return True if the operation succeeded
		** \warning This method will not be called if doInitialize() has previously returned false
		** \see doInitialize()
		*/
		virtual bool doExecute();

		/*!
		** \brief Release all internal variables
		**
		** This method is called even if doInitialize() has previously returned false
		**
		** \see doInitialize()
		*/
		virtual void doFinalize() = 0;

		/*!
		** \brief Wait for an user event (mouse, keyboard...)
		*/
		virtual void waitForEvent() = 0;

		/*!
		** \brief Init the area	with a given TDF filename
		**
		** \see AREA::load_tdf()
		** \see pArea
		*/
		void loadAreaFromTDF(const String& caption, const String& relFilename);

		/*!
		** \brief Execute another menu according the user inputs
		** \return True if the execution should be aborted, false otherwise
		*/
		virtual bool maySwitchToAnotherMenu() = 0;

		/*!
		** \brief Redraw the entire screen using the Area
		** \see pArea
		*/
		virtual void redrawTheScreen();

		/*!
		**
		*/
		virtual bool doLoop();

		/*!
		** \brief Destroy a texture if needed and reassign it
		** \param textVar Reference to the texture to destroy (index)
		** \param newValue The newValue to assign. `0` means NULL
		*/
		static void ResetTexture(GLuint& textVar, const GLuint newValue = 0);


	protected:
		//! Our Window handle
		AREA::Ptr pArea;

		//! Last value of `mouse_x`
		int pMouseX;
		//! last value of `mouse_y`
		int pMouseY;
		//! Last value of `mouse_z`
		int pMouseZ;
		//! Last value of `mouse_b`
		int pMouseB;

	private:
		/*!
		** \brief Call doInitialize() using Guards functions
		** \see doInitialize()
		*/
		bool doGuardInitialize();
		/*!
		** \brief Call doExecute() using Guards functions
		** \see doExecute()
		*/
		bool doGuardExecute();
		/*!
		** \brief Call doFinalize() using Guards functions
		** \see doFinalize()
		*/
		void doGuardFinalize();

	private:
		//! Cached value for the current class name
		String pTypeName;

	}; // class Abstract




} // namespace Menus
} // namespace TA3D


#endif // __TA3D_INGAME_MENUS_BASE_H__