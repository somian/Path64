#!/usr/bin/python
#
# Copyright 2006. QLogic Corporation.
#
# Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
# Unpublished -- rights reserved under the copyright laws of the United
# States. USE OF A COPYRIGHT NOTICE DOES NOT IMPLY PUBLICATION OR
# DISCLOSURE. THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
# SECRETS OF PATHSCALE, INC. USE, DISCLOSURE, OR REPRODUCTION IS
# PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF PATHSCALE,
# INC.
#
# U.S. Government Restricted Rights:
# The Software is a "commercial item," as that term is defined at 48
# C.F.R. 2.101 (OCT 1995), consisting of "commercial computer software"
# and "commercial computer software documentation," as such terms are used
# in 48 C.F.R. 12.212 (SEPT 1995).  Consistent with 48 C.F.R. 12.212 and
# 48 C.F.R. 227-7202-1 through 227-7202-4 (JUNE 1995), all U.S. Government
# End Users acquire the Software with only those rights set forth in the
# accompanying license agreement. PathScale, Inc. 2071 Stierlin Ct.,
# Suite 200, Mountain View, CA 94043.

# This is meant to be a sort of an MVC (model, view, controller)
# approach to a user interface. However, it's curses, and you can't
# really separate the view and the controller, so all that stuff is in
# the "_view" functions.

try:
   import curses
except (ImportError),e:
   print e
   print  'It is contained in the python-curses rpm' 
   print  'If you are on a SuSE-type distribution, root can issue\n\t "yast2 --install python-curses"\n to install.'
   exit(1)

import os, sys

# UI "Widgets"

# display_centered
#
# Display <text> centered on line <line>, with optional attributes <attr>.
# I'm not smart enough to wrap if the line doesn't fit.

def display_centered (scr, line, text, attr = curses.A_NORMAL):
    """Display text centered on a line."""

    maxy, maxx = scr.getmaxyx()
    indent = (maxx - len(text)) / 2
    if indent < 0:
        indent = 0
    scr.addstr (line, indent, text, attr)

# radio_box
#
# Given a screen <scr>, <y>, <x>, an array of strings <choice_list>,
# an optional default selection, and a list of keys that will quit the
# widget without making a selection, display a menu of the selections
# and return the index of the choice, or -1 if the user has quit out
# of the widget. Allow only a single selection.

def radio_box (scr, y, x, choice_list, default_ind = 0, quit_keys = None):
    """Create a selection menu for the choices in choice_list."""

    temp_y = y
    
    for choice in choice_list:
        scr.addstr (temp_y, x, '[ ] ' + choice)
        temp_y += 1

    min_y = y
    max_y = temp_y-1
    scr.addstr (y + default_ind, x+1, "X")
    scr.move (y + default_ind, x+1)

    selection_made = False
    while not selection_made:
        cur_y, cur_x = scr.getyx()
        c = scr.getch()
        if c == curses.KEY_UP:
            if cur_y == min_y:
                curses.beep()
            else:
                scr.addstr (cur_y, cur_x, " ")
                scr.addstr (cur_y-1, cur_x, "X")
                scr.move (cur_y-1, cur_x)
        elif c == curses.KEY_DOWN:
            if cur_y == max_y:
                curses.beep()
            else:
                scr.addstr (cur_y, cur_x, " ")
                scr.addstr (cur_y+1, cur_x, "X")
                scr.move (cur_y+1, cur_x)
        elif c == curses.KEY_ENTER or c == ord('\n'):
            selection_made = True
        elif quit_keys:
            if c in [ord(s) for s in quit_keys]:
                return -1

    cur_y, cur_x = scr.getyx()
    
    return cur_y-min_y

# selection_box
#
# Given a screen <scr>, <y>, <x>, and an array of strings
# <choice_list>, display a menu of the selections and return an array
# of the choices. Allow zero or more selections.

