//
// Created by hualet on 11/22/16.
//

#include "errortooltip.h"

ErrorTooltip::ErrorTooltip(QString errorMessage) :
    DArrowRectangle(DArrowRectangle::ArrowTop),
    m_label(new QLabel(errorMessage))
{
    setWindowFlags(Qt::ToolTip);

    m_label->setStyleSheet("padding: 5px 10px; color: #f9704f");
    m_label->adjustSize();
    setContent(m_label);
}

void ErrorTooltip::setMessage(QString message) {
    m_label->setText(message);
}
