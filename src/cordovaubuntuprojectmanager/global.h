/*
 * Copyright 2013 Canonical Ltd.
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
 */

#ifndef CORDOVAUBUNTUPROJECTMANAGER_GLOBAL_H
#define CORDOVAUBUNTUPROJECTMANAGER_GLOBAL_H

#include <QtGlobal>

#if defined(CORDOVAUBUNTUPROJECTMANAGER_LIBRARY)
#  define CORDOVAUBUNTUPROJECTMANAGERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CORDOVAUBUNTUPROJECTMANAGERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CORDOVAUBUNTUPROJECTMANAGER_GLOBAL_H

