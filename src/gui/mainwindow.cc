#include <QBoxLayout>
#include "gui/mainwindow.h"

MainWindow::MainWindow()
{
    auto central = new QWidget;
    setCentralWidget(central);

    auto mainLayout = new QVBoxLayout;
    central->setLayout(mainLayout);

    trackView = new TrackView(this);

    mainLayout->addWidget(trackView);
}