def selection_box (scr, y, x, choice_list):
    """Create a selection menu for the choices in choice_list."""

    temp_y = y
    choices = [False] * len(choice_list)
    
    for choice in choice_list:
        scr.addstr (temp_y, x, '[ ] ' + choice)
        temp_y += 1

    min_y = y
    max_y = temp_y-1
    scr.move (y, x+1)

    selection_made = False
    while not selection_made:
        cur_y, cur_x = scr.getyx()
        c = scr.getch()
        if c == curses.KEY_UP:
            if cur_y == min_y:
                curses.beep()
            else:
                scr.move (cur_y-1, cur_x)
        elif c == curses.KEY_DOWN:
            if cur_y == max_y:
                curses.beep()
            else:
                scr.move (cur_y+1, cur_x)
        elif c == ord(' '):
            index = cur_y - min_y
            if choices[index] == True:
                scr.addstr (cur_y, cur_x, " ")
                scr.move (cur_y, cur_x)
                choices[index] = False
            else:
                scr.addstr (cur_y, cur_x, "X")
                scr.move (cur_y, cur_x)
                choices[index] = True
        elif c == curses.KEY_ENTER or c == ord('\n'):
            selection_made = True
   
    return choices

# fill_bar
#
# Draw a progress bar at on screen <scr> at location <y>, <x>, of
# width <width>, and fill percentage <fill>. The bar takes three
# lines of the screen.

def fill_bar (scr, y, x, width, fill):
    """Draw a progress bar."""

    start_y, start_x = scr.getyx()

    num_blocks = int((width - 2) * fill)
    
    scr.addch (y, x, curses.ACS_ULCORNER)
    scr.hline (y, x + 1, curses.ACS_HLINE, width - 2)
    scr.addch (y, x + width - 1, curses.ACS_URCORNER)
    scr.addch (y + 1, x, curses.ACS_VLINE)
    scr.hline (y + 1, x + 1, curses.ACS_BLOCK, num_blocks)
    scr.addch (y + 1, x + width - 1, curses.ACS_VLINE)
    scr.addch (y + 2, x, curses.ACS_LLCORNER)
    scr.hline (y + 2, x + 1, curses.ACS_HLINE, width - 2)
    scr.addch (y + 2, x + width - 1, curses.ACS_LRCORNER)

    scr.move (start_y, start_x)

# text_box
#
# This creates a word-wrapped text box at <y, x>. Text that does not
# fit in the box will be truncated. Return the number of lines drawn.
#
# Oh, and you can do [b][/b] or [u][/u] to bold or underline text.
# Don't mix them. Don't make them their own words (attach them to
# words).

def text_box (scr, y, x, height, width, txt):
    """Create a word-wrapped text box."""

    cur_y = y
    cur_x = x
    text_mode = curses.A_NORMAL
    revert_text = False
    
    split_txt = txt.split()

    while cur_y < y + height and len(split_txt) > 0:
        while len(split_txt) > 0 and cur_x + len(split_txt[0]) < x + width:
            if split_txt[0].startswith('[b]') or split_txt[0].startswith('[B]'):
                text_mode = curses.A_BOLD
                split_txt[0] = split_txt[0][3:]
            elif split_txt[0].startswith('[u]') or split_txt[0].startswith('[U]'):
                text_mode = curses.A_UNDERLINE
                split_txt[0] = split_txt[0][3:]
            if (split_txt[0].endswith('[/b]') or split_txt[0].endswith('[/B]')) and text_mode == curses.A_BOLD:
                revert_text = True
                split_txt[0] = split_txt[0][:-4]
            elif (split_txt[0].endswith('[/u]') or split_txt[0].endswith('[/U]')) and text_mode == curses.A_UNDERLINE:
                revert_text = True
                split_txt[0] = split_txt[0][:-4]

            scr.addstr(cur_y, cur_x, split_txt[0] + ' ', text_mode)
            cur_x = cur_x + len(split_txt[0]) + 1
            split_txt.pop(0)
            if revert_text == True:
                text_mode = curses.A_NORMAL
                revert_text = False
        cur_y += 1
        cur_x = x

    return cur_y - y

# text_entry_field
#
# Create a text entry field at <y, x>. Return the text entered after
# the user has pressed the enter key.

def text_entry_field (scr, y, x, width):
    """Create a text entry field."""

    curses.echo()

    scr.addstr (y, x, ' ' * width, curses.A_UNDERLINE)

    ret = scr.getstr (y, x)

    curses.noecho()

    return ret

