/*
 * simple_xkb_tray_icon.cpp
 * 
 * Copyright 2023 Eugeniusz <Eugene at sky.community>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#define APP_NAME "SIMPLE XKB TRAY ICON"
#define APP_CODE "simple_xkb_tray_icon"
#define ICONS_CATALOGUE "/opt/simple_xkb_tray_icon/icons"
#define DEFAULT_ICON "./icon.png"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <stdbool.h>

#include <X11/XKBlib.h>
#include <X11/extensions/XKBrules.h>

#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>

#include <string>

int is_silent = 0;
int do_stop = 0;

static void Quit_menu()
{
    if(is_silent == 0) printf( "[ INFO ][ %s ] Exiting ...\n", APP_NAME );
    gtk_main_quit();
    do_stop = 1;
}

std::string getNthWord(std::string haystack, int word_num) {
    std::string res = "";
    std::string word = "";
    int i=0;
    int found = 0;
    for (auto x : haystack) {
        if (x == ',') {
            if(i == word_num) {
                res = word;
                found = 1;
                break;
            }
            word = "";
            i++;
        } else {
            word = word + x;
        }
    }
    // for last word
    if((found == 0) && (word_num == i)) {res = word;found=1;}
    return res;
}


int main(int argc, char *argv[]) {
    
    if (argc >= 2) {
        for (int i = 0; i < argc; i++) {
            if(strcmp(argv[i],"--help")==0)  {
                printf( "[ INFO ][ %s ] Usage parameters:\n\n\t--help\t\tprint this message\n\t--silent\tdon't output anything but errors\n\n", APP_NAME );
                exit(0);
            } else if(strcmp(argv[i],"--silent")==0) {
                is_silent=1;
            }
        }
    }
    if(is_silent == 0) printf("[ INFO ][ %s ] Starting main loop.\n", APP_NAME);
    
    // UI part
    
    GtkWidget *indicator_menu;
    GtkWidget *menuItem2;

    AppIndicator *indicator;
    gtk_init( &argc, &argv );
    
    indicator = app_indicator_new( APP_CODE, DEFAULT_ICON, APP_INDICATOR_CATEGORY_APPLICATION_STATUS );

    app_indicator_set_icon( indicator, DEFAULT_ICON );

    indicator_menu = gtk_menu_new();

    menuItem2 = gtk_menu_item_new_with_label( "Quit" );
    gtk_menu_shell_insert( GTK_MENU_SHELL( indicator_menu ), menuItem2, 1 );
    gtk_widget_show_all( menuItem2 );
    
    g_signal_connect( menuItem2, "activate", G_CALLBACK( Quit_menu ), NULL );

    app_indicator_set_menu( indicator, GTK_MENU( indicator_menu ) );
    app_indicator_set_status( indicator, APP_INDICATOR_STATUS_ACTIVE );
    gtk_main_iteration_do(0);
    
    // EOF UI part
    
    int just_started = 1;
    std::string icon_fname = "";
    std::string lang_code = "";
    std::string old_lang_code = "";
    
    
    Display *dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        fprintf(stderr, "[ ERROR ] [ %s ] Cannot open display.\n", APP_NAME);
        exit(1);
    }
    XkbStateRec state;
    XkbGetState(dpy, XkbUseCoreKbd, &state);
    XkbDescPtr desc = XkbGetKeyboard(dpy, XkbAllComponentsMask, XkbUseCoreKbd);
    char *group = XGetAtomName(dpy, desc->names->groups[state.group]);
    XkbRF_VarDefsRec vd;
    XkbRF_GetNamesProp(dpy, NULL, &vd);
    
    while (1) {
        XkbRF_GetNamesProp(dpy, NULL, &vd);
        XkbGetState(dpy, XkbUseCoreKbd, &state);
        desc = XkbGetKeyboard(dpy, XkbAllComponentsMask, XkbUseCoreKbd);
        group = XGetAtomName(dpy, desc->names->groups[state.group]);
        
        // state.group is the index of the layout code if we'll split vd.layout to words by comma delimiter
        std::string vdlt(vd.layout);
        lang_code = getNthWord(vdlt,state.group);
        
        if(lang_code == "") lang_code = "icon";
        if((old_lang_code.compare(lang_code) != 0) || (just_started == 1)) {
            if(is_silent == 0) printf("Received layout name: %s and language code: %s \n", group, lang_code.c_str());
            old_lang_code = lang_code;
            icon_fname = std::string("") + ICONS_CATALOGUE + "/" + lang_code + ".png";
            app_indicator_set_icon( indicator, icon_fname.c_str() );
            just_started = 0;
        }
        gtk_main_iteration_do(0);
        if(do_stop) break;
  }
  XFree(group);
  XCloseDisplay(dpy);
  return 0;
}









