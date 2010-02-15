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
#include	"gc_entry.h"

static gboolean on_key_press(GtkWidget *entry, GdkEventKey *event, gpointer data)
{
    switch (event->keyval) {
        case GDK_Escape:
            gtk_main_quit();
            break;
        default:
            break;
    }
    return FALSE;
}
GtkWidget *gc_entry_new(void)
{
    GtkWidget *entry = gtk_entry_new();

    g_signal_connect(G_OBJECT(entry), "key_press_event", G_CALLBACK(on_key_press), NULL);

    return entry;
}

