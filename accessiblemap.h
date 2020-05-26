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
#ifndef ACCESSIBLEMAP_H
#define ACCESSIBLEMAP_H

#include <QObject>
#include <QAccessible>

class AccessibleMap : public QObject
{
    Q_OBJECT

public:
    static AccessibleMap *instance(QObject *parent = nullptr);
    const QMap< QAccessible::Role, QList< QString > > accessibleMap() const;
    void accessibleMapAppend(QAccessible::Role r, QString accessibleName);

public slots:
    void clearAccessibleMap();

private:
    explicit AccessibleMap(QObject *parent = nullptr);

private:
    static AccessibleMap *INSTANCE;
    QMap< QAccessible::Role, QList< QString > > m_accessibleMap;
};

#endif // ACCESSIBLEMAP_H

