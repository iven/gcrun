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

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  gc_key_file_init
 *  Description:  Just open the configuration file.
 * =====================================================================================
 */
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

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  gc_value_replace_variable
 *  Description:  Replace ${variable}s with actual environment values.
 * =====================================================================================
 */
static void gc_value_replace_variable(gpointer *value)
{
    GString *string = g_string_new("");
    gchar *p_str, *p_found, *env_key;
    const gchar *env_val;

    p_str = (gchar *) *value;
    p_found = g_strstr_len(p_str, -1, "${");    /* find ${ */
    while (p_found != NULL) {                   /* found ${ */
        g_string_append_len(string, p_str, p_found - p_str); /* copy chars before ${ */
        p_str = p_found;
        p_found = g_strstr_len(p_str, -1, "}"); /* find } */
        if (p_found != NULL) {                  /* found } */
            env_key = g_strndup(p_str + 2, p_found - p_str - 2);
            env_val = gc_config_get_string_join("Env", env_key, NULL);
            g_free(env_key);
            if (env_val != NULL) {              /* got the value */
                g_string_append(string, env_val);
            } else {                            /* variable doesn't exist */
                g_string_append_len(string, p_str, p_found + 1 - p_str);
            }
            p_str = p_found + 1;                /* search ${ again */
            p_found = g_strstr_len(p_str, -1, "${");
        }
    }
    g_string_append(string, p_str);             /* copy remaining chars */
    g_free(*value);
//    g_print("%s\n", string->str);
    *value = (gpointer) string;                 /* store with GString */
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  gc_key_file_get_string_list
 *  Description:  Convert gchar** to a GPtrArray which consists with GString.
 * =====================================================================================
 */
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

    if (gc_key_file_init()) {                   /* error occurs */
        g_key_file_free (gc_key_file);
        return TRUE;
    }
    /*-----------------------------------------------------------------------------
     *  Load all the key groups from the configuration file
     *-----------------------------------------------------------------------------*/
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
    gpointer retval = g_datalist_get_data(&gc_config, key);
    if (retval == NULL) {
        retval = GINT_TO_POINTER(pre_value);
        g_datalist_set_data(&gc_config, key, retval);
    }
    return GPOINTER_TO_INT(retval);
}

gboolean gc_config_get_boolean(const char *key, const gboolean pre_value)
{
    gpointer retval = g_datalist_get_data(&gc_config, key);
    if (retval == NULL) {
        retval = GINT_TO_POINTER(pre_value);
        g_datalist_set_data(&gc_config, key, retval);
    }
    return GPOINTER_TO_INT(retval);
}

const gchar *gc_config_get_string(const gchar *key, const gchar *pre_value)
{
    gpointer retval = g_datalist_get_data(&gc_config, key);
    if (retval == NULL) {
        if (pre_value == NULL) {
            return NULL;
        } else {
            retval = (gpointer) g_string_new(pre_value);
            g_datalist_set_data(&gc_config, key, retval);
        }
    }
    return ((GString *) retval)->str;
}

const gchar *gc_config_get_string_join(const gchar *prefix,
        const gchar *body, const gchar *pre_value)
{
    gchar *key = g_strjoin("_", prefix, body, NULL);
    const gchar *retval = gc_config_get_string(key, pre_value);
    g_free(key);
    return retval;
}
