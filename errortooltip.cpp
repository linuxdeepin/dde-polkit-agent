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

#include "errortooltip.h"

ErrorTooltip::ErrorTooltip(QString errorMessage, QWidget *parent) :
    DArrowRectangle(DArrowRectangle::ArrowTop, parent),
    m_label(new QLabel)
{
    setWindowFlags(Qt::ToolTip);
    setBackgroundColor(DBlurEffectWidget::LightColor);

    m_label->setStyleSheet("padding: 5px 10px; color: #f9704f");
    m_label->setAccessibleName("content");
    setContent(m_label);
    setMessage(errorMessage);
}

void ErrorTooltip::setMessage(QString message)
{
    m_label->setText(message);
    m_label->adjustSize();
    resizeWithContent();
}

// 返回错误信息
QString ErrorTooltip::text() const
{
    return m_label->text();
}

