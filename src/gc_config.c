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
        g_warning(_("Unable to load configuration file: %s"), err->message);
        g_error_free(err);
        return TRUE;
    }
    return FALSE;
}

static void gc_value_replace_variable(gpointer *value)
{
    /* TODO: replace ${variable}s */
}

static gpointer gc_key_file_get_string_list(const gchar *group_name, const gchar *key, GError *err)
{
    gchar **values = NULL, **pvalue = NULL;
    GPtrArray *str_array;
    GString *string;
    values = g_key_file_get_string_list(gc_key_file, group_name, key, NULL, &err);
    if (err != NULL) {
        return NULL;
    }
    str_array = g_ptr_array_new();
    for (pvalue = values; *pvalue != NULL; pvalue++)
    {
        string = g_string_new(*pvalue);
        g_ptr_array_add(str_array, string);
    }
    return str_array;
}
static void gc_key_file_get_keys(const char *group_name,
        gpointer get_key(GKeyFile *, const gchar *, const gchar *, GError **))
{
    GError *err = NULL;
    gchar **keys = NULL, **pkey;
    gpointer value;
    g_debug(_("Loading group %s..."), group_name);
    keys = g_key_file_get_keys(gc_key_file, group_name, NULL, &err);
    if (err != NULL) {
        g_warning(_("Unable to load group %s: %s"), group_name, err->message);
        g_error_free(err);
        return;
    }
    for (pkey = keys; *pkey != NULL; pkey++) {
        g_debug(_("Loading key %s..."), *pkey);
        if (get_key == g_key_file_get_string_list) {
            value == gc_key_file_get_string_list(group_name, *pkey, err);
        } else {
            value = get_key(gc_key_file, group_name, *pkey, &err);
        }
        if (err != NULL) {
            g_warning(_("Unable to load key %s: %s"), *pkey, err->message);
            g_error_free(err);
            continue;
        }
        if (get_key == g_key_file_get_string) {
            gc_value_replace_variable(&value);
        }
        g_datalist_set_data(&gc_config, *pkey, value);
        g_debug(_("Loaded key %s."), *pkey);
    }
    g_strfreev(keys);
    g_debug(_("Loaded group %s."), group_name);
}

gboolean gc_config_init (void)
{
    g_datalist_init(&gc_config);

    if (gc_key_file_init()) {
        g_key_file_free (gc_key_file);
        return TRUE;
    }

    gc_key_file_get_keys("Environment", g_key_file_get_string);
    gc_key_file_get_keys("Geometry", g_key_file_get_integer);
    gc_key_file_get_keys("History", g_key_file_get_integer);
    gc_key_file_get_keys("Completion", g_key_file_get_integer);
    gc_key_file_get_keys("Switch", g_key_file_get_boolean);
    gc_key_file_get_keys("ProtocolHandler", g_key_file_get_string);
    gc_key_file_get_keys("ExtensionHandler", g_key_file_get_string);
    gc_key_file_get_keys("Terminal", g_key_file_get_string_list);
    gc_key_file_get_keys("Protocol", g_key_file_get_string_list);
    gc_key_file_get_keys("Extension", g_key_file_get_string_list);

    /* TODO: Process Protocol and Extension Handlers. */

    g_key_file_free (gc_key_file);
    return FALSE;
}

gint gc_config_get_integer(const char *key, const gint pre_value)
{
    gpointer retval = g_datalist_get_data(gc_config, key);
    return retval == NULL ? pre_value : (gint) retval;
}

gboolean gc_config_get_boolean(const char *key, const gboolean pre_value)
{
    gpointer retval = g_datalist_get_data(gc_config, key);
    return retval == NULL ? pre_value : (gboolean) retval;
}

gchar *gc_config_get_string(const char *key, const gchar *pre_value)
{
    /* TODO: Use GString instead of (gchar *)? */
    gpointer retval = g_datalist_get_data(gc_config, key);
    return retval == NULL ? pre_value : (gchar *) retval;
}

