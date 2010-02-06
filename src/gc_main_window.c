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

GtkWidget *gc_main_window_new(void)
{
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_title(GTK_WINDOW(window), APP_NAME " - GTK+ Completion-Run Utility");
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    g_signal_connect(window, "destroy", gtk_main_quit, NULL);

    GtkWidget *vbox = gtk_vbox_new(FALSE, 2);
    gtk_widget_show(vbox);
    gtk_container_add(GTK_CONTAINER(window), vbox);

//    GtkWidget *hbox = gtk_hbox_new(FALSE, 2);
//    gtk_widget_show(hbox);
//    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    GtkWidget *label = gtk_label_new(_("Run Program:"));
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

    return window;
}
