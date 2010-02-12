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

#include	"main.h"
#include	"gc_config.h"
#include	"gc_log.h"
#include	"gc_main_window.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  main function
 * =====================================================================================
 */
int main (int argc, char *argv[])
{
    GError *err = NULL;
    gchar *geometry = NULL;
    const GOptionEntry entries[] =
    {
        {"geometry", 'g', 0, G_OPTION_ARG_STRING, &geometry, _("Create the initial window with the given geometry."), "GEOMETRY" },
        {NULL}
    };

    bindtextdomain(GC_GETTEXT_PACKAGE, GC_LOCALE_DIR);
    bind_textdomain_codeset(GC_GETTEXT_PACKAGE, "UTF-8");
    textdomain(GC_GETTEXT_PACKAGE);

    gtk_init(&argc, &argv);
    gc_log_init();
    gc_config_init();

    GOptionContext *context = g_option_context_new(_("- GTK+ Completion-Run Utility"));
    g_option_context_add_main_entries(context, entries, GC_GETTEXT_PACKAGE);
    g_option_context_add_group(context, gtk_get_option_group(TRUE));
    if (!g_option_context_parse(context, &argc, &argv, &err)) {
        g_warning(_("Option parsing failed: %s\n"), err->message);
        g_error_free(err);
    }
    g_option_context_free(context);

    GtkWidget *window = gc_main_window_new(geometry);
    gtk_widget_show(window);

    gtk_main();

	return 0;
}		/* ----------  end of function main  ---------- */

