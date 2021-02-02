/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
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
#include "yearscheduleview.h"
#include "schedulectrldlg.h"
#include "myscheduleview.h"
#include "scheduledlg.h"
#include "scheduledatamanage.h"
#include "constants.h"

#include <DMessageBox>
#include <DPushButton>
#include <DHiDPIHelper>
#include <DPalette>

#include <QAction>
#include <QMenu>
#include <QListWidget>
#include <QLabel>
#include <QPainter>
#include <QHBoxLayout>
#include <QStylePainter>
#include <QRect>
#include <QMouseEvent>

DGUI_USE_NAMESPACE

const QString fontfamily = QStringLiteral("SourceHanSansSC-Medium");

CYearScheduleView::CYearScheduleView(QWidget *parent)
    : DWidget(parent)
{
    setContentsMargins(10, 10, 10, 10);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
}

CYearScheduleView::~CYearScheduleView()
{
}

bool YScheduleDateThan(const ScheduleDataInfo &s1, const ScheduleDataInfo &s2)
{
    QDate bdate1 = s1.getBeginDateTime().date();
    QDate edate1 = s1.getEndDateTime().date();
    QDate bdate2 = s2.getBeginDateTime().date();
    QDate edate2 = s2.getEndDateTime().date();

    if (bdate1 != edate1 && bdate2 == edate2) {
        return true;
    } else if (bdate1 == edate1 && bdate2 != edate2) {
        return false;
    } else if (bdate1 != edate1 && bdate2 != edate2) {
        return bdate1 < bdate2;
    } else {
        if (s1.getBeginDateTime() == s2.getBeginDateTime()) {
            return s1.getTitleName() < s2.getTitleName();
        } else {
            return s1.getBeginDateTime() < s2.getBeginDateTime();
        }
    }
}
bool YScheduleDaysThan(const ScheduleDataInfo &s1, const ScheduleDataInfo &s2)
{
    return s1.getBeginDateTime().date().daysTo(s1.getEndDateTime().date()) > s2.getBeginDateTime().date().daysTo(s2.getEndDateTime().date());
}

void CYearScheduleView::setData(QVector<ScheduleDataInfo> &vListData)
{
    QVector<ScheduleDataInfo> valldayListData, vDaylistdata;

    for (int i = 0; i < vListData.count(); i++) {
        if (vListData.at(i).getAllDay()) {
            valldayListData.append(vListData.at(i));
        } else {
            vDaylistdata.append(vListData.at(i));
        }
    }

    std::sort(valldayListData.begin(), valldayListData.end(), YScheduleDaysThan);
    std::sort(valldayListData.begin(), valldayListData.end(), YScheduleDateThan);
    std::sort(vDaylistdata.begin(), vDaylistdata.end(), YScheduleDaysThan);
    std::sort(vDaylistdata.begin(), vDaylistdata.end(), YScheduleDateThan);

    for (int i = 0; i < valldayListData.count(); i++) {
        QVector<ScheduleDataInfo>::iterator iter = valldayListData.begin();
        if (valldayListData.at(i).getType() == DDECalendar::FestivalTypeID) {
            ScheduleDataInfo moveDate;
            moveDate = valldayListData.at(i);
            valldayListData.removeAt(i);
            valldayListData.insert(iter, moveDate);
        }
    }

    m_vlistData.clear();
    m_vlistData.append(valldayListData);
    m_vlistData.append(vDaylistdata);

    if (m_vlistData.size() > DDEYearCalendar::YearScheduleListMaxcount) {
        QVector<ScheduleDataInfo> vTlistData;
        for (int i = 0; i < 4; i++) {
            if (m_vlistData.at(i).getBeginDateTime().date() != m_vlistData.at(i).getEndDateTime().date() && !m_vlistData.at(i).getAllDay()) {
                if (m_vlistData.at(i).getBeginDateTime().date() != m_currentDate) {
                    m_vlistData[i].setAllDay(true);
                }
            }
            vTlistData.append(m_vlistData.at(i));
        }
        ScheduleDataInfo info;
        info.setTitleName("......");
        info.setID(-1);
        vTlistData.append(info);
        m_vlistData = vTlistData;
    }
}

void CYearScheduleView::clearData()
{
    m_vlistData.clear();
    return;
}

int CYearScheduleView::showWindow()
{
    if (m_vlistData.isEmpty()) {
        setFixedSize(130, 45);
    } else {
        setFixedSize(240, 180);
    }
    updateDateShow();

    return this->width();
}

