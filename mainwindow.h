#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qnetworkreply.h>
#include "ui_mainwindow.h"
#include <QApplication>
#include <QClipboard>
#include <QMainWindow>
#include <QNetworkAccessManager>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

public slots:
  void checkClipboard();
  void replyFinished(QNetworkReply*);
  void showsetkey();

private:
  Ui::MainWindow *ui;
  QNetworkAccessManager *manager;
};
#endif // MAINWINDOW_H
