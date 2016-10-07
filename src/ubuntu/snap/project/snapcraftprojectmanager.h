/*
 * Copyright 2016 Canonical Ltd.
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
 * Author: Benjamin Zeller <benjamin.zeller@canonical.com>
 */

#ifndef UBUNTU_INTERNAL_SNAPCRAFTPROJECTMANAGER_H
#define UBUNTU_INTERNAL_SNAPCRAFTPROJECTMANAGER_H

#include <projectexplorer/iprojectmanager.h>

namespace Ubuntu {
namespace Internal {

class SnapcraftProjectManager : public ProjectExplorer::IProjectManager
{
public:
    SnapcraftProjectManager();

    // IProjectManager interface
    virtual QString mimeType() const override;
    virtual ProjectExplorer::Project *openProject(const QString &fileName, QString *errorString) override;
};

} // namespace Internal
} // namespace Ubuntu

#endif // UBUNTU_INTERNAL_SNAPCRAFTPROJECTMANAGER_H
