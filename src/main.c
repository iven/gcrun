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

#ifndef  MTRACE
#include	<mcheck.h>
#endif     /* -----  not MTRACE  ----- */

#include	"main.h"
#include	"gc_config.h"
#include	"gc_main_window.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  main function
 * =====================================================================================
 */
int main (int argc, char *argv[])
{
#ifdef MTRACE
    mtrace();
#endif

    bindtextdomain(GC_GETTEXT_PACKAGE, GC_LOCALE_DIR);
    bind_textdomain_codeset(GC_GETTEXT_PACKAGE, "UTF-8");
    textdomain(GC_GETTEXT_PACKAGE);

    gtk_init(&argc, &argv);
    gc_config_init();

    GtkWidget *window = gc_main_window_new();
    gtk_widget_show(window);

    gtk_main();

	return 0;
}		/* ----------  end of function main  ---------- */

