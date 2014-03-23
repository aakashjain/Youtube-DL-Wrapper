#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtConcurrent/QtConcurrent>
#include <QProcess>
#include <QProgressDialog>
#include <QRegularExpression>
#include <QDir>
#include <QDesktopServices>

namespace Ui {
class YoutubeDL;
}

class YoutubeDL : public QMainWindow
{
    Q_OBJECT

public:
    explicit YoutubeDL(QWidget *parent = 0);
    ~YoutubeDL();

private slots:
    void on_fetchButton_clicked();

    void on_downloadButton_clicked();

    void runCmd(QString command);

    void writeDesc();

    void writeFormats();

    void writeDL();

private:
    Ui::YoutubeDL *ui;
    QString pout, perr;
    QFuture<void> *fetcher;
    QFutureWatcher<void> *watcher;
};

#endif // YoutubeDL_H
