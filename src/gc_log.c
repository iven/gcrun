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

#include	"gc_log.h"

static void gc_debug_handler(const gchar *log_domain, GLogLevelFlags log_level,
        const gchar *message, gpointer user_data)
{
#ifdef  DEBUG
    g_print("** DEBUG: %s\n", message);
#endif     /* -----  DEBUG  ----- */
}

inline void gc_log_init(void)
{
    g_log_set_handler(NULL, G_LOG_LEVEL_DEBUG | G_LOG_FLAG_FATAL
           | G_LOG_FLAG_RECURSION, gc_debug_handler, NULL);
}
