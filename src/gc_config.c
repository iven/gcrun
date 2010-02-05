/*
 *  gcrun - GTK+ Completion-Run Utility
 *  Copyright (C) 2010  Iven Day <ivenvd#gmail.com>
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gc_config.h"

static GKeyFile *gc_key_file;
static GData *gc_config;

static gboolean gc_key_file_init(void)
{
    GError *err = NULL;
    const gchar *search_path [2] = {g_get_user_config_dir (), GC_DATA_DIR};

    gc_key_file = g_key_file_new ();
    g_key_file_load_from_dirs (gc_key_file, GC_CONFIG_FILE_NAME,
            search_path, NULL,
            G_KEY_FILE_NONE, &err);
    if (err != NULL) {
        g_warning("Unable to load configuration: %s\n", err->message);
        g_error_free(err);
        return TRUE;
    }
    return FALSE;
}

static gchar **gc_key_file_get_keys(const char *group_name)
{
    GError *err = NULL;
    gchar **keys = NULL;
    keys = g_key_file_get_keys(gc_key_file, group_name, NULL, &err);
    if (err != NULL) {
        g_warning("Unable to load group %s: %s\n", group_name, err->message);
        g_error_free(err);
    }
    return keys;
}
    
gboolean gc_config_init (void)
{
    GError *err = NULL;
    gchar **keys = NULL, **p;
    g_datalist_init(&gc_config);

    if (gc_key_file_init()) {
        g_key_file_free (gc_key_file);
        return TRUE;
    }

    keys = gc_key_file_get_keys("Geometry");
    if (keys != NULL) {
        for (p = keys; *p != NULL; p++) {
            g_print("%s - %d\n", *p, g_key_file_get_integer(gc_key_file, "Geometry", *p, NULL));
        }
        g_strfreev(keys);
    }
    g_key_file_free (gc_key_file);
    return FALSE;
}

//gint gc_key_file_get_integer (const gchar *group, const gchar *key) {
//    GError *err = NULL;
//    gint value = g_key_file_get_integer (gc_key_file, group, key, &err);
//    if (err != NULL) {
//        gc_show_error (&err);
//        gc_key_file_set_integer (group, key, 0);
//        value = 0;
//    }
//    return value;
//}
//gboolean gc_key_file_get_boolean (const gchar *group, const gchar *key) {
//    GError *err = NULL;
//    gboolean value = g_key_file_get_boolean (gc_key_file, group, key, &err);
//    if (err != NULL) {
//        gc_show_error (&err);
//        gc_key_file_set_boolean (group, key, FALSE);
//        value = FALSE;
//    }
//    return value;
//}
//gchar *gc_key_file_get_value (const gchar *group, const gchar *key) {
//    GError *err = NULL;
//    gchar *value = g_key_file_get_value (gc_key_file, group, key, &err);
//    if (err != NULL) {
//        gc_show_error (&err);
//        gc_key_file_set_value (group, key, "");
//        value = "";
//    }
//    return value;
//}

