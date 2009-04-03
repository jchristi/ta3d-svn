
#include "solo.h"
#include <list>
#include "../../misc/paths.h"
#include "../gamedata.h"
#include "../../TA3D_Exception.h"
#include "../../restore.h"
#include "../../ta3dbase.h"
#include "../../misc/paths.h"

// TODO Must be removed
#include "../../menu.h"


using namespace TA3D::Exceptions;


namespace TA3D
{
namespace Menus
{

    
    bool Solo::Execute()
    {
        Solo m;
        return m.execute();
    }



    Solo::Solo()
        :Abstract()
    {}

    Solo::~Solo()
    {}
    
    bool Solo::doInitialize()
    {
        loadAreaFromTDF("solo", "gui/solo.area");
        return true;
    }

    void Solo::doFinalize()
    {
        // Do nothing
    }


    void Solo::waitForEvent()
    {
        bool keyIsPressed(false);
        do
        {
            // Get if a key was pressed
            keyIsPressed = keypressed();
            // Grab user events
            pArea->check();
            // Wait to reduce CPU consumption 
            rest(TA3D_MENUS_RECOMMENDED_TIME_MS_FOR_RESTING);

        } while (pMouseX == mouse_x && pMouseY == mouse_y && pMouseZ == mouse_z && pMouseB == mouse_b
                 && mouse_b == 0
                 && !key[KEY_ENTER] && !key[KEY_ESC] && !key[KEY_SPACE] && !key[KEY_C]
                 && !keyIsPressed && !pArea->scrolling);
    }


    bool Solo::maySwitchToAnotherMenu()
    {
        // Exit
        if (key[KEY_ESC] || pArea->get_state("solo.b_back"))
            return true;

        // All savegames
        if (pArea->get_state("solo.b_load") && pArea->get_object("load_menu.l_file") )
            return doDisplayAllSavegames();

        // Load the selected savegame
        if (pArea->get_state("load_menu.b_load"))
            return doGoMenuLoadSingleGame();

        // Campaign
        if (pArea->get_state("solo.b_campaign") || key[KEY_C])
            return doGoMenuCompaign();

        // Skirmish
        if (pArea->get_state("solo.b_skirmish") || key[KEY_ENTER])
            return doGoMenuSkirmish();

        return false;
    }

    bool Solo::doGoMenuSkirmish()
    {
        glPushMatrix();
        setup_game();
        glPopMatrix();
        return false;
    }

    bool Solo::doGoMenuCompaign()
    {
        glPushMatrix();
        campaign_main_menu();
        glPopMatrix();
        return false;
    }

    bool Solo::doDisplayAllSavegames()
    {
        GUIOBJ* obj = pArea->get_object("load_menu.l_file");
        if (obj)
        {
            std::list<String> fileList;
            Paths::Glob(fileList, TA3D::Paths::Savegames + "*.sav");
            fileList.sort();
            obj->Text.clear();
            obj->Text.reserve(fileList.size());
            for (std::list<String>::const_iterator i = fileList.begin(); i != fileList.end(); ++i)
            {
            	// Remove the Savegames path, leaving just the bare file names
                obj->Text.push_back(Paths::ExtractFileName(*i));
            }
        }
        else
        {
            LOG_ERROR("Impossible to get an area object : `load_menu.l_file`");
        }
        return false;
    }

    bool Solo::doGoMenuLoadSingleGame()
    {
        pArea->set_state("load_menu.b_load", false);
        GUIOBJ* guiObj = pArea->get_object("load_menu.l_file");
        if (!guiObj)
        {
            LOG_ERROR("Impossible to get an area object : `load_menu.l_file`");
        }
        else
        {
            if (guiObj->Pos >= 0 && guiObj->Pos < guiObj->Text.size())
            {
                GameData game_data;
                load_game_data(TA3D::Paths::Savegames + guiObj->Text[guiObj->Pos], &game_data);

                if (!game_data.saved_file.empty())
                {
                    gfx->unset_2D_mode();
                    GuardStart(play);
                    play(&game_data);
                    GuardCatch();
                    if (IsExceptionInProgress()) // if guard threw an error this will be true.
                    {
                        GuardDisplayAndLogError();   // record and display the error.
                        LOG_CRITICAL("Exception caught. Aborting now.");
                    }
                    gfx->set_2D_mode();
                    gfx->ReInitTexSys();
                }
            }
        }
        return false;
    }

} // namespace Menus
} // namespace TA3D

