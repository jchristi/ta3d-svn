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
**  File: realfs.h
** Notes:
**   Zuzuf:  This module implements the real filesystem interface through the Archive layer
*/

#ifndef __TA3D_UTILS_VFS_REALFS_H__
# define __TA3D_UTILS_VFS_REALFS_H__

# include "archive.h"
# include <map>

namespace TA3D
{
    namespace UTILS
    {
        /*! \class RealFS
        **
        ** \brief abstract class defining the interface required to manipulate real files
        */
        class RealFS : public Archive
        {
        public:
            class RealFile : public Archive::File
            {
            public:
                String pathToFile;
            public:
                inline void setName(const String &name)   {  Archive::File::name = name; }
                inline void setParent(Archive *parent)   {  Archive::File::parent = parent; }
            };
        public:
            //! Constructor
            RealFS(const String &filename);
            //! Destructor
            virtual ~RealFS();

            /*!
            ** \brief Loads an archive
            */
            virtual void open(const String& filename);

            /*!
            ** \brief Just close the opened archive
            */
            virtual void close();

            /*!
            ** \brief Return the list of all files in the archive
            */
            virtual void getFileList(std::list<File*> &lFiles);

            /*!
            ** \brief
            */
            virtual byte* readFile(const String& filename, uint32* file_length = NULL);
            virtual byte* readFile(const File *file, uint32* file_length = NULL);

            /*!
            ** \brief
            ** \param filename
            ** \param start
            ** \param length
            ** \return
            */
            virtual byte* readFileRange(const String& filename, const uint32 start, const uint32 length, uint32 *file_length = NULL);
            virtual byte* readFileRange(const File *file, const uint32 start, const uint32 length, uint32 *file_length = NULL);

            /*!
            ** \brief returns true if using the cache is a good idea (real FS will return false)
            ** \return
            */
            virtual bool needsCaching();

        private:
            std::map<String, File*> files;

        public:
            static void finder(String::List &fileList, const String &path);
            static Archive* loader(const String &filename);
        }; // class RealFS
    } // namespace utils
} // namespace TA3D


#endif // __TA3D_UTILS_VFS_REALFS_H__