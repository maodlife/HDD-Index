#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "solution.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static const QString JsonFileDirPath;
    static const QString RepoJsonFileName;

private slots:
    void on_addNewBtn_clicked();

    void on_hddComboBox_currentIndexChanged(int index);

    void on_createDirBtn_clicked();

    void on_AddToRepoAndDeclareBtn_clicked();

private:
    Ui::MainWindow *ui;
    Solution s;

    void setHddComboboxView();
};
#endif // MAINWINDOW_H
