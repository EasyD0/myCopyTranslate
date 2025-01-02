#include "mainwindow.h"

#include <memory>
#include <qlogging.h>
#include <qobject.h>

#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QmimeData>

#include "./ui_mainwindow.h"
#include "setkey.h"
#include "user_settings.h"

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
    connect(manager, &QNetworkAccessManager::finished, this,
            &MainWindow::replyFinished);
	connect(ui->appidset, &QAction::triggered, this, &MainWindow::showsetkey);
}

void MainWindow::checkClipboard() {
	/*
    static QString appid = "20241210002225022";
    static QString key = "TiG5sGsAeyvPd6ffHWIJ";
    static QString from = "en";
    static QString to = "zh";
    static const QString url =
        "http://api.fanyi.baidu.com/api/trans/vip/translate?";
	*/

    qDebug() << "in showclipboard()";
    if (appid.isEmpty() || key.isEmpty()) {
        ui->textBrowser->clear();
        ui->textBrowser->setText("appid或者key失效, 请在设置中修改");
    }
    QClipboard *clipboard = QApplication::clipboard();

    /*
	auto data = clipboard->mimeData();
	if (data->hasImage() || data->hasHtml() || !data->hasText()) return;
    */

    QString text = clipboard->text();
    if (text.isEmpty())
        return;
	
	// 有中文则跳过
	for (auto e : text) {
        if (e.unicode() >= 0x4e00 && e.unicode() <= 0x9fa5) {
            qDebug() << "含有中文字符";
            return;
        }
    }

	// 显示原文
    ui->textBrowser->clear();
    ui->textBrowser->setText(QString("原文: ") + text);

    // 纯英文的情况, 将文本进行处理多余换行和空格
    static const QRegularExpression Rexp1("[\r\n]+");
    static const QRegularExpression Rexp2("\\s+");
    text.replace(Rexp1, " ");
    text.replace(Rexp2, " ");

    // 将文本翻译为中文
    static QString salt = QString::number(rand());
    static QString sign = appid + text + salt + key;
    static QByteArray hash =
        QCryptographicHash::hash(sign.toUtf8(), QCryptographicHash::Md5);
    static QString md5Sign = hash.toHex();
    static QString myurl = url + "appid=" + appid + "&q=" + text + "&from=" + from +
                    "&to=" + to + "&salt=" + salt + "&sign=" + md5Sign;

    static QNetworkRequest request;
    request.setUrl(QUrl(myurl));
    manager->get(request);
}

void MainWindow::replyFinished(QNetworkReply *reply) {
    qDebug() << "in MainWindow::replyFinished(QNetworkReply *reply)";
    if (reply->error() != QNetworkReply::NoError) { // 网络请求get失败
        qDebug() << "Error: " << reply->errorString();
        return;
    }

    // todo 增加可以请求,但返回错误码的情况.
    if (false) {
        ui->textBrowser->clear();
        ui->textBrowser->setText("appid或者key失效, 请在设置中修改");
        showsetkey();// 最好直接弹出该窗口
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
    ui->textBrowser->append(QString("翻译结果: ") + result);
}

MainWindow::~MainWindow() {
    if (ui)
        delete ui;
    if (manager)
        delete manager;
}

void MainWindow::showsetkey() {
	qDebug()<<"in MainWindow::showsetkey()";
    std::shared_ptr<setkey> s {setkey::getInstance()};
	if (s){
		s->show();
		s->exec();
	}
}

/*
QTextBrowser provides backward() and forward() slots which you can use to
implement Back and Forward buttons. The home() slot sets the text to the very
first document displayed. The anchorClicked() signal is emitted when the user
clicks an anchor. To override the default navigation behavior of the browser,
call the setSource() function to supply new document text in a slot connected to
this signal.
*/
