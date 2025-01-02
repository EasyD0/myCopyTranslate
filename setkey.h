#ifndef SETKEY_H
#define SETKEY_H

#include <QDialog>
#include <mutex>
#include <qtmetamacros.h>


namespace Ui {
class setkey;
}

class setkey : public QDialog {
    Q_OBJECT

  public:
	inline static std::mutex mtx{};
  	inline static setkey *Instance{nullptr};
	
	static setkey *getInstance(){
		std::lock_guard lk(mtx);
		if (Instance == nullptr) 
			Instance = new setkey();
		return Instance;	
	} 
    ~setkey();

  private:
    explicit setkey(QWidget *parent = nullptr);

  private:
    Ui::setkey *ui;


  public slots:
    void on_setkey_button_accepted();
};

#endif // SETKEY_H
