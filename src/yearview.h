#ifndef YEARVIEW_H
#define YEARVIEW_H
#include "weekindicator.h"
#include "constants.h"
#include <QWidget>
#include <QList>
#include <QDate>
#include <QStyleOption>
#include <QSignalMapper>
#include "calendardbus.h"
class CalendarDBus;
class CaLunarDayInfo;
class QLabel;
enum CalendarYearDayType {
    SO_YFestival = QStyleOption::SO_CustomBase + 0x01,
    SO_YWeekends = QStyleOption::SO_CustomBase + 0x02,
    SO_YWeekendsAndFestival = SO_YFestival | SO_YWeekends,
    SO_YNotCurrentMonth = 0x04,
    SO_YNotCurrentMonthFestival = SO_YNotCurrentMonth | SO_YFestival,
    SO_YDefault,
};
class CYearView: public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QColor backgroundCircleColor MEMBER m_backgroundCircleColor DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(QColor defaultTextColor MEMBER m_defaultTextColor DESIGNABLE true SCRIPTABLE true)
    // Q_PROPERTY(QColor defaultLunarColor MEMBER m_defaultLunarColor DESIGNABLE true SCRIPTABLE true)
    //Q_PROPERTY(QColor festivalLunarColor MEMBER m_festivalLunarColor DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(QColor weekendsTextColor MEMBER m_weekendsTextColor DESIGNABLE true SCRIPTABLE true)
    ///Q_PROPERTY(QColor weekendsLunarColor MEMBER m_weekendsLunarColor DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(QColor topBorderColor MEMBER m_topBorderColor DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(bool cellSelectable READ cellSelectable WRITE setCellSelectable NOTIFY cellSelectableChanged)
public:
    explicit CYearView(QWidget *parent = 0);
    void setFirstWeekday(int weekday);
    int getDateType(const QDate &date);
    inline bool cellSelectable() const
    {
        return m_cellSelectable;
    }
    void updateSelectState();
signals:
    void dateSelected(const QDate date, const CaLunarDayInfo &detail) const;
    void signalcurrentDateChanged(QDate date);
    void cellSelectableChanged(bool cellSelectable) const;
    void singanleActiveW(CYearView *w);
public slots:
    void setCurrentDate(const QDate date, int type = 0);
    void setCellSelectable(bool selectable);
    void handleCurrentDateChanged(const QDate date, const CaLunarDayInfo &detail);

private:
    int getDateIndex(const QDate &date) const;
    const QString getCellDayNum(int pos);
    const QDate getCellDate(int pos);
    void paintCell(QWidget *cell);
    bool eventFilter(QObject *o, QEvent *e);
    void updateDate();

private slots:
    void cellClicked(QWidget *cell);
    void setSelectedCell(int index);
private:
    QLabel *m_currentMouth;
    QList<QWidget *> m_cellList;
    QDate m_days[42];
    QDate m_currentDate;

    int m_selectedCell = -1;
    bool m_cellSelectable = true;

    QFont m_dayNumFont;

    QColor m_topBorderColor = Qt::red;
    QColor m_backgroundCircleColor = "#2ca7f8";

    QColor m_defaultTextColor = Qt::black;
    QColor m_currentDayTextColor = "#2ca7f8";
    QColor m_weekendsTextColor = Qt::black;
    QColor m_selectedTextColor = Qt::white;
    QColor m_festivalTextColor = Qt::black;
    QColor m_notCurrentTextColor = "#b2b2b2";
    int m_firstWeekDay = 0;
};

#endif // YEARVIEW_H