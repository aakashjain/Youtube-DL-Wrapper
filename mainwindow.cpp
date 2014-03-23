#include "mainwindow.h"
#include "ui_mainwindow.h"

YoutubeDL::YoutubeDL(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::YoutubeDL)
{
    ui->setupUi(this);
    setWindowTitle("YouTube-DL Wrapper");
    ui->progressBar->hide();
}

YoutubeDL::~YoutubeDL()
{
    QDir remover;
    remove("thumb.jpg");
    delete ui;
}

void YoutubeDL::runCmd(QString command)
{
    QProcess p;
    p.start(command);
    p.waitForFinished(-1);
    pout = p.readAllStandardOutput();
    perr = p.readAllStandardError();
}

void YoutubeDL::on_fetchButton_clicked()
{
    ui->progressBar->show();
    ui->descBox->setEnabled(false);
    ui->formatBox->setEnabled(false);
    ui->downloadButton->setEnabled(false);
    ui->formatBox->clear();
    ui->descBox->setText("Fetching...");
    ui->formatBox->addItem("Fetching...");
    fetcher = new QFuture<void>;
    watcher = new QFutureWatcher<void>;
    connect(watcher, SIGNAL(finished()), this, SLOT(writeDesc()));
    *fetcher = QtConcurrent::run(this, &YoutubeDL::runCmd,
                                 "youtube-dl --skip-download --write-thumbnail -o thumb.jpg --get-title --get-description " +
                                 ui->urlBox->text());
    watcher->setFuture(*fetcher);
}

void YoutubeDL::writeDesc()
{
    delete fetcher;
    delete watcher;
    if (perr.length())
    {
        ui->descBox->setText("Error occured.\n" + perr);
        ui->thumbLabel->setEnabled(false);
        ui->descBox->setEnabled(false);
    }
    else
    {
        ui->descBox->setText(pout);
        ui->thumbLabel->setPixmap(QPixmap ("thumb.jpg"));
        ui->thumbLabel->setEnabled(true);
        ui->descBox->setEnabled(true);
    }
    fetcher = new QFuture<void>;
    watcher = new QFutureWatcher<void>;
    connect(watcher, SIGNAL(finished()), this, SLOT(writeFormats()));
    *fetcher = QtConcurrent::run(this, &YoutubeDL::runCmd, "youtube-dl -F " + ui->urlBox->text());
    watcher->setFuture(*fetcher);
}

void YoutubeDL::writeFormats()
{

    delete fetcher;
    delete watcher;
    ui->formatBox->clear();
    if (perr.length())
    {
        ui->formatBox->addItem("Error occured.");
        ui->formatBox->setEnabled(false);
        ui->downloadButton->setEnabled(false);
    }
    else
    {
        QStringList plist = pout.split('\n');
        foreach(QString line, plist)
            if (line!="Available formats:" && line[0]!='[' && line[0]!='\0')
                ui->formatBox->addItem(line);
        ui->formatBox->setEnabled(true);
        ui->downloadButton->setEnabled(true);
    }
    ui->progressBar->hide();
}

void YoutubeDL::on_downloadButton_clicked()
{
    ui->progressBar->show();
    ui->fetchButton->setEnabled(false);
    ui->downloadButton->setEnabled(false);
    QString format = ui->formatBox->currentText();
    format.remove(format.indexOf('\t'), format.length());
    fetcher = new QFuture<void>;
    watcher = new QFutureWatcher<void>;
    connect(watcher, SIGNAL(finished()), this, SLOT(writeDL()));
    *fetcher = QtConcurrent::run(this, &YoutubeDL::runCmd, "youtube-dl -f " + format + " " + ui->urlBox->text());
    watcher->setFuture(*fetcher);
}

void YoutubeDL::writeDL()
{
    delete fetcher;
    delete watcher;
    if (perr.length())
        ui->descBox->setText("Error during download.\n" + perr);
    else
    {
        ui->descBox->setText("Download completed!");
        QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath()));
    }
    ui->fetchButton->setEnabled(true);
    ui->downloadButton->setEnabled(true);
    ui->progressBar->hide();
}
