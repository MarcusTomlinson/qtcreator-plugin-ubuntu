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

#ifndef UBUNTUHTML5DEVTOOLS_H
#define UBUNTUHTML5DEVTOOLSE_H

#include "ubuntuwebmode.h"
#include <QObject>

namespace Ubuntu {
namespace Internal {

class UbuntuHTML5DevTools : public UbuntuWebMode
{
    Q_OBJECT

public:
    explicit UbuntuHTML5DevTools(QObject *parent = 0);

};

}
}

#endif // UBUNTUHTML5DEVTOOLS_H
