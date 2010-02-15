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

#include	"gc_main_window.h"
#include	"gc_config.h"

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
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

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
    gtk_widget_show(entry);
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    return window;
}
