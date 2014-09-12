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
 * Author: Benjamin Zeller <benjamin.zeller@canonical.com>
 */

#ifndef UBUNTU_INTERNAL_UBUNTUMANIFESTEDITORWIDGET_H
#define UBUNTU_INTERNAL_UBUNTUMANIFESTEDITORWIDGET_H

#include "ubuntuclickmanifest.h"

#include <texteditor/basetexteditor.h>
#include <texteditor/plaintexteditor.h>

#include <QScrollArea>
#include <QPointer>
#include "ubuntuabstractguieditorwidget.h"
#include "ui_ubuntumanifesteditor.h"

class QStackedWidget;

namespace Ubuntu {
namespace Internal {

class UbuntuAbstractGuiEditor;
class UbuntuManifestEditorWidget;
class UbuntuClickManifest;

class UbuntuManifestEditorWidget : public UbuntuAbstractGuiEditorWidget
{
    Q_OBJECT
public:

    explicit UbuntuManifestEditorWidget();
    ~UbuntuManifestEditorWidget();

    bool open(QString *errorString, const QString &fileName, const QString &realFileName);

    static QString createPackageName (const QString &userName, const QString &projectName);

protected:
    bool syncToWidgets ();
    bool syncToWidgets (UbuntuClickManifest *source);
    void syncToSource  ();
    QWidget *createMainWidget();
    void updateFrameworkList ();    
    void addMissingFieldsToManifest(QString fileName);    
    QString policyForFramework(const QString &fw);

protected slots:
    void bzrChanged ();
    void onFrameworkChanged ();

private:
    QWidget *createHookWidget (const UbuntuClickManifest::Hook &hook);

private:
    Ui::UbuntuManifestEditor *m_ui;
    QSharedPointer<UbuntuClickManifest> m_manifest;
};

} // namespace Internal
} // namespace Ubuntu

#endif // UBUNTU_INTERNAL_UBUNTUMANIFESTEDITORWIDGET_H