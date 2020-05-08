/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     Hualet Wang <mr.asianwang@gmail.com
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

//
// Created by hualet on 11/22/16.
//

#ifndef DDE_POLKIT_AGENT_ERRORTOOLTIP_H
#define DDE_POLKIT_AGENT_ERRORTOOLTIP_H

#include <darrowrectangle.h>

DWIDGET_USE_NAMESPACE

class ErrorTooltip : public DArrowRectangle
{
    Q_OBJECT
public:
    ErrorTooltip(QString errorMessage, QWidget *parent = nullptr);
    ~ErrorTooltip() {delete m_label;}

    void setMessage(QString text);

    QString text() const;

private:
    QLabel *m_label;
};


#endif //DDE_POLKIT_AGENT_ERRORTOOLTIP_H
