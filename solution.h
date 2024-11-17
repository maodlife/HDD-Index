#ifndef SOLUTION_H
#define SOLUTION_H

#include "hddtreenode.h"
#include "repotreenode.h"
#include "hddtreemodel.h"
#include "repotreemodel.h"
#include <vector>
#include <QMainWindow>
#include <QSplitter>
#include <QPushButton>
#include <QLineEdit>
#include <QTreeView>
#include <QComboBox>

struct HddData {
    std::shared_ptr<HddTreeNode> rootPtr;
    std::shared_ptr<HddTreeModel> model;
    QString labelName;
    bool hasLoaded = false;
    bool isDirty = false;
    void LoadJson(QString path);
    void TryLoadJson(QString path);
};

struct RepoData {
    std::shared_ptr<RepoTreeNode> rootPtr;
    std::shared_ptr<RepoTreeModel> model;
    bool hasLoaded = false;
    bool isDirty = false;
    void LoadJson(QString path);
    void TryLoadJson(QString path);
};

struct UIData{
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
    QPushButton *jmpToRepoNodeBtn;

    void CreataUIData(QMainWindow* parent);

};

class Solution {
public:
    std::vector<HddData> hddDataList;
    RepoData repoData;
    UIData *uiData;

    Solution();

    std::shared_ptr<RepoTreeNode> currCutRepoNode;

    // 检查右边能否声明持有左边，要求左边的所有文件和结构都在右边中，但右边可以有多余的
    static bool CheckCanDeclare(std::shared_ptr<TreeNode> leftPtr, std::shared_ptr<TreeNode> rightPtr);

    void ExpandAndSetTreeViewNode(QTreeView *treeView, QModelIndex &index);

private:
};

#endif // SOLUTION_H
