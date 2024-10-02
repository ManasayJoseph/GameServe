#ifndef SPEEDOMETERWIDGET_H
#define SPEEDOMETERWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>

class SpeedometerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SpeedometerWidget(QWidget *parent = nullptr);
    void setRange(double min, double max);
    void setValue(double value);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    double m_min;        // Minimum value
    double m_max;        // Maximum value
    double m_value;      // Current value
};

#endif // SPEEDOMETERWIDGET_H
