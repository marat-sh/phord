#include <QSettings>
#include <QStringList>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , cache(this)
    , fileSystemModel(this)
    , fileListModel(&cache, this)
    , imageListModel(&cache, this)
{
    ui->setupUi(this);

    fileSystemModel.setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    fileSystemModel.setRootPath(QDir::rootPath());

    ui->treeViewDirs->setModel(&fileSystemModel);
    // keep only first column (with names)
    for(int col=fileSystemModel.columnCount()-1; col>0; --col){
        ui->treeViewDirs->hideColumn(col);
    }

    ui->listViewFiles->setModel(&fileListModel);

    ui->imageListView->setModel(&imageListModel);

    connect(ui->treeViewDirs->selectionModel(), &QItemSelectionModel::currentChanged,
            [this](const QModelIndex &current, const QModelIndex &)
    {
        fileListModel.setDir(fileSystemModel.filePath(current));
        // TODO column width
    });

    loadSettings();

    // Some trick. Scroll showed window.
    QTimer::singleShot(100, [this]{ui->treeViewDirs->scrollTo(ui->treeViewDirs->currentIndex());});
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::loadSettings()
{
    QSettings settings;

    settings.beginGroup(QStringLiteral("MainWindow"));

    QByteArray geometry = settings.value(QStringLiteral("geometry")).toByteArray();
    restoreGeometry(geometry);

    QByteArray state = settings.value(QStringLiteral("state")).toByteArray();
    restoreState(state);

    QByteArray splitterState = settings.value(QStringLiteral("splitterState")).toByteArray();
    ui->splitterCentral->restoreState(splitterState);

    QString path = settings.value(QStringLiteral("path"), QDir::homePath()).toString();
    ui->treeViewDirs->selectionModel()->setCurrentIndex(fileSystemModel.index(path), QItemSelectionModel::ClearAndSelect);

    settings.endGroup();

    // TODO work around project saving / loading
    settings.beginGroup(QStringLiteral("Project"));
    QStringList imageNames = settings.value(QStringLiteral("fileList")).toStringList();
    imageListModel.setNames(std::move(imageNames));
}

void MainWindow::saveSettings()
{
    QSettings settings;

    settings.beginGroup(QStringLiteral("MainWindow"));
    settings.setValue(QStringLiteral("geometry"), saveGeometry());
    settings.setValue(QStringLiteral("state"), saveState());
    settings.setValue(QStringLiteral("splitterState"), ui->splitterCentral->saveState());
    settings.setValue(QStringLiteral("path"), ui->treeViewDirs->currentIndex().data(QFileSystemModel::FilePathRole));
    settings.endGroup();

    // TODO work around project saving / loading
    settings.beginGroup(QStringLiteral("Project"));
    settings.setValue(QStringLiteral("fileList"), imageListModel.names());
}
