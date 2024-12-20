#include "mainwindow.h"

#include <qlogging.h>
#include <qobject.h>

#include <QRegularExpression>
#include <QmimeData>

#include "./ui_mainwindow.h"
#include <QCryptographicHash>

#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

const QString appid = "20241210002225022";
const QString key = "TiG5sGsAeyvPd6ffHWIJ";
const QString url = "http://api.fanyi.baidu.com/api/trans/vip/translate?";
const QString from = "en";
const QString to = "zh";


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
	ui->textBrowser->setStyleSheet("font - size:50px;");
    ui->textBrowser->setText("Hello, World!");
	this->setWindowTitle("划词翻译");
	
	
	manager = new QNetworkAccessManager(this);

    QClipboard *clipboard = QApplication::clipboard();
	
    connect(clipboard, &QClipboard::dataChanged, this,
            &MainWindow::checkClipboard);
	connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::replyFinished);
}

void MainWindow::checkClipboard() {
	qDebug()<<"showclipboard";
    QClipboard *clipboard = QApplication::clipboard();


	/*
    auto data = clipboard->mimeData();
    if (data->hasImage() || data->hasHtml() || !data->hasText()) return;
	*/

    QString text = clipboard->text();
	if (text.isEmpty()) return;
	ui->textBrowser->clear();
	ui->textBrowser->setText(QString("原文: ")+text);

    for (auto e : text) {
        if (e.unicode() >= 0x4e00 && e.unicode() <= 0x9fa5) {
            qDebug() << "含有中文字符";
            return;
        }
    }

    // 纯英文的情况, 将文本进行处理多余换行和空格
    text.replace(QRegularExpression("[\r\n]+"), " ");
    text.replace(QRegularExpression("\\s+"), " ");

	// 将文本翻译为中文
	QString salt = QString::number(rand());
	QString sign = appid + text + salt + key;
    QByteArray hash = QCryptographicHash::hash(sign.toUtf8(), QCryptographicHash::Md5);
    QString md5Sign = hash.toHex();
	QString myurl = url + "appid=" + appid + "&q=" + text + "&from=" + from + "&to=" + to + "&salt=" + salt + "&sign=" + md5Sign;
	
	
	QNetworkRequest request;
	request.setUrl(QUrl(myurl));
	manager->get(request);
	
}


void MainWindow::replyFinished(QNetworkReply* reply) {
	qDebug()<<"replyFinished";
	if (reply->error() != QNetworkReply::NoError) {
		qDebug() << "Error: " << reply->errorString();
		return;
	}

	QByteArray data = reply->readAll();
	QJsonDocument doc = QJsonDocument::fromJson(data);
	QJsonObject obj = doc.object();
	QJsonArray transResultArray = obj["trans_result"].toArray();
	QString result;
	if (!transResultArray.isEmpty()) {
		QJsonObject transResult = transResultArray.first().toObject();
		result = transResult["dst"].toString();
	}
	ui->textBrowser->append(QString("翻译结果: ")+result);
}

MainWindow::~MainWindow() {
	if (ui) delete ui;
	if (manager) delete manager;
}



/*
QTextBrowser provides backward() and forward() slots which you can use to implement 
Back and Forward buttons. The home() slot sets the text to the very first document 
displayed. The anchorClicked() signal is emitted when the user clicks an anchor. 
To override the default navigation behavior of the browser, call the setSource() function 
to supply new document text in a slot connected to this signal.
*/