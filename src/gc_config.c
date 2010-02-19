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

#define	Gc_Get_Key_Func(f)  ((GcGetKeyFunc) (f))			/*  */
typedef gpointer (*GcGetKeyFunc)(GKeyFile *, const gchar *, const gchar *, GError **);
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
    *value = (gpointer) string->str;
    g_string_free(string, FALSE);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  gc_config_process_handlers
 *  Description:  Associate protocols and extensions with handlers.
 * =====================================================================================
 */
static void gc_config_process_handlers(const gchar *group_name)
{
    gchar **keys = NULL, **pkey = NULL, **values = NULL, **pvalue;
    gchar *prefix, *new_key;
    const gchar *handler;
    prefix = g_strndup(group_name, 4);
    keys = (gchar **) gc_config_get_string_list(group_name);
    for (pkey = keys; *pkey != NULL; pkey++) {
        values = (gchar **) gc_config_get_string_list(*pkey);
        handler = gc_config_get_string_join("H", *pkey, NULL);
        if (handler == NULL || values == NULL) {
            continue;
        }
        for (pvalue = values; *pvalue != NULL; pvalue++) {
            new_key = g_strjoin("_", prefix, *pvalue, NULL);
            g_datalist_set_data(&gc_config, new_key, (gpointer) handler);
            g_free(new_key);
        }
        g_strfreev(values);
        g_datalist_remove_data(&gc_config, *pkey);
    }
    g_free(prefix);
    g_strfreev(keys);
    g_datalist_remove_data(&gc_config, group_name);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  gc_key_file_get_keys
 *  Description:  Get data from g_key_file to g_datalist.
 * =====================================================================================
 */
static void gc_key_file_get_keys(const char *group_name, GcGetKeyFunc get_key)
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
        if (get_key == Gc_Get_Key_Func(g_key_file_get_string_list)) {
            value = g_key_file_get_string_list(gc_key_file, group_name, *pkey, NULL, &err);
        } else {
            value = get_key(gc_key_file, group_name, *pkey, &err);
        }
        if (err != NULL) {
            g_warning(_("Unable to load key %s: %s"), *pkey, err->message);
            g_error_free(err);
        } else {
            if (get_key == Gc_Get_Key_Func(g_key_file_get_string)) {
                gc_value_replace_variable(&value);
            }
            g_datalist_set_data(&gc_config, *pkey, value); /* save it */
            g_debug(_("Loaded key %s."), *pkey);
        }
    }
    if (g_strcmp0(group_name, "Protocol") == 0 || g_strcmp0(group_name, "Extension") == 0) {
        g_datalist_set_data(&gc_config, group_name, keys); /* save keys for later use */
    } else {
        g_strfreev(keys);
    }
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
    gc_key_file_get_keys("Environment", Gc_Get_Key_Func(g_key_file_get_string));
    gc_key_file_get_keys("Geometry", Gc_Get_Key_Func(g_key_file_get_integer));
    gc_key_file_get_keys("History", Gc_Get_Key_Func(g_key_file_get_integer));
    gc_key_file_get_keys("Completion", Gc_Get_Key_Func(g_key_file_get_integer));
    gc_key_file_get_keys("Switch", Gc_Get_Key_Func(g_key_file_get_boolean));
    gc_key_file_get_keys("ProtocolHandler", Gc_Get_Key_Func(g_key_file_get_string));
    gc_key_file_get_keys("ExtensionHandler", Gc_Get_Key_Func(g_key_file_get_string));
    gc_key_file_get_keys("Terminal", Gc_Get_Key_Func(g_key_file_get_string_list));
    gc_key_file_get_keys("Protocol", Gc_Get_Key_Func(g_key_file_get_string_list));
    gc_key_file_get_keys("Extension", Gc_Get_Key_Func(g_key_file_get_string_list));

    gc_config_process_handlers("Protocol");
    gc_config_process_handlers("Extension");

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
        retval = (gpointer) pre_value;
        g_datalist_set_data(&gc_config, key, retval);
    }
    return (const gchar *) retval;
}

const gchar *gc_config_get_string_join(const gchar *prefix,
        const gchar *body, const gchar *pre_value)
{
    gchar *key = g_strjoin("_", prefix, body, NULL);
    const gchar *retval = gc_config_get_string(key, pre_value);
    g_free(key);
    return retval;
}

const gchar **gc_config_get_string_list(const gchar *key)
{
    gpointer retval = g_datalist_get_data(&gc_config, key);
    return (const gchar **) retval;
}