void CYearScheduleView::setTheMe(int type)
{
    if (type == 0 || type == 1) {
        m_btimecolor = "#414D68";
        m_btimecolor.setAlphaF(0.7);
        m_bttextcolor = "#414D68";
        m_lBackgroundcolor = "#EBEBEB";
        m_lBackgroundcolor.setAlphaF(0.0);
        m_solocolor = "#FF7272";
    } else if (type == 2) {
        m_btimecolor = "#C0C6D4";
        m_btimecolor.setAlphaF(0.7);
        m_bttextcolor = "#C0C6D4";
        m_lBackgroundcolor = "#191919";
        m_lBackgroundcolor.setAlphaF(0.00);
        m_solocolor = "#FF7272";
        m_solocolor.setAlphaF(0.8);
    }
}

void CYearScheduleView::setCurrentDate(QDate cdate)
{
    m_currentDate = cdate;
}

QDate CYearScheduleView::getCurrentDate()
{
    return m_currentDate;
}

void CYearScheduleView::adjustPosition(bool ad)
{
    adjustPos = ad;
}

void CYearScheduleView::updateDateShow()
{
    int sviewNum = 0;

    if (!m_vlistData.isEmpty()) {
        if (m_vlistData.size() > DDEYearCalendar::YearScheduleListMaxcount) {
            sviewNum = DDEYearCalendar::YearScheduleListMaxcount;
        } else {
            sviewNum = m_vlistData.size();
        }
    }

    if (!m_vlistData.isEmpty())
        setFixedSize(240, 45 + (sviewNum - 1) * 29);
    update();

    return;
}

void CYearScheduleView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    for (int i = 0; i < m_vlistData.size(); ++i) {
        paintItem(m_vlistData.at(i), i, 0);
    }
    if (m_vlistData.isEmpty()) {
        paintItem();
    }
    adjustPos = false;
}

void CYearScheduleView::paintItem(ScheduleDataInfo info, int index, int type)
{
    int labelwidth = width() - 30;
    int bheight = index * 29 + 10;
    int labelheight = 28;
    ScheduleDataInfo &gd = info;
    CSchedulesColor gdcolor = CScheduleDataManage::getScheduleDataManage()->getScheduleColorByType(gd.getType());
    QFont font;

    font.setWeight(QFont::Medium);
    font.setPixelSize(DDECalendar::FontSizeTwelve);
    QColor scolor = gdcolor.Purecolor;
    scolor.setAlphaF(1.0);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 反锯齿;


    if (gd.getID() == -1) {
        QString str = "...";

        painter.save();
        painter.setPen(m_btimecolor);
        painter.setFont(font);
        if (adjustPos) {
            painter.drawText(QRect(25 - 18, bheight, labelwidth - 80, labelheight - 2), Qt::AlignLeft | Qt::AlignVCenter, str);
        } else {
            painter.drawText(QRect(25, bheight, labelwidth - 80, labelheight - 2), Qt::AlignLeft | Qt::AlignVCenter, str);
        }
        painter.restore();
    } else {
        if (info.getID() != -1) {
            //圆点
            painter.save();
            if (type == 0)
                painter.setBrush(QBrush(gdcolor.splitColor));
            else {
                painter.setBrush(QBrush(m_solocolor));
            }
            painter.setPen(Qt::NoPen);

            if (adjustPos) {
                painter.drawEllipse(QRect(25 - 18, bheight + (labelheight - 8) / 2, 8, 8));
            } else {
                painter.drawEllipse(QRect(25, bheight + (labelheight - 8) / 2, 8, 8));
            }
            painter.restore();
        }

        QString str;
        //左边文字
        painter.save();
        painter.setPen(m_bttextcolor);
        painter.setFont(font);
        QFontMetrics fm = painter.fontMetrics();
        QString tStitlename = gd.getTitleName();
        tStitlename.replace("\n", "");
        str = tStitlename;
        int tilenameW = labelwidth - 80;
        QString tstr;

        for (int i = 0; i < str.count(); i++) {
            tstr.append(str.at(i));
            int widthT = fm.width(tstr) + 5;
            if (widthT >= tilenameW) {
                tstr.chop(1);
                break;
            }
        }
        if (tstr != str) {
            tstr = tstr + "...";
        }
        if (adjustPos) {
            painter.drawText(QRect(41 - 18, bheight, tilenameW, labelheight - 2), Qt::AlignLeft | Qt::AlignVCenter, tstr);
        } else {
            painter.drawText(QRect(41, bheight, tilenameW, labelheight - 2), Qt::AlignLeft | Qt::AlignVCenter, tstr);
        }
        painter.restore();

        if (info.getID() != -1) {
            //右边时间
            painter.save();
            painter.setPen(m_btimecolor);
            painter.setFont(font);
            QLocale locale;

            if (info.getAllDay()) {
                str = tr("All Day");
            } else {
                if (m_currentDate > info.getBeginDateTime().date()) {
                    str = tr("All Day");
                } else {
                    str = info.getBeginDateTime().time().toString("hh:mm");
                }
            }

            QFontMetrics fm2 = painter.fontMetrics();

            if (adjustPos) {
                painter.drawText(QRect(width() - 70 - 18, bheight, 57, labelheight - 2), Qt::AlignRight | Qt::AlignVCenter, str);
            } else {
                painter.drawText(QRect(width() - 70, bheight, 57, labelheight - 2), Qt::AlignRight | Qt::AlignVCenter, str);
            }
            painter.restore();
        }
    }
}

