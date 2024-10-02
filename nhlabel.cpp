#include "nhlabel.h"

NHLabel::NHLabel() {}

#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QUrl>
#include <QDesktopServices>
#include <QMouseEvent>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = nullptr) : QLabel(parent) {
        // Set the cursor to look like a hand when hovering
        setCursor(Qt::PointingHandCursor);
    }

protected:
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            // Open the URL in the default web browser
            QDesktopServices::openUrl(QUrl("https://www.google.com"));
        }
        QLabel::mousePressEvent(event);
    }
};

#endif // CLICKABLELABEL_H
