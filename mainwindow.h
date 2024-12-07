#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "solution.h"
#include <QCloseEvent>
#include <QMainWindow>
#include <qpushbutton.h>

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

protected:
    void closeEvent(QCloseEvent *event) override;

    // UI
private:
    // left ui
    QPushButton *saveRepoBtn;
    QLineEdit *searchRepoNodeLineEdit;
    QLabel *searchRepoNodeResultLabel;
    QPushButton *searchRepoNodePrevBtn;
    QPushButton *searchRepoNodeNextBtn;
    QTreeView *repoTreeView;
    QLineEdit *createDirNameLineEdit;
    QPushButton *createRepoSubDirBtn;
    QPushButton *renameRepoDirBtn;
    QComboBox *repoSaveDataHddComboBox;
    QPushButton *jmpRepoSaveDataHddBtn;
    QPushButton *deleteRepoNodeBtn;
    QPushButton *cutRepoNodeBtn;
    QPushButton *pasteRepoNodeBtn;
    // right ui
    QComboBox *hddComboBox;
    QPushButton *saveHddBtn;
    QLineEdit *hddLabelNameLineEdit;
    QPushButton *addHddBtn;
    QPushButton *deleteHddBtn;
    QTreeView *hddTreeView;
    QLabel *localFileLabel;           // 显示连接的本地磁盘信息
    QPushButton *setLocalFileRootBtn; // 设置根目录的本地连接
    QPushButton *refreshHddBtn;       // 刷新HDD与本地磁盘连接
    QPushButton *openLocalFileBtn;
    QPushButton *createRepoAndDeclareBtn;
    QPushButton *declareBtn;
    QPushButton *nodeclareBtn;
    QPushButton *jmpToRepoNodeBtn;
    QPushButton *copyHddTreeToRepoBtn;
    QPushButton *guessCanDeclareBtn;
    QPushButton *findSameNameBtn;

    void CreataUIData(QMainWindow *parent);

    // UI Callback
private:
    void connectUiData();

    // 添加HDD
    void on_addNewBtn_clicked();

    // 刷新HDD
    void on_refreshHddBtn_clicked();

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

    // 放弃声明持有
    void on_nodeclareBtn_clicked();

    // 保存Repository到Json文件
    void on_pushButton_3_clicked();

    // 搜索repo节点
    void on_searchRepoNodeLineEdit_textChanged();

    // 跳转到上一个搜索结果
    void on_searchRepoNodePrevBtn_clicked();

    // 跳转到下一个搜索结果
    void on_searchRepoNodeNextBtn_clicked();

    // 删除repo节点
    void on_deleteRepoNodeBtn_clicked();

    // 剪切repo节点
    void on_cutBtn_clicked();

    // 粘贴repo节点
    void on_pasteRepoBtn_clicked();

    // 重命名repo节点
    void on_renameRepoBtn_clicked();

    // 拷贝文件层级，但不声明持有
    void on_copyHddTreeToRepoBtn_clicked();

    // 跳转到能声明持有的repo节点
    void on_guessCanDeclareBtn_clicked();

    // 跳转到同名的repo节点
    void on_findSameNameBtn_clicked();

private:
    Ui::MainWindow *ui;
    Solution s;

    // 搜索结果
    std::vector<std::shared_ptr<TreeNode>> searchRepoNodeResult;
    int searchRepoNodeReusltIdx;

    void setHddComboboxView();

    void setSearchResultView();
};
#endif // MAINWINDOW_H
