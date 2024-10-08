#include <QWidget>
#include <QPainter>

class SpeedometerWidget : public QWidget {
    Q_OBJECT

public:
    SpeedometerWidget(QWidget *parent = nullptr) : QWidget(parent) {}

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        int width = this->width();
        int height = this->height();
        int side = qMin(width, height);

        // Translate and scale to center
        painter.translate(width / 2, height / 2);
        painter.scale(side / 200.0, side / 200.0);

        drawDial(&painter);  // Only draw the static outer dial (no needle)
    }

private:
    void drawDial(QPainter *painter) {
        // Draw the speedometer outer dial
        painter->setPen(QPen(Qt::black, 2));
        painter->drawEllipse(-90, -90, 180, 180);  // Circle for the dial

        // Draw tick marks (static)
        painter->setPen(QPen(Qt::black, 3));
        for (int i = 0; i <= 100; i += 10) {
            painter->save();
            painter->rotate(i * 2.7 - 135);  // Rotate for each tick mark

            if (i % 20 == 0) {
                painter->drawLine(0, -70, 0, -80);  // Longer tick mark for 20, 40, 60, etc.
            } else {
                painter->drawLine(0, -75, 0, -80);  // Shorter tick mark
            }
            painter->restore();
        }
    }
};
