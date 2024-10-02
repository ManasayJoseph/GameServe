#include "speedometerwidget.h"

SpeedometerWidget::SpeedometerWidget(QWidget *parent)
    : QWidget(parent), m_min(0), m_max(100), m_value(0)
{
    setFixedSize(150, 150); // Set a smaller fixed size for the speedometer
}

void SpeedometerWidget::setRange(double min, double max)
{
    m_min = min;
    m_max = max;
    update(); // Update the widget to reflect changes
}

void SpeedometerWidget::setValue(double value)
{
    if (value < m_min) value = m_min;
    if (value > m_max) value = m_max;
    m_value = value;
    update(); // Update the widget to reflect changes
}

void SpeedometerWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Set center of the widget
    QPointF center(width() / 2, height() / 2);
    double radius = width() / 2 - 20;  // Margin for the arcs and ticks

    // 1. Draw Dark Background
    painter.setBrush(QColor(20, 20, 30));  // Dark background color
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, width(), height());

    // 2. Draw the gradient arc (arc should follow the needle position)
    QRectF arcRect(20, 20, width() - 40, height() - 40);  // Leave space for the gradient arc

    QConicalGradient gradient(center, 240);  // Gradient starts at an angle
    gradient.setColorAt(0.0, QColor(0, 255, 255));   // Cyan
    gradient.setColorAt(1.0, QColor(0, 100, 200));   // Dark Blue

    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QBrush(gradient), 15));  // Use the gradient for the arc

    // Start angle is 30 degrees, and the span will depend on the current value of the needle
    int startAngle = 30 * 16;  // Start at 30 degrees
    int spanAngle = (240 * (m_value - m_min) / (m_max - m_min)) * 16;  // Arc length based on value
    painter.drawArc(arcRect, startAngle, spanAngle);  // Dynamic arc

    // 3. Draw the needle
    double needleAngle = 30 + 240 * (m_value - m_min) / (m_max - m_min);  // Calculate needle angle based on value
    double needleRadian = qDegreesToRadians(needleAngle);
    double needleLength = radius - 10;  // Needle length

    // Draw shadow of the needle for a 3D effect
    painter.setPen(QPen(QColor(0, 0, 0, 100), 6));  // Semi-transparent black shadow
    double shadowX = center.x() + needleLength * qCos(needleRadian);
    double shadowY = center.y() - needleLength * qSin(needleRadian);
    painter.drawLine(center, QPointF(shadowX, shadowY));

    // Draw the actual needle
    painter.setPen(QPen(Qt::white, 4));  // White needle
    painter.drawLine(center, QPointF(shadowX, shadowY));

    // 4. Draw the value in the center
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.drawText(arcRect, Qt::AlignCenter, QString::number(m_value, 'f', 2));  // Show value with two decimal places
}
