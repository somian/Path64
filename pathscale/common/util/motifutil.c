/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

*/



static char *source_file = __FILE__;
static char *rcs_id = "$Source$ $Revision$";
/* #include "defs.h" */

#include <Xm/Xm.h>	   
#include <Xm/CascadeBG.h>
#include <Xm/Form.h>	   
#include <Xm/Label.h>	   
#include <Xm/RowColumn.h>	   
#include <Xm/ToggleBG.h>
#include "motifutil.h"

/* ====================================================================
 *
 * Quit_Callback
 *
 * Callback for quitting the program.
 *
 * ====================================================================
 */

void
Quit_Callback (
  Widget w,			/* Invoking widget */
  XtPointer client_data,	/* Client's data */
  XtPointer call_data )		/* Callback data */
{
  XtCloseDisplay ( XtDisplay(w) );
  exit (0);
}


/* ====================================================================
 *
 * Unmanage_Callback
 *
 * Callback for unmanaging a popup form.
 *
 * ====================================================================
 */

void
Unmanage_Callback (
  Widget w,			/* Invoking widget */
  XtPointer form,		/* Client's data -- form to unmanage*/
  XtPointer call_data )		/* Callback data */
{
  XtUnmanageChild ( (Widget) form );
}

/* ====================================================================
 *
 * Build_Radio_Box
 *
 * Create a radio box widget, with the given parent and Args, and with
 * children which are toggle button gadgets with labels given.
 *
 * If title is non-NULL, the radio box is put in a form with the title
 * above it.
 *
 * ====================================================================
 */

Widget
Build_Radio_Box (
  Widget parent,	/* Parent widget */
  char 	*title,		/* Title or NULL */
  char	*name,		/* Name of radio box */
  Arg	wargs[],	/* Argument array, with some extra space */
  int	nargs,		/* Current size of wargs array */
  char	*labels[],	/* Button label array */
  int	nlabels )	/* Number of buttons */
{
  Widget form = NULL;
  Widget title_lab, radio_box, radio_button;
  char namebuf[512];
  int i;

  if ( title != NULL ) {
    /* We need a form to attach a title: */
    (void) strcpy ( namebuf, name );
    (void) strcat ( namebuf, ".form" );
    form = XtCreateManagedWidget (
		namebuf, xmFormWidgetClass, parent, wargs, nargs );
    title_lab = XtCreateManagedWidget (
		title, xmLabelGadgetClass, form, NULL, 0 );
    nargs = 0;
    XtSetArg ( wargs[nargs], XmNtopAttachment, XmATTACH_WIDGET );
    nargs++;
    XtSetArg ( wargs[nargs], XmNtopWidget, title_lab );
    nargs++;
    parent = form;
  }

  XtSetArg ( wargs[nargs], XmNentryClass,
	     xmToggleButtonGadgetClass ); nargs++;
  radio_box = XmCreateRadioBox ( parent, name, wargs, nargs );
  XtManageChild ( radio_box );
  for (i=0; i < nlabels; i++) {
    radio_button = XtCreateManagedWidget (
		labels[i],
		xmToggleButtonGadgetClass,
		radio_box,
		NULL,
		0);
  }

  return form ? form : radio_box;
}

/* ====================================================================
 *
 * Build_Pulldown_Menu
 *
 * Construct a pulldown menu.  Pulldown menus are built from cascade
 * buttons, so this function creates the cascade button which owns the
 * menu, the menu, and any submenu specified.  It uses the recursive
 * MENU_ITEM data structure for describing the menu(s) to be created.
 *
 * This is based on an article in The X Journal, Jan/Feb 1992, by Dan
 * Heller.
 *
 * ====================================================================
 */

Widget
Build_Pulldown_Menu (
  Widget parent,
  char	*menu_title,
  char	menu_mnemonic,
  MENU_ITEM *items )
{
  Widget PullDown, cascade, widget;
  int i;
  XmString str;

  /* Create the menu shell: */
  PullDown = XmCreatePulldownMenu ( parent, "_pulldown", NULL, 0 );

  /* Create the cascade button: */
  str = XmStringCreateSimple ( menu_title );
  cascade = XtVaCreateManagedWidget (
		menu_title, xmCascadeButtonGadgetClass, parent,
		XmNsubMenuId,	PullDown,
		XmNlabelString,	str,
		XmNmnemonic,	menu_mnemonic,
		NULL );
  XmStringFree ( str );

  /* Now add the menu items: */
  for ( i = 0; MITEM_label(items,i) != NULL; i++ ) {

    /* If a subitem menu is specified, make a recursive call for the
     * pull-right menu, and use the cascade button returned:
     */
    if ( MITEM_subitems(items,i) != NULL ) {
      widget = Build_Pulldown_Menu ( PullDown,
		    MITEM_label(items,i),
		    MITEM_mnemonic(items,i),
		    MITEM_subitems(items,i) );
    } else {
      widget = XtVaCreateManagedWidget (
		    MITEM_label(items,i),
		    *MITEM_class(items,i),
		    PullDown, NULL );
    }

    /* In either case, this item can have a mnemonic: */
    if ( MITEM_mnemonic(items,i) != NULL ) {
      XtVaSetValues ( widget,
		      XmNmnemonic, MITEM_mnemonic(items,i),
		      NULL );
    }

    /* Any item can have an accelerator except cascade menus.  But we
     * don't worry about the exception, assuming that the menu item
     * structures were built correctly:
     */
    if ( MITEM_accelerator(items,i) != NULL ) {
      str = XmStringCreateSimple ( MITEM_accel_text(items,i) );
      XtVaSetValues ( widget,
		      XmNaccelerator, MITEM_accelerator(items,i),
		      XmNacceleratorText, str,
		      NULL );
      XmStringFree ( str );
    }

    /* Finally, anyone can have a callback: */
    if ( MITEM_callback(items,i) != NULL ) {
      XtAddCallback ( widget, XmNactivateCallback,
		      MITEM_callback(items,i),
		      MITEM_callback_data(items,i) );
    }
  }

  /* Return the cascade button: */
  return cascade;
}
