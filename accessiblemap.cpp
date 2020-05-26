/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     Shaoxiong Feng <fengshaoxiong@uniontech.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "accessiblemap.h"
AccessibleMap *AccessibleMap::INSTANCE = nullptr;
AccessibleMap::AccessibleMap(QObject *parent) : QObject(parent)
{
}

AccessibleMap *AccessibleMap::instance(QObject *parent)
{
    if (!INSTANCE)
        INSTANCE = new AccessibleMap(parent);

    return INSTANCE;
}

const QMap< QAccessible::Role, QList< QString > > AccessibleMap::accessibleMap() const
{
    return  m_accessibleMap;
}

void AccessibleMap::accessibleMapAppend(QAccessible::Role r, QString accessibleName)
{
    m_accessibleMap[r].append(accessibleName);
}

void AccessibleMap::clearAccessibleMap()
{
    m_accessibleMap.clear();
}
