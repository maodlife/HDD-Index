#include "solution.h"
#include <QCoreApplication>
#include <QDir>
#include <QLabel>
#include <QStandardPaths>
#include <algorithm>

using namespace std;

Solution::Solution() {
    uiData = new UIData();
    InitDirPathValue();
    // load json file
    QDir jsonFileDir(ProgramDirPath + "/" + JsonFileDirName);
    auto fileList = jsonFileDir.entryInfoList(QDir::Files);
    for (const auto &file : std::as_const(fileList)) {
        if (file.fileName() == RepoJsonFileName) {
            // repository
            QString path =
                ProgramDirPath + "/" + JsonFileDirName + "/" + RepoJsonFileName;
            this->repoData.TryLoadJson(path);
        } else {
            HddData hddData;
            hddData.labelName = file.fileName();
            hddData.labelName.chop(4);
            this->hddDataList.push_back(hddData);
        }
    }
    // 对所有hdd data load json, 不然之后还要考虑没有load的情况心智负担太大了
    for (auto &hddData : this->hddDataList) {
        QString path = ProgramDirPath + "/" + JsonFileDirName + "/" +
                       hddData.labelName + ".txt";
        hddData.TryLoadJson(path);
    }
    // init reposity if empty
    if (this->repoData.hasLoaded == false) {
        std::shared_ptr<RepoTreeNode> rootPtr =
            std::make_shared<RepoTreeNode>();
        rootPtr->isDir = true;
        rootPtr->name = "Repository";
        this->repoData.rootPtr = rootPtr;
        this->repoData.model = make_shared<RepoTreeModel>(this->repoData.rootPtr);
        this->repoData.hasLoaded = true;
    }
}

void Solution::SaveRepoData()
{
    QString filePath =
        ProgramDirPath + "/" + JsonFileDirName + "/" + RepoJsonFileName;
    TreeNode::saveTreeToFile(this->repoData.rootPtr, filePath);
    this->repoData.isDirty = false;
}

void Solution::SaveAllHddData() {
    for (auto &hddData : this->hddDataList) {
        if (hddData.isDirty == false)
            continue;
        QString filePath = ProgramDirPath + "/" + JsonFileDirName + "/" +
                           hddData.labelName + ".txt";
        TreeNode::saveTreeToFile(hddData.rootPtr, filePath);
        hddData.isDirty = false;
    }
}

bool Solution::CheckCanDeclare(std::shared_ptr<TreeNode> leftPtr,
                               std::shared_ptr<TreeNode> rightPtr,
                               QString &errName) {
    if (leftPtr->name != rightPtr->name) {
        errName = leftPtr->name;
        return false;
    }
    for (const auto &child : leftPtr->childs) {
        auto childName = child->name;
        auto foundInRight = find_if(
            rightPtr->childs.begin(), rightPtr->childs.end(),
            [=](const auto &value) { return value->name == childName; });
        if (foundInRight == rightPtr->childs.end()) {
            errName = childName;
            return false;
        }
        auto rightChild = *foundInRight;
        if (child->isDir &&
            CheckCanDeclare(child, rightChild, errName) == false)
            return false;
    }
    return true;
}

void Solution::ExpandAndSetTreeViewNode(QTreeView *treeView,
                                        QModelIndex &index) {
    // 展开到目标节点
    treeView->expand(index.parent());
    // 选中目标节点
    treeView->setCurrentIndex(index);
    // 确保目标节点可见
    treeView->scrollTo(index);
}

void Solution::InitDirPathValue() {
    if (ProgramDirPath.isEmpty()) {
        ProgramDirPath = QStandardPaths::writableLocation(
                             QStandardPaths::DocumentsLocation) +
                         "/HDD-Index";
    }
    JsonFileDirName = "JsonFiles";
    RepoJsonFileName = "RepoTreeData.txt";
}

