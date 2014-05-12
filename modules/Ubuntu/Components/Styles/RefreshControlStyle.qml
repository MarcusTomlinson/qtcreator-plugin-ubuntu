/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.2

/*!
    \qmltype RefreshControlStyle
    \inqmlmodule Ubuntu.Components.Styles 1.1
    \ingroup style-api
    \brief Style API for RefreshControl component.

    The component defines the style API for the RefreshControl component.
  */

Item {
    /*!
      The property holds the threshold value when the component should enter
      into \a ready-to-refresh state when dragged manually. By default this
      value is the sum of the Flickable's \a topMargin and \a height of the
      component.
      */
    property real activationThreshold: 0.0
    /*!
      States:
      \list
      \li \b ready-to-refresh - the component is ready to initiate refresh
      \li \b refreshing - the component is in refreshing state
      \endlist
      */
}
