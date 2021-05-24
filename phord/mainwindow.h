#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>

#include "filelistmodel.h"
#include "imagelistmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:

private:
    void loadSettings();
    void saveSettings();

private:
    Ui::MainWindow *ui;
    QFileSystemModel fileSystemModel;
    FileListModel fileListModel;
    ImageListModel imageListModel;
};
#endif // MAINWINDOW_H
