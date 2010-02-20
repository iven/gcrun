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

#include	<string.h>
#include	"gc_main_window.h"
#include	"gc_entry.h"
#include	"gc_config.h"

static gchar *gc_str_replace(const gchar *text,
        const gchar *before, const gchar *after)
{
    GString *string;
    const gchar *p_str, *p_found;
    gchar *retval;
    string = g_string_new("");
    p_str = text;
    p_found = g_strstr_len(p_str, -1, before);
    while (p_found != NULL) {
        g_string_append_len(string, p_str, p_found - p_str);
        g_string_append(string, after);
        p_str = p_found + 2;
        p_found = g_strstr_len(p_str, -1, before);
    }
    g_string_append(string, p_str);
    retval = string->str;
    g_string_free(string, FALSE);
    return retval;
}

static gchar *gc_str_first_token(const gchar *text)
{
    const gchar *p_found;
    p_found = text;
    while (*p_found != '\0' && *p_found != ' ' && *p_found != '\t') {
        p_found++;
    }
    return g_strndup(text, p_found - text);
}

static gchar *gc_prot_parse(const gchar *text)
{
    gchar *scheme, *replaced_u, *replaced_s = NULL;
    const gchar *handler;

    scheme = g_uri_parse_scheme(text);
    handler = gc_config_get_string_join("Prot", scheme, NULL);
    if (handler != NULL) {
        replaced_u = gc_str_replace(handler, "%u", text);
        replaced_s = gc_str_replace(replaced_u, "%s",
                text + strlen(scheme) + 1);
        g_free(replaced_u);
    }
    g_free(scheme);
    return replaced_s;
}

static gchar *gc_exte_parse(const gchar *text)
{
    gchar *token, *ext, *replaced_s = NULL;
    const gchar *handler;

    token = gc_str_first_token(text);
    ext = g_strrstr(token, ".");
    if (ext != NULL) {
        handler = gc_config_get_string_join("Exte", ext + 1, NULL);
        if (handler != NULL) {
            replaced_s = gc_str_replace(handler, "%s", token);
        }
    }
    g_free(token);
    return replaced_s;
}

static gboolean on_destroy(GtkWidget *entry, gpointer data)
{
    gtk_main_quit();
    return FALSE;
}

static gboolean on_activate(GtkWidget *entry, gpointer data)
{
    const gchar *text, *termexec;
    gchar *cmd, *token, *tmp;
    const gchar **ter_always, **p_ter;

    text = gtk_entry_get_text(GTK_ENTRY(entry));
    if ((cmd = gc_prot_parse(text)) == NULL && 
            (cmd = gc_exte_parse(text)) == NULL) {
        cmd = g_strdup(text);
    }
    if (!gc_entry_get_in_term(entry)) {
        ter_always = gc_config_get_string_list("Ter_Always");
        token = gc_str_first_token(cmd);
        for (p_ter = ter_always; *p_ter != NULL; p_ter++) {
            if (g_str_equal(*p_ter, token)) {
                gc_entry_set_in_term(entry, TRUE);
                break;
            }
        }
        g_free(token);
    }
    if (gc_entry_get_in_term(entry)) {
        termexec = gc_config_get_string("Env_TermExec", NULL);
        if (termexec != NULL) {
            tmp = gc_str_replace(termexec, "%s", cmd);
            g_free(cmd);
            cmd = tmp;
        }
    }
    g_debug(_("Running command: %s"), cmd);
    g_spawn_command_line_async(cmd, NULL);
    g_free(cmd);
    g_signal_emit_by_name(entry, "destroy");
    return TRUE;
}

GtkWidget *gc_main_window_new(gchar *geometry)
{
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_title(GTK_WINDOW(window), APP_NAME " - GTK+ Completion-Run Utility");
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(window), 3);

    if (geometry != NULL && gtk_window_parse_geometry(GTK_WINDOW(window), geometry)) {
        g_free(geometry);
    } else {
        gint geo_left, geo_top;
        geo_left = gc_config_get_integer("Geo_Left", 100);
        geo_top = gc_config_get_integer("Geo_Top", 80);
        g_debug(_("Moving window to (%d, %d)..."), geo_left, geo_top);
        gtk_window_move(GTK_WINDOW(window), geo_left, geo_top);
    }
    gint geo_width = gc_config_get_integer("Geo_Width", 400);
    g_debug(_("Setting window width %d..."), geo_width);
    gtk_widget_set_size_request(window, geo_width, -1);
    g_signal_connect(window, "destroy", G_CALLBACK(on_destroy), NULL);

    GtkWidget *vbox = gtk_vbox_new(FALSE, 2);
    gtk_widget_show(vbox);
    gtk_container_add(GTK_CONTAINER(window), vbox);

//    GtkWidget *hbox = gtk_hbox_new(FALSE, 2);
//    gtk_widget_show(hbox);
//    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    GtkWidget *label = gtk_label_new(_("Run Program:"));
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

    GtkWidget *entry = gc_entry_new();
    g_signal_connect(entry, "activate", G_CALLBACK(on_activate), NULL);
    g_signal_connect(entry, "destroy", G_CALLBACK(on_destroy), NULL);
    gtk_widget_show(entry);
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    return window;
}
