#include "setkey.h"

#include "ui_setkey.h"
#include <qlogging.h>
#include "user_settings.h"

setkey::setkey(QWidget *parent) : QDialog(parent), ui(new Ui::setkey) {
    ui->setupUi(this);
	qDebug()<<"create setkey UI";
	connect(ui->setkey_button, &QDialogButtonBox::accepted, this, &setkey::on_setkey_button_accepted);
}

setkey::~setkey() { 
	delete ui;
}

void setkey::on_setkey_button_accepted(){
	qDebug()<<"in setkey::on_setkey_button_accepted()";
	::appid = ui->lineEdit->text();
	::key = ui->lineEdit_2->text();
}
