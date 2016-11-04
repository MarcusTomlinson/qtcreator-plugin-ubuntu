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

#include "ubuntuversion.h"

#include <QFile>
#include <QStringList>

namespace Ubuntu {
namespace Internal {

Q_GLOBAL_STATIC(UbuntuVersion, g_instance);

const char  DISTRIB_ID[] = "DISTRIB_ID=";
const char  DISTRIB_CODENAME[] = "DISTRIB_CODENAME=";
const char  DISTRIB_RELEASE[] = "DISTRIB_RELEASE=";
const char  DISTRIB_DESCRIPTION[] = "DISTRIB_DESCRIPTION=";
const char  LSB_RELEASE[] = "/etc/lsb-release";

UbuntuVersion::UbuntuVersion()
{
    initFromLsbFile();
}

bool UbuntuVersion::supportsSnappy() const
{
    if (m_valid)
        return codename() == Constants::XENIAL;
    return false;
}

UbuntuVersion *UbuntuVersion::instance()
{
    return g_instance();
}

void UbuntuVersion::initFromLsbFile()
{
    QFile lsbRelease(QString::fromLatin1(LSB_RELEASE));
    if (lsbRelease.open(QIODevice::ReadOnly)) {
        QByteArray data = lsbRelease.readAll();
        lsbRelease.close();

        foreach(QString line, QString::fromLatin1(data).split(QLatin1String("\n"))) {
            if (line.startsWith(QLatin1String(DISTRIB_ID))) {
                m_id = line.replace(QLatin1String(DISTRIB_ID),QLatin1String(""));

            } else if (line.startsWith(QLatin1String(DISTRIB_RELEASE))) {
                m_release = line.replace(QLatin1String(DISTRIB_RELEASE),QLatin1String(""));

            } else if (line.startsWith(QLatin1String(DISTRIB_CODENAME))) {
                m_codename = line.replace(QLatin1String(DISTRIB_CODENAME),QLatin1String(""));

            } else if (line.startsWith(QLatin1String(DISTRIB_DESCRIPTION))) {
                m_description = line.replace(QLatin1String(DISTRIB_DESCRIPTION),QLatin1String(""));
            }
        }

        m_valid = true;
    }
}

}}

