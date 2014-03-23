#include "mainwindow.h"
#include "ui_mainwindow.h"

YoutubeDL::YoutubeDL(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::YoutubeDL)
{
    ui->setupUi(this);
    setWindowTitle("YouTube-DL Wrapper");
    ui->statusBar->setSizeGripEnabled(false);
    ui->thumbLabel->setScaledContents(true);
}

YoutubeDL::~YoutubeDL()
{
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
    ui->descBox->setText("Fetching...");
    ui->formatBox->addItem("Fetching...");
    fetcher = new QFuture<void>;
    watcher = new QFutureWatcher<void>;
    connect(watcher, SIGNAL(finished()), this, SLOT(writeDesc()));
    *fetcher = QtConcurrent::run(this, &YoutubeDL::runCmd,
                                  "youtube-dl --skip-download --write-thumbnail -o 1.jpg --get-title --get-description " + ui->urlBox->text());
    watcher->setFuture(*fetcher);
}

void YoutubeDL::writeDesc()
{
    if (perr.length())
    {
        ui->descBox->setText("Error occured.\n" + perr);
        ui->thumbLabel->setEnabled(false);
        ui->descBox->setEnabled(false);
    }
    else
    {
        ui->descBox->setText(pout);
        ui->thumbLabel->setPixmap(QPixmap ("1.jpg"));
        ui->thumbLabel->setEnabled(true);
        ui->descBox->setEnabled(true);
    }
    delete fetcher;
    delete watcher;
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
}

void YoutubeDL::on_downloadButton_clicked()
{
    ui->fetchButton->setEnabled(false);
    ui->downloadButton->setEnabled(false);
    ui->descBox->setText("Downloading...");
    QString format = ui->formatBox->currentText(), get;
//improve format grab
    int i=0;
    for (i=0; format[i]!='\t'; i++)
        get[i]=format[i];
    get[i]='\0';
    fetcher = new QFuture<void>;
    watcher = new QFutureWatcher<void>;
    connect(watcher, SIGNAL(finished()), this, SLOT(writeDL()));
    *fetcher = QtConcurrent::run(this, &YoutubeDL::runCmd, "youtube-dl -f " + get + " " + ui->urlBox->text());
    watcher->setFuture(*fetcher);
    /*ui->progressBar->setEnabled(true);
    while (!p.atEnd())
    {
        QString prog = p.readLine();
        QRegularExpression rx("[0-9]{1,3}\\.[0-9]{0,1}]");
        QRegularExpressionMatch m = rx.match(prog);
        if (m.hasMatch()) prog = m.captured(1);
        int perc = prog.toFloat();
        ui->progressBar->setValue(perc);
    }*/

}

void YoutubeDL::writeDL()
{
    delete fetcher;
    delete watcher;
    if (perr.length())
        ui->descBox->setText("Error during download.\n" + perr);
    else
        ui->descBox->setText("Download completed!");
    ui->fetchButton->setEnabled(true);
    ui->downloadButton->setEnabled(true);
}
