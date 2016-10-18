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

#include "snapcraftprojectfile.h"

using namespace Ubuntu::Internal;

SnapcraftProjectFile::SnapcraftProjectFile(Core::Id id)
    : TextEditor::TextDocument(id)
{
}

Core::IDocument::ReloadBehavior SnapcraftProjectFile::reloadBehavior(Core::IDocument::ChangeTrigger state, Core::IDocument::ChangeType type) const
{
    Q_UNUSED(state)
    Q_UNUSED(type)
    return BehaviorSilent;
}

bool SnapcraftProjectFile::reload(QString *errorString, Core::IDocument::ReloadFlag flag, Core::IDocument::ChangeType type)
{
    if (type != TypePermissions) {
        emit changed();
    }

    return TextDocument::reload(errorString, flag, type);
}
