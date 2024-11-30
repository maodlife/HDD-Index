#ifndef SOLUTION_H
#define SOLUTION_H

#include "hdddata.h"
#include "repodata.h"
#include <QComboBox>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QSplitter>
#include <QTreeView>
#include <vector>

struct UIData {
    // left ui
    QPushButton *saveRepoBtn;
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
    QPushButton *refreshHddBtn;
    QPushButton *saveHddBtn;
    QLineEdit *hddLabelNameLineEdit;
    QPushButton *addHddBtn;
    QPushButton *deleteHddBtn;
    QTreeView *hddTreeView;
    QPushButton *createRepoAndDeclareBtn;
    QPushButton *declareBtn;
    QPushButton *nodeclareBtn;
    QPushButton *jmpToRepoNodeBtn;
    QPushButton *copyHddTreeToRepoBtn;
    QPushButton *guessCanDeclareBtn;
    QPushButton *findSameNameBtn;

    void CreataUIData(QMainWindow *parent);
};

class Solution {
public:
    std::vector<HddData> hddDataList;
    RepoData repoData;
    UIData *uiData;
    // 暂存剪切的repo节点
    std::shared_ptr<RepoTreeNode> currCutRepoNode;

    QString ProgramDirPath;  // 程序文件保存路径
    QString JsonFileDirName;  // Json文件目录名
    QString RepoJsonFileName;  // Repo数据文件名

    Solution();

    void SaveRepoData();
    void SaveAllHddData();

    // 检查右边能否声明持有左边，要求左边的所有文件和结构都在右边中，但右边可以有多余的
    static bool CheckCanDeclare(std::shared_ptr<TreeNode> leftPtr,
                                std::shared_ptr<TreeNode> rightPtr,
                                QString &errName);

    // 展开并选中
    static void ExpandAndSetTreeViewNode(QTreeView *treeView,
                                         QModelIndex &index);

private:
    void InitDirPathValue();
};

#endif // SOLUTION_H
