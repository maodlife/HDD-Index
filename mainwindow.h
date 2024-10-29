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
    // 添加HDD
    void on_addNewBtn_clicked();

    // 选择hddComboBox
    void on_hddComboBox_currentIndexChanged(int index);

    // 创建子目录
    void on_createDirBtn_clicked();

    // 新建至Repo，并声明持有
    void on_AddToRepoAndDeclareBtn_clicked();

    // 保存所有HDD
    void on_saveHddBtn_clicked();

    // repo tree view点击
    void on_repoTreeView_clicked(const QModelIndex &index);

    // 跳转到存储了这个节点的HDD
    void on_jumpToSaveHddNodeBtn_clicked();

    // 跳转到持有的Repo节点
    void on_jumpToRepoNodeBtn_clicked();

    // 声明持有
    void on_declareBtn_clicked();

private:
    Ui::MainWindow *ui;
    Solution s;

    void setHddComboboxView();
};
#endif // MAINWINDOW_H
