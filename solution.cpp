#include "solution.h"
#include <QCoreApplication>
#include <algorithm>
#include <QLabel>

using namespace std;

Solution::Solution() {
    uiData = new UIData();
}

bool Solution::CheckCanDeclare(std::shared_ptr<TreeNode> leftPtr,
                               std::shared_ptr<TreeNode> rightPtr) {
    if (leftPtr->name != rightPtr->name)
        return false;
    for (const auto &child : leftPtr->childs) {
        auto childName = child->name;
        auto foundInRight =
            find_if(rightPtr->childs.begin(), rightPtr->childs.end(),
                                    [=](const auto &value) { return value->name == childName; });
        if (foundInRight == rightPtr->childs.end())
            return false;
        auto rightChild = *foundInRight;
        if (child->isDir && CheckCanDeclare(child, rightChild) == false)
            return false;
    }
    return true;
}

void Solution::ExpandAndSetTreeViewNode(QTreeView *treeView, QModelIndex &index)
{
    // 展开到目标节点
    treeView->expand(index.parent());
    // 选中目标节点
    treeView->setCurrentIndex(index);
    // 确保目标节点可见
    treeView->scrollTo(index);
}

void HddData::LoadJson(QString path) {
    auto rootPtr = TreeNode::loadTreeFromFile<HddTreeNode>(path);
    this->rootPtr = rootPtr;
    this->hasLoaded = true;
}

void HddData::TryLoadJson(QString path) {
    if (this->hasLoaded == false) {
        this->LoadJson(path);
    }
}

void RepoData::LoadJson(QString path)
{
    auto rootPtr = TreeNode::loadTreeFromFile<RepoTreeNode>(path);
    this->rootPtr = rootPtr;
    this->hasLoaded = true;
}

void RepoData::TryLoadJson(QString path)
{
    if (this->hasLoaded == false) {
        this->LoadJson(path);
    }
}

void UIData::CreataUIData(QMainWindow* parent)
{
    QSplitter *splitter = new QSplitter(Qt::Horizontal, parent);
    QSplitter *splitterLeft = new QSplitter(Qt::Vertical, splitter);
    QSplitter *splitterRight = new QSplitter(Qt::Vertical, splitter);
    splitter->addWidget(splitterLeft);
    splitter->addWidget(splitterRight);

    // left
    saveRepoBtn = new QPushButton(splitterLeft);
    saveRepoBtn->setText("保存Repo");
    repoTreeView = new QTreeView(splitterLeft);

    QSplitter *splitterCreateRepoSubDir = new QSplitter(Qt::Horizontal, splitterLeft);
    createDirNameLineEdit = new QLineEdit(splitterCreateRepoSubDir);
    createRepoSubDirBtn = new QPushButton(splitterCreateRepoSubDir);
    createRepoSubDirBtn->setText("创建子目录");
    renameRepoDirBtn = new QPushButton(splitterCreateRepoSubDir);
    renameRepoDirBtn->setText("重命名");
    splitterCreateRepoSubDir->addWidget(createDirNameLineEdit);
    splitterCreateRepoSubDir->addWidget(createRepoSubDirBtn);
    splitterCreateRepoSubDir->addWidget(renameRepoDirBtn);

    QSplitter *splitterRepoSaveNode = new QSplitter(Qt::Horizontal, splitterLeft);
    QLabel *repoSaveNodeLabel = new QLabel(splitterRepoSaveNode);
    repoSaveNodeLabel->setText("保存了该节点的HDD");
    splitterRepoSaveNode->addWidget(repoSaveNodeLabel);
    repoSaveDataHddComboBox = new QComboBox(splitterRepoSaveNode);
    splitterRepoSaveNode->addWidget(repoSaveDataHddComboBox);
    jmpRepoSaveDataHddBtn = new QPushButton(splitterRepoSaveNode);
    jmpRepoSaveDataHddBtn->setText("跳转");
    splitterRepoSaveNode->addWidget(jmpRepoSaveDataHddBtn);

    QSplitter *splitterRepoNodeOp = new QSplitter(Qt::Horizontal, splitterLeft);
    deleteRepoNodeBtn = new QPushButton(splitterRepoNodeOp);
    deleteRepoNodeBtn->setText("删除节点");
    cutRepoNodeBtn = new QPushButton(splitterRepoNodeOp);
    cutRepoNodeBtn->setText("剪切");

    splitterLeft->addWidget(saveRepoBtn);
    splitterLeft->addWidget(repoTreeView);
    splitterLeft->addWidget(splitterCreateRepoSubDir);
    splitterLeft->addWidget(splitterRepoSaveNode);
    splitterLeft->addWidget(splitterRepoNodeOp);

    // right
    QSplitter *splitterHddView = new QSplitter(Qt::Horizontal, splitterLeft);
    hddComboBox = new QComboBox(splitterHddView);
    splitterHddView->addWidget(hddComboBox);
    refreshHddBtn = new QPushButton(splitterHddView);
    refreshHddBtn->setText("刷新");
    splitterHddView->addWidget(refreshHddBtn);
    saveHddBtn = new QPushButton(splitterHddView);
    saveHddBtn->setText("保存所有HDD");
    splitterHddView->addWidget(saveHddBtn);

    QSplitter *splitterHddOp = new QSplitter(Qt::Horizontal, splitterRight);
    hddLabelNameLineEdit = new QLineEdit(splitterHddOp);
    splitterHddOp->addWidget(hddLabelNameLineEdit);
    addHddBtn = new QPushButton(splitterRight);
    addHddBtn->setText("添加");
    splitterHddOp->addWidget(addHddBtn);
    deleteHddBtn = new QPushButton(splitterRight);
    deleteHddBtn->setText("删除当前");
    splitterHddOp->addWidget(deleteHddBtn);

    QSplitter *splitterHddNodeOp = new QSplitter(Qt::Horizontal, splitterRight);
    createRepoAndDeclareBtn = new QPushButton(splitterRight);
    createRepoAndDeclareBtn->setText("新建至Repo并声明持有");
    splitterHddNodeOp->addWidget(createRepoAndDeclareBtn);
    declareBtn = new QPushButton(splitterRight);
    declareBtn->setText("声明持有");
    splitterHddNodeOp->addWidget(declareBtn);
    jmpToRepoNodeBtn = new QPushButton(splitterRight);
    jmpToRepoNodeBtn->setText("跳转至Repo");
    splitterHddNodeOp->addWidget(jmpToRepoNodeBtn);

    hddTreeView = new QTreeView(splitterRight);

    splitterRight->addWidget(splitterHddView);
    splitterRight->addWidget(splitterHddOp);
    splitterRight->addWidget(hddTreeView);
    splitterRight->addWidget(splitterHddNodeOp);

    parent->setCentralWidget(splitter);
}