void CYearScheduleView::paintItem()
{
    QFont font;
    font.setPixelSize(DDECalendar::FontSizeTwelve);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 反锯齿;
    //左边文字
    painter.save();
    painter.setPen(m_bttextcolor);
    painter.setFont(font);
    QFontMetrics fm = painter.fontMetrics();
    QString tStitlename = tr("No event");

    painter.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, tStitlename);
    painter.restore();
}

CYearScheduleOutView::CYearScheduleOutView(QWidget *parent)
    : DArrowRectangle(DArrowRectangle::ArrowLeft, parent)
{
    yearscheduleview = new CYearScheduleView();
    this->setContent(yearscheduleview);
}

void CYearScheduleOutView::setData(QVector<ScheduleDataInfo> &vListData)
{
    list_count = vListData.size();
    yearscheduleview->setData(vListData);
    scheduleinfoList = yearscheduleview->getlistdate();
}

void CYearScheduleOutView::clearData()
{
    yearscheduleview->clearData();
}

void CYearScheduleOutView::showWindow()
{
    int w = yearscheduleview->showWindow();
    this->setFixedSize(w, yearscheduleview->height());
}

void CYearScheduleOutView::setTheMe(int type)
{
    yearscheduleview->setTheMe(type);
    //根据主题设备不一样的背景色
    if (type == 2) {
        setBackgroundColor(DBlurEffectWidget::DarkColor);
    } else {
        setBackgroundColor(DBlurEffectWidget::LightColor);
    }
}

void CYearScheduleOutView::setCurrentDate(QDate cdate)
{
    currentdate = cdate;
    yearscheduleview->setCurrentDate(cdate);
}

void CYearScheduleOutView::adjustPosition(bool ad)
{
    yearscheduleview->adjustPosition(ad);
}

void CYearScheduleOutView::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    QPoint pos = QCursor::pos();
    pos = this->mapFromGlobal(pos);
    QVector<QRect> rect_press;
    QRect rect(35, 50, width() - 50, 20);
    int listShow = 0;

    if (!scheduleinfoList.isEmpty()) {
        if (scheduleinfoList.size() < DDEYearCalendar::YearScheduleListMaxcount)
            listShow = scheduleinfoList.size();
        else
            listShow = DDEYearCalendar::YearScheduleListMaxcount;
    }
    for (int i = 0; i < listShow; i++) {
        if (this->arrowDirection() == DArrowRectangle::ArrowLeft)
            rect_press.append(QRect(35, 20 + i * 30, width() - 50, 20));
        else
            rect_press.append(QRect(20, 20 + i * 30, width() - 50, 20));
    }
    for (int i = 0; i < listShow; i++) {
        if (rect_press.at(i).contains(pos)) {
            if (i > 3 && list_count > DDEYearCalendar::YearScheduleListMaxcount) {
                emit signalsViewSelectDate(currentdate);
                this->hide();
                //跳转到周视图
            } else {
                //如果日程类型不为节假日或纪念日则显示编辑框
                if (scheduleinfoList.at(i).getType() != DDECalendar::FestivalTypeID) {
                    emit signalViewtransparentFrame(1);
                    //因为提示框会消失，所以设置CScheduleDlg的父类为主窗口
                    CScheduleDlg dlg(0, qobject_cast<QWidget *>(this->parent()));
                    dlg.setData(scheduleinfoList.at(i));
                    dlg.exec();
                    emit signalViewtransparentFrame(0);
                }
            }
        }
    }
}
