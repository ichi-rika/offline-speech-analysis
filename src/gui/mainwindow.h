#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gui/trackview.h"

class MainWindow : public QMainWindow
{
public:
    MainWindow();

public:
    TrackView *trackView;

};

#endif // MAINWINDOW_H
