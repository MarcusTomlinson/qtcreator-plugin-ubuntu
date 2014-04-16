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
 * Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
 */

#include "ubuntudevicemode.h"
#include "ubuntuconstants.h"
#include "ubuntudevicesmodel.h"

#include <coreplugin/modemanager.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/dialogs/iwizard.h>
#include <coreplugin/coreconstants.h>
#include <projectexplorer/projectexplorer.h>
#include <coreplugin/icore.h>
#include <coreplugin/imode.h>
#include <utils/styledbar.h>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QProcessEnvironment>

using namespace Ubuntu::Internal;

UbuntuDeviceMode::UbuntuDeviceMode(QObject *parent) :
    Core::IMode(parent)
{
    setDisplayName(tr(Ubuntu::Constants::UBUNTU_MODE_DEVICES_DISPLAYNAME));
    setIcon(QIcon(QLatin1String(Ubuntu::Constants::UBUNTU_MODE_DEVICES_ICON)));
    setPriority(Ubuntu::Constants::UBUNTU_MODE_DEVICES_PRIORITY);
    setId(Ubuntu::Constants::UBUNTU_MODE_DEVICES);
    setObjectName(QLatin1String(Ubuntu::Constants::UBUNTU_MODE_DEVICES));


    m_modeWidget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    m_modeWidget->setLayout(layout);

    Utils::StyledBar* styledBar = new Utils::StyledBar(m_modeWidget);
    layout->addWidget(styledBar);
    QScrollArea *scrollArea = new QScrollArea(m_modeWidget);
    scrollArea->setFrameShape(QFrame::NoFrame);
    layout->addWidget(scrollArea);

#if 0
    scrollArea->setWidget(&m_ubuntuDevicesWidget);
#else
    m_modeView = new QQuickView;
    m_modeView->setResizeMode(QQuickView::SizeRootObjectToView);
    m_devicesModel = new UbuntuDevicesModel(m_modeView);
    QWidget* container = QWidget::createWindowContainer(m_modeView);
    container->setMinimumWidth(860);
    container->setMinimumHeight(548);
    container->setFocusPolicy(Qt::TabFocus);

    scrollArea->setWidget(container);



#endif
    scrollArea->setWidgetResizable(true);

    connect(Core::ModeManager::instance(), SIGNAL(currentModeChanged(Core::IMode*)), SLOT(modeChanged(Core::IMode*)));

    m_modeView->rootContext()->setContextProperty(QLatin1String("devicesModel"),m_devicesModel);
    m_modeView->setSource(QUrl::fromLocalFile(Constants::UBUNTU_DEVICESCREEN_QML));

    setWidget(m_modeWidget);
}

void UbuntuDeviceMode::modeChanged(Core::IMode *mode) {

}

void UbuntuDeviceMode::initialize() {
    qDebug()<<"Import path: "<<m_modeView->engine()->importPathList();
    qDebug()<<"Plugin path: "<<m_modeView->engine()->pluginPathList();
    qDebug()<<QProcessEnvironment::systemEnvironment().toStringList();
}
