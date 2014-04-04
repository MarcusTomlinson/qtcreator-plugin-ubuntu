/*
 * Copyright 2013-2014 Canonical Ltd.
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
 *
 * Author: Christian Dywan <christian.dywan@canonical.com>
 */

#include "uctestcase.h"

#include <stdlib.h>

#include <QtQml/QQmlEngine>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtTest/QtTest>
#include <QtQuick/QQuickItem>

/*!
 * \ingroup ubuntu
 * \brief UbuntuTestCase is the C++ pendant to the QML UbuntuTestCase.
 */
UbuntuTestCase::UbuntuTestCase(const QString& file, QWindow* parent) : QQuickView(parent)
{
    QString modules("../../../modules");
    Q_ASSERT(QDir(modules).exists());
    QString modulePath(QDir(modules).absolutePath());
    engine()->addImportPath(modulePath);

    m_spy = new QSignalSpy(engine(), SIGNAL(warnings(QList<QQmlError>)));
    m_spy->setParent(this);

    Q_ASSERT(!file.isEmpty());
    setSource(QUrl::fromLocalFile(file));
    Q_ASSERT(status() == QQuickView::Ready);
    Q_ASSERT(rootObject());
    show();
    QTest::qWaitForWindowExposed(this);
}
