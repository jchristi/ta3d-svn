
#include "settings.h"
#include "../logs/logs.h"
#include "paths.h"
#include "files.h"
#include "../TA3D_NameSpace.h"
#include "../ta3dbase.h"
#include "../languages/i18n.h"
#include <fstream>
#include "tdf.h"



namespace TA3D
{
namespace Settings
{



    bool Backup(const String& filename)
    {
        LOG_INFO(LOG_PREFIX_SETTINGS << "Making a backup for `" << filename << "`...");
        if (Paths::Files::Copy(filename, filename + ".bak"))
        {
            LOG_INFO(LOG_PREFIX_SETTINGS << "The backup is done.");
            return true;
        }
        LOG_WARNING(LOG_PREFIX_SETTINGS << "Impossible to make the backup.");
        return false;
    }


    bool Save()
    {
        lp_CONFIG->Lang = LANG;
        if (!TA3D::VARS::lp_CONFIG)
            return false;

        // Make a copy that can be restored if TA3D does not start any more
        TA3D::Settings::Backup(TA3D::Paths::ConfigFile);

        TA3D::VARS::lp_CONFIG->last_MOD = TA3D::VARS::TA3D_CURRENT_MOD;

        String s;
        s << "// TA3D Settings\n"
            << "// Auto-generated by ta3d: " << Logs::info.date() << "\n"
            << "\n"
            << "[TA3D]\n"
            << "{\n"
            << "                FPS Limit = " << TA3D::VARS::lp_CONFIG->fps_limit << "; // -1 means `unlimited`\n"
            << "                 Shadow R = " << TA3D::VARS::lp_CONFIG->shadow_r << ";\n"
            << "              Time Factor = " << TA3D::VARS::lp_CONFIG->timefactor << ";\n"
            << "           Shadow Quality = " << TA3D::VARS::lp_CONFIG->shadow_quality << "; // 0..100\n"
            << "           Priority Level = " << TA3D::VARS::lp_CONFIG->priority_level << "; // 0, 1, 2\n"
            << "                     FSAA = " << TA3D::VARS::lp_CONFIG->fsaa << ";\n"
            << "                 Language = " << TA3D::VARS::lp_CONFIG->Lang << ";\n"
            << "            Water Quality = " << TA3D::VARS::lp_CONFIG->water_quality << "; // 0..4\n"
            << "             Screen Width = " << TA3D::VARS::lp_CONFIG->screen_width << ";\n"
            << "            Screen Height = " << TA3D::VARS::lp_CONFIG->screen_height << ";\n"
            << "              Color Depth = " << (int)TA3D::VARS::lp_CONFIG->color_depth << ";\n"
            << "                 Show FPS = " << TA3D::VARS::lp_CONFIG->showfps << ";\n"
            << "           Show Wireframe = " << TA3D::VARS::lp_CONFIG->wireframe << ";\n"
            << "           Show particles = " << TA3D::VARS::lp_CONFIG->particle << ";\n"
            << " Show explosion particles = " << TA3D::VARS::lp_CONFIG->explosion_particles << ";\n"
            << "               Show Waves = " << TA3D::VARS::lp_CONFIG->waves << ";\n"
            << "             Show Shadows = " << TA3D::VARS::lp_CONFIG->shadow << ";\n"
            << "        Show Height Lines = " << TA3D::VARS::lp_CONFIG->height_line << ";\n"
            << "          Show FullScreen = " << TA3D::VARS::lp_CONFIG->fullscreen << ";\n"
            << "           Detail Texture = " << TA3D::VARS::lp_CONFIG->detail_tex << ";\n"
            << "     Draw Console Loading = " << TA3D::VARS::lp_CONFIG->draw_console_loading << ";\n"
            << "              Last Script = " << ReplaceChar( TA3D::VARS::lp_CONFIG->last_script, '\\', '/' ) << ";\n"
            << "                 Last Map = " << ReplaceChar( TA3D::VARS::lp_CONFIG->last_map, '\\', '/' ) << ";\n"
            << "                 Last FOW = " << (int)TA3D::VARS::lp_CONFIG->last_FOW << ";\n"
            << "                 Last MOD = " << TA3D::VARS::lp_CONFIG->last_MOD << ";\n"
            << "              Player name = " << TA3D::VARS::lp_CONFIG->player_name << ";\n"
            << "         Camera Zoom Mode = " << (int)TA3D::VARS::lp_CONFIG->camera_zoom << ";\n"
            << "     Camera Default Angle = " << TA3D::VARS::lp_CONFIG->camera_def_angle << ";\n"
            << "    Camera Default Height = " << TA3D::VARS::lp_CONFIG->camera_def_h << ";\n"
            << "        Camera Zoom Speed = " << TA3D::VARS::lp_CONFIG->camera_zoom_speed << ";\n"
            << "                     Skin = " << TA3D::VARS::lp_CONFIG->skin_name << ";\n"
            << "        Use Texture Cache = " << TA3D::VARS::lp_CONFIG->use_texture_cache << ";\n"
            << "               Net Server = " << TA3D::VARS::lp_CONFIG->net_server << "; // default: ta3d.darkstars.co.uk\n"
            << "               Render Sky = " << TA3D::VARS::lp_CONFIG->render_sky << ";\n"
            << "       Low Definition Map = " << TA3D::VARS::lp_CONFIG->low_definition_map << ";\n"
            << "}\n";

        if (Paths::Files::SaveToFile(TA3D::Paths::ConfigFile, s))
        {
            LOG_INFO(LOG_PREFIX_SETTINGS << "The settings has been saved.");
            return true;
        }
        LOG_ERROR(LOG_PREFIX_SETTINGS << "Impossible to write settings: `" << TA3D::Paths::ConfigFile << "`");
        return false;
    }



