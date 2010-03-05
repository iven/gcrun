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

#include	<gdk/gdkkeysyms.h>
#include	<glib/gstdio.h>
#include	"gc_entry.h"

static gboolean on_key_press(GtkWidget *entry, GdkEventKey *event, gpointer data)
{
    switch (event->keyval) {
        case GDK_Escape:
            g_signal_emit_by_name(entry, "destroy");
            break;
        case GDK_Return:
            if (event->state & GDK_CONTROL_MASK) {
                gc_entry_set_in_term(entry, TRUE);
                g_signal_emit_by_name(entry, "activate");
            }
            break;
        default:
            break;
    }

    return FALSE;
}

static gboolean on_changed(GtkWidget *entry, gpointer data)
{

    return FALSE;
}

GtkListStore *gc_entry_default_model(void)
{
    GtkListStore *model;
    GtkTreeIter iter;
    GDir *dir;
    GError *err = NULL;
    const gchar *env_path, *file_name;
    gchar **paths, **p_path, *full_name;
    struct stat stat_buf;

    model = gtk_list_store_new(1, G_TYPE_STRING);
    env_path = g_getenv("PATH");
    paths = g_strsplit(env_path, ":", -1);
    for (p_path = paths; *p_path != NULL; p_path++) {
        dir = g_dir_open(*p_path, 0, &err);
        if (err != NULL) {
            g_error_free(err);
            continue;
        }
        while ((file_name = g_dir_read_name(dir)) != NULL) {
            full_name = g_build_filename(*p_path, file_name, NULL);
            if (g_stat(full_name, &stat_buf) == -1 || S_ISDIR(stat_buf.st_mode)) {
                continue;
            }
            gtk_list_store_append(model, &iter);
            gtk_list_store_set(model, &iter, 0, file_name, -1);
        }
        g_dir_close(dir);
    }
    g_strfreev(paths);
    return model;
}

void gc_entry_set_in_term(GtkWidget *entry, gboolean value)
{
    g_object_set_data(G_OBJECT(entry), "in_term", GINT_TO_POINTER(value));
    return;
}

gboolean gc_entry_get_in_term(GtkWidget *entry)
{
    return GPOINTER_TO_INT(g_object_get_data(G_OBJECT(entry), "in_term"));
}

GtkWidget *gc_entry_new(void)
{
    GtkWidget *entry = gtk_entry_new();
    gc_entry_set_in_term(entry, FALSE);
    g_signal_connect(entry, "key_press_event", G_CALLBACK(on_key_press), NULL);
    g_signal_connect(entry, "changed", G_CALLBACK(on_changed), NULL);

    GtkEntryCompletion *completion = gtk_entry_completion_new();
    gtk_entry_completion_set_inline_completion(completion, TRUE);
    gtk_entry_completion_set_inline_selection(completion, TRUE);
    gtk_entry_completion_set_popup_single_match(completion, FALSE);
    gtk_entry_set_completion(GTK_ENTRY(entry), completion);

    GtkListStore *model = gc_entry_default_model();
    gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(model));
    gtk_entry_completion_set_text_column(completion, 0);

    return entry;
}

