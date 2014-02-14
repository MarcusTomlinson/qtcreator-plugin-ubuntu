/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 2.1.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: David Barth <david.barth@canonical.com>
 */

#include "ubuntuhtml5devtools.h"
#include "ubuntuconstants.h"

using namespace Ubuntu::Internal;

UbuntuHTML5DevTools::UbuntuHTML5DevTools(QObject *parent) :
    UbuntuWebMode(parent)
{
    setDisplayName(tr(Ubuntu::Constants::UBUNTU_HTML5DEVTOOLS_DISPLAYNAME));
    setId(Ubuntu::Constants::UBUNTU_HTML5DEVTOOLS);
    setObjectName(QLatin1String(Ubuntu::Constants::UBUNTU_HTML5DEVTOOLS));

    setUrl(QUrl::fromUserInput(QLatin1String(Ubuntu::Constants::HTML5_DEVTOOLS)));
}