void UIData::CreataUIData(QMainWindow *parent) {
    QSplitter *splitter = new QSplitter(Qt::Horizontal, parent);
    QSplitter *splitterLeft = new QSplitter(Qt::Vertical, splitter);
    QSplitter *splitterRight = new QSplitter(Qt::Vertical, splitter);
    splitter->addWidget(splitterLeft);
    splitter->addWidget(splitterRight);

    // left
    saveRepoBtn = new QPushButton(splitterLeft);
    saveRepoBtn->setText("保存Repo");
    repoTreeView = new QTreeView(splitterLeft);

    QSplitter *splitterSearchRepo =
        new QSplitter(Qt::Horizontal, splitterLeft);
    searchRepoNodeLineEdit = new QLineEdit(splitterSearchRepo);
    searchRepoNodeLineEdit->setPlaceholderText("搜索repo节点");
    searchRepoNodeResultLabel = new QLabel();
    searchRepoNodePrevBtn = new QPushButton();
    searchRepoNodePrevBtn->setText("上一个");
    searchRepoNodeNextBtn = new QPushButton();
    searchRepoNodeNextBtn->setText("下一个");
    splitterSearchRepo->addWidget(searchRepoNodeLineEdit);
    splitterSearchRepo->addWidget(searchRepoNodeResultLabel);
    splitterSearchRepo->addWidget(searchRepoNodePrevBtn);
    splitterSearchRepo->addWidget(searchRepoNodeNextBtn);

    QSplitter *splitterCreateRepoSubDir =
        new QSplitter(Qt::Horizontal, splitterLeft);
    createDirNameLineEdit = new QLineEdit(splitterCreateRepoSubDir);
    createRepoSubDirBtn = new QPushButton(splitterCreateRepoSubDir);
    createRepoSubDirBtn->setText("创建子目录");
    renameRepoDirBtn = new QPushButton(splitterCreateRepoSubDir);
    renameRepoDirBtn->setText("重命名");
    splitterCreateRepoSubDir->addWidget(createDirNameLineEdit);
    splitterCreateRepoSubDir->addWidget(createRepoSubDirBtn);
    splitterCreateRepoSubDir->addWidget(renameRepoDirBtn);

    QSplitter *splitterRepoSaveNode =
        new QSplitter(Qt::Horizontal, splitterLeft);
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
    pasteRepoNodeBtn = new QPushButton(splitterRepoNodeOp);
    pasteRepoNodeBtn->setText("粘贴");
    pasteRepoNodeBtn->setEnabled(false);
    splitterRepoNodeOp->addWidget(deleteRepoNodeBtn);
    splitterRepoNodeOp->addWidget(cutRepoNodeBtn);
    splitterRepoNodeOp->addWidget(pasteRepoNodeBtn);

    splitterLeft->addWidget(saveRepoBtn);
    splitterLeft->addWidget(splitterSearchRepo);
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

    hddTreeView = new QTreeView(splitterRight);

    QSplitter *splitterHddNodeOp = new QSplitter(Qt::Horizontal, splitterRight);
    createRepoAndDeclareBtn = new QPushButton(splitterHddNodeOp);
    createRepoAndDeclareBtn->setText("新建至Repo并声明持有");
    splitterHddNodeOp->addWidget(createRepoAndDeclareBtn);
    declareBtn = new QPushButton(splitterHddNodeOp);
    declareBtn->setText("声明持有");
    splitterHddNodeOp->addWidget(declareBtn);
    nodeclareBtn = new QPushButton(splitterHddNodeOp);
    nodeclareBtn->setText("放弃声明持有");
    splitterHddNodeOp->addWidget(nodeclareBtn);
    jmpToRepoNodeBtn = new QPushButton(splitterHddNodeOp);
    jmpToRepoNodeBtn->setText("跳转至Repo");
    splitterHddNodeOp->addWidget(jmpToRepoNodeBtn);

    QSplitter *splitterHddNodeOp2 =
        new QSplitter(Qt::Horizontal, splitterRight);
    copyHddTreeToRepoBtn = new QPushButton(splitterHddNodeOp2);
    copyHddTreeToRepoBtn->setText("拷贝文件层级，但不声明持有");
    splitterHddNodeOp2->addWidget(copyHddTreeToRepoBtn);
    guessCanDeclareBtn = new QPushButton(splitterHddNodeOp2);
    guessCanDeclareBtn->setText("跳转到能声明持有的repo节点");
    splitterHddNodeOp2->addWidget(guessCanDeclareBtn);
    findSameNameBtn = new QPushButton(splitterHddNodeOp2);
    findSameNameBtn->setText("跳转同名repo节点");
    splitterHddNodeOp2->addWidget(findSameNameBtn);

    splitterRight->addWidget(splitterHddView);
    splitterRight->addWidget(splitterHddOp);
    splitterRight->addWidget(hddTreeView);
    splitterRight->addWidget(splitterHddNodeOp);
    splitterRight->addWidget(splitterHddNodeOp2);

    parent->setCentralWidget(splitter);
}