    bool Restore(const String& filename) 
    {
        LOG_INFO(LOG_PREFIX_SETTINGS << "Restoring the backup for `" << filename << "`...");
        if (Paths::Files::Copy(filename + ".bak", filename))
        {
            LOG_INFO(LOG_PREFIX_SETTINGS << "The settings have been restored.");
            return true;
        }
        LOG_WARNING(LOG_PREFIX_SETTINGS << "Impossible to restore the settings.");
        return false;
    }


    bool Load()
    {
        TDFParser cfgFile;
        if (!cfgFile.loadFromFile(TA3D::Paths::ConfigFile))
        {
            LOG_ERROR(LOG_PREFIX_SETTINGS << "Impossible to load the settings from `" << TA3D::Paths::ConfigFile << "`");
            return false;
        }

        TA3D::VARS::lp_CONFIG->fps_limit = cfgFile.pullAsFloat("TA3D.FPS Limit");
        TA3D::VARS::lp_CONFIG->shadow_r  = cfgFile.pullAsFloat("TA3D.Shadow R");
        TA3D::VARS::lp_CONFIG->timefactor = cfgFile.pullAsFloat("TA3D.Time Factor");

        TA3D::VARS::lp_CONFIG->shadow_quality = cfgFile.pullAsInt("TA3D.Shadow Quality");
        TA3D::VARS::lp_CONFIG->priority_level = cfgFile.pullAsInt("TA3D.Priority Level");
        TA3D::VARS::lp_CONFIG->fsaa = cfgFile.pullAsInt("TA3D.FSAA");
        TA3D::VARS::lp_CONFIG->Lang = cfgFile.pullAsInt("TA3D.Language");
        TA3D::VARS::lp_CONFIG->water_quality = cfgFile.pullAsInt("TA3D.Water Quality");
        TA3D::VARS::lp_CONFIG->screen_width = cfgFile.pullAsInt("TA3D.Screen Width");
        TA3D::VARS::lp_CONFIG->screen_height = cfgFile.pullAsInt("TA3D.Screen Height");
        TA3D::VARS::lp_CONFIG->color_depth = cfgFile.pullAsInt("TA3D.Color Depth", 32);

        TA3D::VARS::lp_CONFIG->showfps = cfgFile.pullAsBool("TA3D.Show FPS");
        TA3D::VARS::lp_CONFIG->wireframe = cfgFile.pullAsBool("TA3D.Show Wireframe");
        TA3D::VARS::lp_CONFIG->explosion_particles = cfgFile.pullAsBool("TA3D.Show explosion particles", true);
        TA3D::VARS::lp_CONFIG->particle = cfgFile.pullAsBool("TA3D.Show particles");
        TA3D::VARS::lp_CONFIG->waves = cfgFile.pullAsBool("TA3D.Show Waves");
        TA3D::VARS::lp_CONFIG->shadow = cfgFile.pullAsBool("TA3D.Show Shadows");
        TA3D::VARS::lp_CONFIG->height_line = cfgFile.pullAsBool("TA3D.Show Height Lines");
        TA3D::VARS::lp_CONFIG->fullscreen = cfgFile.pullAsBool("TA3D.Show FullScreen", false);
        TA3D::VARS::lp_CONFIG->detail_tex = cfgFile.pullAsBool("TA3D.Detail Texture");
        TA3D::VARS::lp_CONFIG->draw_console_loading = cfgFile.pullAsBool("TA3D.Draw Console Loading");

        TA3D::VARS::lp_CONFIG->last_script = cfgFile.pullAsString("TA3D.Last Script", "scripts\\default.c").convertSlashesIntoAntiSlashes();
        TA3D::VARS::lp_CONFIG->last_map = cfgFile.pullAsString("TA3D.Last Map", "").convertSlashesIntoAntiSlashes();
        TA3D::VARS::lp_CONFIG->last_FOW = cfgFile.pullAsInt("TA3D.Last FOW", 0);
        TA3D::VARS::lp_CONFIG->last_MOD = cfgFile.pullAsString("TA3D.Last MOD", "");

        TA3D::VARS::lp_CONFIG->camera_zoom = cfgFile.pullAsInt("TA3D.Camera Zoom Mode", ZOOM_NORMAL);
        TA3D::VARS::lp_CONFIG->camera_def_angle = cfgFile.pullAsFloat("TA3D.Camera Default Angle", 63.44f);
        TA3D::VARS::lp_CONFIG->camera_def_h = cfgFile.pullAsFloat("TA3D.Camera Default Height", 200.0f);
        TA3D::VARS::lp_CONFIG->camera_zoom_speed = cfgFile.pullAsFloat("TA3D.Camera Zoom Speed", 1.0f);

        TA3D::VARS::lp_CONFIG->use_texture_cache = cfgFile.pullAsBool("TA3D.Use Texture Cache", false);

        TA3D::VARS::lp_CONFIG->skin_name = cfgFile.pullAsString("TA3D.Skin", "");

        TA3D::VARS::lp_CONFIG->net_server = cfgFile.pullAsString("TA3D.Net Server", "ta3d.darkstars.co.uk");

        TA3D::VARS::TA3D_CURRENT_MOD = TA3D::VARS::lp_CONFIG->last_MOD;

        TA3D::VARS::lp_CONFIG->player_name = cfgFile.pullAsString("TA3D.Player name", "player");

        TA3D::VARS::lp_CONFIG->render_sky = cfgFile.pullAsBool("TA3D.Render Sky", true);
        
        TA3D::VARS::lp_CONFIG->low_definition_map = cfgFile.pullAsBool("TA3D.Low Definition Map", false);

        LANG = lp_CONFIG->Lang;
        // Apply settings for the current language
        I18N::Instance()->currentLanguage(lp_CONFIG->Lang);

        LOG_INFO(LOG_PREFIX_SETTINGS << "Loaded from `" << TA3D::Paths::ConfigFile << "`");
        return true;
    }



} // namespace Settings
} // namespace TA3D
