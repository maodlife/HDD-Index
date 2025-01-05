#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "hddtreenode.h"
#include "repotreenode.h"
#include "solution.h"
#include "treenode.h"
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QUrl>
#include <QtCore/qstringliteral.h>
#include <QtWidgets/qpushbutton.h>
#include <algorithm>
#include <memory>
#include <qdebug.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qsplitter.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    CreataUIData(this);
    connectUiData();
    // set hdd combobox
    setHddComboboxView(); // 此时会触发combo box的回调来设置hdd tree view
    // set repository tree view
    this->repoTreeView->setModel(s.repoData.model.get());
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::closeEvent(QCloseEvent *event) {
    bool haveUnsaved = false;
    QString msg;
    if (s.repoData.isDirty) {
        msg += "repo\n";
        haveUnsaved = true;
    }
    for (const auto &hdd : s.hddDataList) {
        if (hdd.isDirty) {
            msg += hdd.labelName + "\n";
            haveUnsaved = true;
        }
    }
    if (haveUnsaved) { // 检查是否有未保存的内容
        QMessageBox::StandardButton reply =
            QMessageBox::warning(this, tr("未保存的更改"),
                                 tr("有未保存的更改。你确定要退出吗？\n") + msg,
                                 QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            event->ignore(); // 取消关闭
            return;
        }
    }
    event->accept(); // 允许关闭
}

void MainWindow::CreataUIData(QMainWindow *parent) {
    QSplitter *splitter = new QSplitter(Qt::Horizontal, parent);
    QSplitter *splitterLeft = new QSplitter(Qt::Vertical, splitter);
    QSplitter *splitterRight = new QSplitter(Qt::Vertical, splitter);
    splitter->addWidget(splitterLeft);
    splitter->addWidget(splitterRight);

    // left
    saveRepoBtn = new QPushButton(splitterLeft);
    saveRepoBtn->setText("保存Repo");
    repoTreeView = new QTreeView(splitterLeft);

    QSplitter *splitterSearchRepo = new QSplitter(Qt::Horizontal, splitterLeft);
    searchRepoNodeLineEdit = new QLineEdit(splitterSearchRepo);
    searchRepoNodeLineEdit->setPlaceholderText("搜索repo节点");
    searchRepoNodeResultLabel = new QLabel();
    searchRepoNodeResultLabel->setText("0/0");
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

    QSplitter *splitterExpandRepo = new QSplitter(Qt::Horizontal, splitterLeft);
    expandAllRepoSaveNodeBtn = new QPushButton(splitterExpandRepo);
    expandAllRepoSaveNodeBtn->setText("一键展开");
    expandAllRepoSaveNodeParentBtn = new QPushButton(splitterExpandRepo);
    expandAllRepoSaveNodeParentBtn->setText("一键展开到父节点");
    splitterExpandRepo->addWidget(expandAllRepoSaveNodeBtn);
    splitterExpandRepo->addWidget(expandAllRepoSaveNodeParentBtn);

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
    splitterLeft->addWidget(splitterExpandRepo);
    splitterLeft->addWidget(splitterRepoSaveNode);
    splitterLeft->addWidget(splitterRepoNodeOp);

    // right
    QSplitter *splitterHddView = new QSplitter(Qt::Horizontal, splitterLeft);
    hddComboBox = new QComboBox(splitterHddView);
    splitterHddView->addWidget(hddComboBox);
    saveHddBtn = new QPushButton(splitterHddView);
    saveHddBtn->setText("保存所有HDD");
    splitterHddView->addWidget(saveHddBtn);

    QSplitter *splitterHddOp = new QSplitter(Qt::Horizontal, splitterRight);
    hddLabelNameLineEdit = new QLineEdit(splitterHddOp);
    splitterHddOp->addWidget(hddLabelNameLineEdit);
    addHddBtn = new QPushButton(splitterRight);
    addHddBtn->setText("读取文件夹并添加HDD");
    splitterHddOp->addWidget(addHddBtn);
    deleteHddBtn = new QPushButton(splitterRight);
    deleteHddBtn->setText("删除当前HDD");
    splitterHddOp->addWidget(deleteHddBtn);

    hddTreeView = new QTreeView(splitterRight);

    QSplitter *splitterHddDirPath =
        new QSplitter(Qt::Horizontal, splitterRight);
    localFileLabel = new QLabel(splitterHddDirPath);
    splitterHddDirPath->addWidget(localFileLabel);
    setLocalFileRootBtn = new QPushButton(splitterHddDirPath);
    setLocalFileRootBtn->setText("连接到本地目录");
    splitterHddDirPath->addWidget(setLocalFileRootBtn);
    clearHddRootDirPathBtn = new QPushButton(splitterHddDirPath);
    clearHddRootDirPathBtn->setText("清除连接");
    splitterHddDirPath->addWidget(clearHddRootDirPathBtn);

    QSplitter *splitterLocalFile = new QSplitter(Qt::Horizontal, splitterRight);
    openLocalFileBtn = new QPushButton(splitterLocalFile);
    openLocalFileBtn->setText("打开对应目录");
    splitterLocalFile->addWidget(openLocalFileBtn);
    refreshHddBtn = new QPushButton(splitterLocalFile);
    refreshHddBtn->setText("读取文件夹并全量刷新");
    splitterLocalFile->addWidget(refreshHddBtn);
    moveLocalFileBtn = new QPushButton(splitterLocalFile);
    moveLocalFileBtn->setText("按照Repo层级移动磁盘文件");
    splitterLocalFile->addWidget(moveLocalFileBtn);

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

    QSplitter *splitterHddNodeCut =
        new QSplitter(Qt::Horizontal, splitterRight);
    cutHddNodeBtn = new QPushButton(splitterHddNodeCut);
    cutHddNodeBtn->setText("剪切");
    splitterHddNodeCut->addWidget(cutHddNodeBtn);
    pasteHddNodeBtn = new QPushButton(splitterHddNodeCut);
    pasteHddNodeBtn->setText("粘贴");
    pasteHddNodeBtn->setEnabled(false);
    splitterHddNodeCut->addWidget(pasteHddNodeBtn);

    splitterRight->addWidget(splitterHddView);
    splitterRight->addWidget(splitterHddOp);
    splitterRight->addWidget(hddTreeView);
    splitterRight->addWidget(splitterHddDirPath);
    splitterRight->addWidget(splitterLocalFile);
    splitterRight->addWidget(splitterHddNodeOp);
    splitterRight->addWidget(splitterHddNodeOp2);
    splitterRight->addWidget(splitterHddNodeCut);

    parent->setCentralWidget(splitter);
}

void MainWindow::connectUiData() {
    connect(this->saveRepoBtn, &QPushButton::clicked, this,
            &MainWindow::on_pushButton_3_clicked);
    connect(this->searchRepoNodeLineEdit, &QLineEdit::textChanged, this,
            &MainWindow::on_searchRepoNodeLineEdit_textChanged);
    connect(this->searchRepoNodePrevBtn, &QPushButton::clicked, this,
            &MainWindow::on_searchRepoNodePrevBtn_clicked);
    connect(this->searchRepoNodeNextBtn, &QPushButton::clicked, this,
            &MainWindow::on_searchRepoNodeNextBtn_clicked);
    connect(this->createRepoSubDirBtn, &QPushButton::clicked, this,
            &MainWindow::on_createDirBtn_clicked);
    connect(this->renameRepoDirBtn, &QPushButton::clicked, this,
            &MainWindow::on_renameRepoBtn_clicked);
    connect(this->expandAllRepoSaveNodeBtn, &QPushButton::clicked, this,
            &MainWindow::on_expandAllRepoSaveNodeBtn_clicked);
    connect(this->expandAllRepoSaveNodeParentBtn, &QPushButton::clicked, this,
            &MainWindow::on_expandAllRepoSaveNodeParentBtn_clicked);
    connect(this->jmpRepoSaveDataHddBtn, &QPushButton::clicked, this,
            &MainWindow::on_jumpToSaveHddNodeBtn_clicked);
    connect(this->deleteRepoNodeBtn, &QPushButton::clicked, this,
            &MainWindow::on_deleteRepoNodeBtn_clicked);
    connect(this->cutRepoNodeBtn, &QPushButton::clicked, this,
            &MainWindow::on_cutBtn_clicked);
    connect(this->pasteRepoNodeBtn, &QPushButton::clicked, this,
            &MainWindow::on_pasteRepoBtn_clicked);
    connect(this->setLocalFileRootBtn, &QPushButton::clicked, this,
            &MainWindow::on_setLocalFileRootBtn_clicked);
    connect(this->clearHddRootDirPathBtn, &QPushButton::clicked, this,
            &MainWindow::on_clearHddRootDirPathBtn_clicked);
    connect(this->openLocalFileBtn, &QPushButton::clicked, this,
            &MainWindow::on_openLocalFileBtn_clicked);
    connect(this->refreshHddBtn, &QPushButton::clicked, this,
            &MainWindow::on_refreshHddBtn_clicked);
    connect(this->moveLocalFileBtn, &QPushButton::clicked, this,
            &MainWindow::on_moveLocalFileBtn_clicked);
    connect(this->saveHddBtn, &QPushButton::clicked, this,
            &MainWindow::on_saveHddBtn_clicked);
    connect(this->addHddBtn, &QPushButton::clicked, this,
            &MainWindow::on_addNewBtn_clicked);
    // connect(this->deleteHddBtn, &QPushButton::clicked, this,
    // &MainWindow::on_saveHddBtn_clicked);
    connect(this->createRepoAndDeclareBtn, &QPushButton::clicked, this,
            &MainWindow::on_AddToRepoAndDeclareBtn_clicked);
    connect(this->declareBtn, &QPushButton::clicked, this,
            &MainWindow::on_declareBtn_clicked);
    connect(this->nodeclareBtn, &QPushButton::clicked, this,
            &MainWindow::on_nodeclareBtn_clicked);
    connect(this->jmpToRepoNodeBtn, &QPushButton::clicked, this,
            &MainWindow::on_jumpToRepoNodeBtn_clicked);
    connect(this->copyHddTreeToRepoBtn, &QPushButton::clicked, this,
            &MainWindow::on_copyHddTreeToRepoBtn_clicked);
    connect(this->hddComboBox, &QComboBox::currentIndexChanged, this,
            &MainWindow::on_hddComboBox_currentIndexChanged);
    connect(this->repoTreeView, &QTreeView::clicked, this,
            &MainWindow::on_repoTreeView_clicked);
    connect(this->guessCanDeclareBtn, &QPushButton::clicked, this,
            &MainWindow::on_guessCanDeclareBtn_clicked);
    connect(this->findSameNameBtn, &QPushButton::clicked, this,
            &MainWindow::on_findSameNameBtn_clicked);
    connect(this->cutHddNodeBtn, &QPushButton::clicked, this,
            &MainWindow::on_cutHddNodeBtn_clicked);
    connect(this->pasteHddNodeBtn, &QPushButton::clicked, this,
            &MainWindow::on_pasteHddNodeBtn_clicked);
}

// 添加HDD
void MainWindow::on_addNewBtn_clicked() {
    auto hddLabel = this->hddLabelNameLineEdit->text();
    // empty label
    if (hddLabel.size() == 0) {
        return;
    }
    // same label
    if (find_if(s.hddDataList.begin(), s.hddDataList.end(),
                [=](const auto &value) {
                    return value.labelName == hddLabel;
                }) != s.hddDataList.end()) {
        return;
    }
    QString selectDirPath = QFileDialog::getExistingDirectory(
        nullptr, "Select Folder", "", QFileDialog::DontResolveSymlinks);
    auto rootPtr = HddTreeNode::CreateTreeNodeByDirPath(selectDirPath);
    HddData hddData;
    hddData.rootPtr = rootPtr;
    hddData.labelName = hddLabel;
    hddData.hasLoaded = true;
    hddData.isDirty = true;
    hddData.dirPath = selectDirPath;
    hddData.model = make_shared<HddTreeModel>(hddData.rootPtr);
    s.hddDataList.push_back(hddData);
    // set hdd combobox
    setHddComboboxView();
}

// hdd连接到本地磁盘
void MainWindow::on_setLocalFileRootBtn_clicked() {
    auto &hddData = s.hddDataList[this->hddComboBox->currentIndex()];
    QString selectDirPath = QFileDialog::getExistingDirectory(
        nullptr, "Select Folder", "", QFileDialog::DontResolveSymlinks);
    if (hddData.dirPath != selectDirPath) {
        hddData.dirPath = selectDirPath;
        hddData.isDirty = true;
        emit this->hddComboBox->currentIndexChanged(
            this->hddComboBox->currentIndex());
    }
}

// 清除本地磁盘链接
void MainWindow::on_clearHddRootDirPathBtn_clicked() {
    auto &hddData = s.hddDataList[this->hddComboBox->currentIndex()];
    if (!hddData.dirPath.isEmpty()) {
        hddData.dirPath.clear();
        hddData.isDirty = true;
        emit this->hddComboBox->currentIndexChanged(
            this->hddComboBox->currentIndex());
    }
}

// 打开对应本地目录
void MainWindow::on_openLocalFileBtn_clicked() {
    const auto &hddData = s.hddDataList[this->hddComboBox->currentIndex()];
    const auto &treeViewIndex = this->hddTreeView->currentIndex();
    auto hddTreeNode = hddData.model->GetSharedPtr(treeViewIndex);
    auto subPath = hddTreeNode->getPath(false);
    qDebug() << subPath;
    QString path = QDir(hddData.dirPath).filePath(subPath);
    qDebug() << path;
    // 用这个接口打开后无法选中文件
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

// 读取文件夹并刷新HDD
void MainWindow::on_refreshHddBtn_clicked() {
    QString selectDirPath = QFileDialog::getExistingDirectory(
        nullptr, "Select Folder", "", QFileDialog::DontResolveSymlinks);
    // 用户二次确认弹窗
    QMessageBox::StandardButton reply = QMessageBox::warning(
        this, tr("警告"),
        tr("选择的文件夹是%1\n确定要全量刷新吗？")
            .arg(selectDirPath),
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::StandardButton::No) {
        return;
    }
    auto &oldHddRootPtr =
        s.hddDataList[this->hddComboBox->currentIndex()].rootPtr;
    auto newHddRootPtr = HddTreeNode::CreateTreeNodeByDirPath(selectDirPath);
    auto [removeNodes, addNodes] =
        HddTreeNode::CompareTree(oldHddRootPtr, newHddRootPtr);
    // compare结果确认
    QString removeResult;
    for (const auto &x : removeNodes) {
        removeResult += x->getPath() + "\n";
    }
    QString addResult;
    for (const auto &x : addNodes) {
        addResult += x->getPath() + "\n";
    }
    QMessageBox::StandardButton compareResultReply =
        QMessageBox::warning(this, tr("警告"),
                             tr("待删除:\n%1\n待新增:\n%2\n是否继续？")
                                 .arg(removeResult)
                                 .arg(addResult),
                             QMessageBox::Yes | QMessageBox::No);
    if (compareResultReply == QMessageBox::StandardButton::No) {
        return;
    }
    QString operationResult;
    bool dirty = false;
    // 删除
    for (const auto &removeNode : removeNodes) {
        // 如果删除的节点的子树中包含了Declare节点，则跳过，让用户自行处理
        auto findRet = TreeNode::findIfInTree(removeNode, [=](const auto &x) {
            auto hddX = dynamic_pointer_cast<HddTreeNode>(x);
            return hddX != nullptr && !hddX->saveData.path.isEmpty();
        });
        if (findRet.size() != 0) {
            operationResult += tr("%1中含有声明节点，已跳过处理。\n")
                                   .arg(removeNode->getPath());
            continue;
        }
        // 如果递归的父节点中有Declare节点，则检查待删除节点是否是必须的
        auto parent = removeNode->parent.lock();
        while (parent != nullptr) {
            auto hddParent = dynamic_pointer_cast<HddTreeNode>(parent);
            if (!hddParent->saveData.path.isEmpty())
                break;
            else
                parent = parent->parent.lock();
        }
        if (parent != nullptr) {
            auto hddParent = dynamic_pointer_cast<HddTreeNode>(parent);
            auto repoParent = TreeNode::getPtrFromPath(
                s.repoData.rootPtr, hddParent->saveData.path);
            auto repoSubNode = TreeNode::getPtrFromPath(
                repoParent, removeNode->getPath(hddParent));
            if (repoSubNode == nullptr) {
                // 没找到
                operationResult +=
                    tr("%1的父节点%2是Declare节点，但%3不必须，已删除。\n")
                        .arg(removeNode->getPath())
                        .arg(hddParent->getPath())
                        .arg(removeNode->name);
                auto removeNodeParent = removeNode->parent.lock();
                removeNodeParent->childs.erase(std::find_if(
                    removeNodeParent->childs.begin(),
                    removeNodeParent->childs.end(), [=](const auto &x) {
                        return x->name == removeNode->name;
                    }));
                dirty = true;
            } else {
                // 找到了
                operationResult +=
                    tr("%1的父节点%2是Declare节点，且%3必须，已跳过。\n")
                        .arg(removeNode->getPath())
                        .arg(hddParent->getPath())
                        .arg(removeNode->name);
            }
            continue;
        }
        // 子树和递归父节点都没有Declare，正常删除
        parent = removeNode->parent.lock();
        auto found = std::find_if(
            parent->childs.begin(), parent->childs.end(),
            [=](const auto &x) { return removeNode->name == x->name; });
        if (found != parent->childs.end()) {
            operationResult += tr("%1已删除\n").arg(removeNode->getPath());
            parent->childs.erase(found);
            dirty = true;
        }
    }
    // 添加
    for (const auto &addNode : addNodes) {
        auto newParent = addNode->parent.lock();
        auto oldParent =
            TreeNode::getPtrFromPath(oldHddRootPtr, newParent->getPath());
        oldParent->childs.push_back(addNode);
        addNode->parent = oldParent;
        oldParent->sortChildByName();
        operationResult += tr("%1已添加\n").arg(addNode->getPath());
    }
    // 输出结果
    QMessageBox operationMsgBox;
    operationMsgBox.setText(operationResult);
    operationMsgBox.exec();
    if (dirty) {
        s.hddDataList[this->hddComboBox->currentIndex()].isDirty = true;
    }
}

// 移动本地文件
void MainWindow::on_moveLocalFileBtn_clicked() {
    // 检查是否连接到本地磁盘
    auto &hddData = s.hddDataList[this->hddComboBox->currentIndex()];
    if (hddData.dirPath.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("还没有连接到本地磁盘"),
                                 QMessageBox::Yes);
        return;
    }
    auto leftIndex = repoTreeView->currentIndex();
    auto leftNode = s.repoData.model->GetSharedPtr(leftIndex);
    if (leftNode == nullptr)
        leftNode = s.repoData.rootPtr;
    // 用户二次确认弹窗
    QMessageBox::StandardButton reply = QMessageBox::warning(
        this, tr("警告"),
        tr("此操作会修改连接的磁盘的文件层级。你确定要继续吗？\n\
左边Repository的%1将视为与右边%2的%3相同层级")
            .arg(leftNode->name)
            .arg(hddData.labelName)
            .arg(hddData.rootPtr->name),
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::StandardButton::No) {
        return;
    }
    // 搜索所有需要移动的hdd node
    auto needMoveNodes =
        TreeNode::findIfInTree(hddData.rootPtr, [](const auto &value) {
            auto hddNode = dynamic_pointer_cast<HddTreeNode>(value);
            return hddNode->saveData.path.size() != 0;
        });
    // 用于记录结果
    QString result;
    // 依次处理每一个
    for (const auto &needMoveNode : needMoveNodes) {
        auto hddNode = dynamic_pointer_cast<HddTreeNode>(needMoveNode);
        // 找到对应的repo节点
        auto repoPath = hddNode->saveData.path;
        auto repoNode = TreeNode::getPtrFromPath(s.repoData.rootPtr, repoPath);
        auto newSubPath = repoNode->getPath(leftNode);
        // 检查是否已经符合repo中的层级
        auto originPath = hddNode->getPath(hddData.rootPtr);
        if (originPath == newSubPath) {
            result += tr("%1 无需移动\n").arg(hddNode->name);
            continue;
        }
        // 检查是否能移动到新的节点中
        auto newParentPath = QFileInfo(newSubPath).path();
        auto newParentNode =
            TreeNode::getPtrFromPath(hddData.rootPtr, newParentPath);
        if (newParentNode == nullptr) {
            // 创建新的父节点, 用model接口
            newParentNode =
                hddData.model->CreatePtrByPath(hddData.rootPtr, newParentPath);
        } else {
            auto it = std::find_if(
                newParentNode->childs.begin(), newParentNode->childs.end(),
                [=](const auto &x) { return x->name == hddNode->name; });
            if (it != newParentNode->childs.end()) {
                QMessageBox::information(
                    this, tr("提示"),
                    tr("%1的目标路径已有同名目录,请检查").arg(hddNode->name),
                    QMessageBox::Yes);
                return;
            }
        }
        // 移动磁盘中的文件夹
        QString sourcePath =
            QDir(hddData.dirPath).filePath(hddNode->getPath(false));
        QString destinationPath = QDir(hddData.dirPath).filePath(newSubPath);
        // result += tr("%1 -> %2\n").arg(sourcePath).arg(destinationPath);
        // 创建所有必要的中间路径
        QDir dir;
        if (!dir.mkpath(QFileInfo(destinationPath).path())) {
            QMessageBox::warning(
                this, tr("error"),
                tr("dir.mkpath %1 fail").arg(QFileInfo(destinationPath).path()),
                QMessageBox::Yes);
            QMessageBox::information(this, tr("结果"), result,
                                     QMessageBox::Yes);
            return;
        }
        // rename操作
        if (!dir.rename(sourcePath, destinationPath)) {
            QMessageBox::warning(this, tr("error"),
                                 tr("qdir.rename %1 %2 fail")
                                     .arg(sourcePath)
                                     .arg(destinationPath),
                                 QMessageBox::Yes);
            QMessageBox::information(this, tr("结果"), result,
                                     QMessageBox::Yes);
            return;
        }
        // 通过model移动treeNode
        hddData.model->CutTreeNode(hddNode, newParentNode);
        hddData.isDirty = true;
        // 修改左边
        auto leftIndex = s.repoData.model->findIndexByTreeNode(repoNode);
        s.repoData.model->ChangeDeclare(leftIndex, hddData.labelName, hddNode);
        s.repoData.isDirty = true;
        // result
        result += tr("%1\n").arg(hddNode->name);
    }
    QMessageBox::information(this, tr("结果"), result, QMessageBox::Yes);
}

// 选择hddComboBox
void MainWindow::on_hddComboBox_currentIndexChanged(int index) {
    if (index < 0 || index >= s.hddDataList.size())
        return;
    auto &hddData = s.hddDataList[index];
    this->hddTreeView->setModel(hddData.model.get());

    this->localFileLabel->setText(hddData.dirPath.isEmpty()
                                      ? "未连接到本地磁盘"
                                      : "已连接到本地磁盘: " + hddData.dirPath);
}

// 创建子目录
void MainWindow::on_createDirBtn_clicked() {
    if (this->createDirNameLineEdit->text().isEmpty()) {
        return;
    }
    auto currRepoTreeIdx = this->repoTreeView->currentIndex();
    s.repoData.model->MakeDir(currRepoTreeIdx,
                              this->createDirNameLineEdit->text());
}

void MainWindow::setHddComboboxView() {
    this->hddComboBox->clear();
    for (const auto &hddData : std::as_const(s.hddDataList)) {
        this->hddComboBox->addItem(hddData.labelName);
    }
}

void MainWindow::setSearchResultView() {
    if (this->searchRepoNodeResult.empty()) {
        this->searchRepoNodeResultLabel->setText("0/0");
        return;
    }
    QString label = "%1/%2";
    this->searchRepoNodeResultLabel->setText(
        label.arg(searchRepoNodeReusltIdx + 1)
            .arg(searchRepoNodeResult.size()));
    auto found = searchRepoNodeResult[searchRepoNodeReusltIdx];
    auto index = s.repoData.model->findIndexByTreeNode(found);
    s.ExpandAndSetTreeViewNode(this->repoTreeView, index);
}

// 新建至Repo，并声明持有
void MainWindow::on_AddToRepoAndDeclareBtn_clicked() {
    auto leftIndex = this->repoTreeView->currentIndex();
    auto rightIndex = this->hddTreeView->currentIndex();
    QString hddLabel = this->hddComboBox->currentText();
    auto rightTreeNodePtr =
        s.hddDataList[this->hddComboBox->currentIndex()].model->GetSharedPtr(
            rightIndex);
    std::shared_ptr<HddTreeNode> hddTreeNodePtr =
        dynamic_pointer_cast<HddTreeNode>(rightTreeNodePtr);
    if (hddTreeNodePtr->saveData.path.isEmpty() == false)
        return;
    // 修改左边
    auto newNode = s.repoData.model->CreateAndDeclare(leftIndex, hddLabel,
                                                      rightTreeNodePtr);
    if (newNode != nullptr) {
        s.repoData.isDirty = true;
    }
    // 修改右边
    s.hddDataList[this->hddComboBox->currentIndex()].model->Declare(
        rightIndex, newNode->getPath());
    s.hddDataList[this->hddComboBox->currentIndex()].isDirty = true;
    // 左边展开
    auto targetRepoIndex = s.repoData.model->findIndexByTreeNode(newNode);
    // 展开到目标节点
    this->repoTreeView->expand(targetRepoIndex.parent());
    // 选中目标节点
    this->repoTreeView->setCurrentIndex(targetRepoIndex);
    // 确保目标节点可见
    this->repoTreeView->scrollTo(targetRepoIndex);
}

// 声明持有
void MainWindow::on_declareBtn_clicked() {
    auto leftIndex = this->repoTreeView->currentIndex();
    auto rightIndex = this->hddTreeView->currentIndex();
    QString hddLabel = this->hddComboBox->currentText();
    auto leftTreeNodePtr = dynamic_pointer_cast<RepoTreeNode>(
        s.repoData.model->GetSharedPtr(leftIndex));
    auto rightTreeNodePtr = dynamic_pointer_cast<HddTreeNode>(
        s.hddDataList[this->hddComboBox->currentIndex()].model->GetSharedPtr(
            rightIndex));
    if (!rightTreeNodePtr->saveData.path.isEmpty()) {
        QMessageBox::information(this, "提示",
                                 "不能声明持有, 因为已声明持有了其他节点");
        return;
    }
    QString errName;
    bool checkCanDeclare =
        Solution::CheckCanDeclare(leftTreeNodePtr, rightTreeNodePtr, errName);
    if (!checkCanDeclare) {
        QMessageBox::information(this, "提示", "不能声明持有" + errName);
        return;
    }
    // 修改左边
    if (s.repoData.model->AddDeclare(leftIndex, hddLabel, rightTreeNodePtr)) {
        s.repoData.isDirty = true;
    }
    // 修改右边
    s.hddDataList[this->hddComboBox->currentIndex()].model->Declare(
        rightIndex, leftTreeNodePtr->getPath());
    s.hddDataList[this->hddComboBox->currentIndex()].isDirty = true;
    // 左边展开
    auto targetRepoIndex =
        s.repoData.model->findIndexByTreeNode(leftTreeNodePtr);
    // 展开到目标节点
    this->repoTreeView->expand(targetRepoIndex.parent());
    // 选中目标节点
    this->repoTreeView->setCurrentIndex(targetRepoIndex);
    // 确保目标节点可见
    this->repoTreeView->scrollTo(targetRepoIndex);
}

void MainWindow::on_nodeclareBtn_clicked() {
    auto rightIndex = this->hddTreeView->currentIndex();
    QString hddLabel = this->hddComboBox->currentText();
    auto &rightModel = s.hddDataList[this->hddComboBox->currentIndex()].model;
    auto rightTreeNodePtr =
        dynamic_pointer_cast<HddTreeNode>(rightModel->GetSharedPtr(rightIndex));
    if (rightTreeNodePtr->saveData.path.isEmpty()) {
        QMessageBox::information(this, "提示", "并没有声明持有repo");
        return;
    }
    auto repoNodePath = rightTreeNodePtr->saveData.path;
    auto repoNode = TreeNode::getPtrFromPath(s.repoData.rootPtr, repoNodePath);
    auto targetRepoIndex = s.repoData.model->findIndexByTreeNode(repoNode);
    // 修改左边
    if (s.repoData.model->RemoveDeclare(targetRepoIndex, hddLabel)) {
        s.repoData.isDirty = true;
    }
    // 修改右边
    if (rightModel->NoDeclare(rightIndex)) {
        s.hddDataList[this->hddComboBox->currentIndex()].isDirty = true;
    }
    // 展开左边
    s.ExpandAndSetTreeViewNode(this->repoTreeView, targetRepoIndex);
    // 展开右边
    s.ExpandAndSetTreeViewNode(this->hddTreeView, rightIndex);
}

// 保存所有HDD
void MainWindow::on_saveHddBtn_clicked() { s.SaveAllHddData(); }

// repo tree view点击
void MainWindow::on_repoTreeView_clicked(const QModelIndex &index) {
    // 显示saveData信息
    auto ptr = s.repoData.model->GetSharedPtr(index);
    this->repoSaveDataHddComboBox->clear();
    shared_ptr<RepoTreeNode> repoPtr = dynamic_pointer_cast<RepoTreeNode>(ptr);
    for (const auto &nodeSaveData : repoPtr->nodeSaveDatas) {
        this->repoSaveDataHddComboBox->addItem(nodeSaveData.hddLabel);
    }
}

// 跳转到存储了这个节点的HDD
void MainWindow::on_jumpToSaveHddNodeBtn_clicked() {
    // 跳转到HDD Tree View中对应节点
    auto ptr =
        s.repoData.model->GetSharedPtr(this->repoTreeView->currentIndex());
    shared_ptr<RepoTreeNode> repoPtr = dynamic_pointer_cast<RepoTreeNode>(ptr);
    auto selectHDDIndex = this->repoSaveDataHddComboBox->currentIndex();
    auto &saveData = repoPtr->nodeSaveDatas[selectHDDIndex];
    if (this->hddComboBox->currentText() != saveData.hddLabel) {
        // 不是当前选择的HDD,切换过去
        // todo
        return;
    }
    auto targetPtr = TreeNode::getPtrFromPath(
        s.hddDataList[this->hddComboBox->currentIndex()].rootPtr,
        saveData.treePath);
    auto targetIndex = s.hddDataList[this->hddComboBox->currentIndex()]
                           .model->findIndexByTreeNode(targetPtr);
    // 展开到目标节点
    this->hddTreeView->expand(targetIndex.parent());
    // 选中目标节点
    this->hddTreeView->setCurrentIndex(targetIndex);
    // 确保目标节点可见
    this->hddTreeView->scrollTo(targetIndex);
}

// 跳转到持有的Repo节点
void MainWindow::on_jumpToRepoNodeBtn_clicked() {
    auto hddTreeViewIndex = this->hddTreeView->currentIndex();
    if (hddTreeViewIndex.isValid() == false)
        return;
    auto treeNodePtr =
        s.hddDataList[this->hddComboBox->currentIndex()].model->GetSharedPtr(
            hddTreeViewIndex);
    auto hddTreeNodePtr = dynamic_pointer_cast<HddTreeNode>(treeNodePtr);
    auto repoNodePath = hddTreeNodePtr->saveData.path;
    auto repoNode = TreeNode::getPtrFromPath(s.repoData.rootPtr, repoNodePath);
    auto targetRepoIndex = s.repoData.model->findIndexByTreeNode(repoNode);
    // 展开到目标节点
    this->repoTreeView->expand(targetRepoIndex.parent());
    // 选中目标节点
    this->repoTreeView->setCurrentIndex(targetRepoIndex);
    // 确保目标节点可见
    this->repoTreeView->scrollTo(targetRepoIndex);
}

// 保存Repository到Json文件
void MainWindow::on_pushButton_3_clicked() {
    if (s.repoData.isDirty == false)
        return;
    s.SaveRepoData();
}

void MainWindow::on_searchRepoNodeLineEdit_textChanged() {
    auto text = this->searchRepoNodeLineEdit->text();
    if (text.isEmpty()) {
        searchRepoNodeResult.clear();
        searchRepoNodeReusltIdx = 0;
        setSearchResultView();
        return;
    }
    searchRepoNodeResult =
        TreeNode::findIfInTree(s.repoData.rootPtr, [=](const auto &ptr) {
            return ptr->name.contains(text);
        });
    if (searchRepoNodeResult.empty()) {
        searchRepoNodeResult.clear();
        searchRepoNodeReusltIdx = 0;
        setSearchResultView();
        return;
    }
    searchRepoNodeReusltIdx = 0;
    setSearchResultView();
}

void MainWindow::on_searchRepoNodePrevBtn_clicked() {
    if (searchRepoNodeResult.empty())
        return;
    if (searchRepoNodeReusltIdx == 0)
        searchRepoNodeReusltIdx = searchRepoNodeResult.size() - 1;
    else
        searchRepoNodeReusltIdx--;
    setSearchResultView();
}

void MainWindow::on_searchRepoNodeNextBtn_clicked() {
    if (searchRepoNodeResult.empty())
        return;
    if (searchRepoNodeReusltIdx == searchRepoNodeResult.size() - 1)
        searchRepoNodeReusltIdx = 0;
    else
        searchRepoNodeReusltIdx++;
    setSearchResultView();
}

// 删除repo节点
void MainWindow::on_deleteRepoNodeBtn_clicked() {
    auto leftIndex = this->repoTreeView->currentIndex();
    auto repoTreeNode = s.repoData.model->GetSharedPtr(leftIndex);
    // 寻找受影响的saveDatas repo node
    auto changedSaveDataRepoNodes =
        TreeNode::findIfInTree(repoTreeNode, [](const auto &value) {
            auto repoValue = dynamic_pointer_cast<RepoTreeNode>(value);
            return repoValue->nodeSaveDatas.size() != 0;
        });
    // 在右边移除对应的声明持有
    for (const auto &repoNode : changedSaveDataRepoNodes) {
        auto repoTreeNode = dynamic_pointer_cast<RepoTreeNode>(repoNode);
        for (const auto &saveData : repoTreeNode->nodeSaveDatas) {
            auto hddLabel = saveData.hddLabel;
            auto &hddData = *find_if(
                s.hddDataList.begin(), s.hddDataList.end(),
                [=](const auto &value) { return value.labelName == hddLabel; });
            auto targetPtr =
                TreeNode::getPtrFromPath(hddData.rootPtr, saveData.treePath);
            auto rightIndex = hddData.model->findIndexByTreeNode(targetPtr);
            hddData.model->NoDeclare(rightIndex);
            hddData.isDirty = true;
        }
    }
    s.repoData.model->removeTreeNode(leftIndex);
    s.repoData.isDirty = true;
}

// 剪切repo节点
void MainWindow::on_cutBtn_clicked() {
    if (s.currCutRepoNode == nullptr) {
        auto leftIndex = this->repoTreeView->currentIndex();
        s.currCutRepoNode = dynamic_pointer_cast<RepoTreeNode>(
            s.repoData.model->GetSharedPtr(leftIndex));
        this->cutRepoNodeBtn->setText("取消剪切");
        this->pasteRepoNodeBtn->setEnabled(true);
    } else {
        s.currCutRepoNode = nullptr;
        this->cutRepoNodeBtn->setText("剪切");
        this->pasteRepoNodeBtn->setEnabled(false);
    }
}

void MainWindow::on_pasteRepoBtn_clicked() {
    if (s.currCutRepoNode == nullptr)
        return;

    auto leftIndex = this->repoTreeView->currentIndex();
    auto newParentNode = dynamic_pointer_cast<RepoTreeNode>(
        s.repoData.model->GetSharedPtr(leftIndex));
    // 修改左边
    s.repoData.model->CutTreeNode(s.currCutRepoNode, newParentNode);
    s.repoData.isDirty = true;
    // 寻找受影响的saveDatas repo node
    auto changedSaveDataRepoNodes =
        TreeNode::findIfInTree(s.currCutRepoNode, [](const auto &value) {
            auto repoValue = dynamic_pointer_cast<RepoTreeNode>(value);
            return repoValue->nodeSaveDatas.size() != 0;
        });
    // 修改右边
    for (const auto &repoNode : changedSaveDataRepoNodes) {
        auto leftNewPathStr = repoNode->getPath();
        auto repoTreeNode = dynamic_pointer_cast<RepoTreeNode>(repoNode);
        for (const auto &saveData : repoTreeNode->nodeSaveDatas) {
            auto hddLabel = saveData.hddLabel;
            auto &hddData = *find_if(
                s.hddDataList.begin(), s.hddDataList.end(),
                [=](const auto &value) { return value.labelName == hddLabel; });
            auto targetPtr =
                TreeNode::getPtrFromPath(hddData.rootPtr, saveData.treePath);
            auto hddTargetPtr = dynamic_pointer_cast<HddTreeNode>(targetPtr);
            // 不能直接用model改，因为可能没有model（没有选中）
            hddData.model->ChangeDeclareRepoPath(hddTargetPtr, leftNewPathStr);
            hddData.isDirty = true;
        }
    }
    // 左边展开
    auto currCutIndex =
        s.repoData.model->findIndexByTreeNode(s.currCutRepoNode);
    s.ExpandAndSetTreeViewNode(this->repoTreeView, currCutIndex);
    // 还原数据
    this->cutRepoNodeBtn->setText("剪切");
    this->pasteRepoNodeBtn->setEnabled(false);
    s.currCutRepoNode = nullptr;
}

// 重命名repo节点
void MainWindow::on_renameRepoBtn_clicked() {
    // 实现上可以做成原地剪切成另一个名字
}

// 一键展开
void MainWindow::on_expandAllRepoSaveNodeBtn_clicked() {
    auto foundList =
        TreeNode::findIfInTree(s.repoData.rootPtr, [](const auto &node) {
            auto repoNode = dynamic_pointer_cast<RepoTreeNode>(node);
            return !repoNode->nodeSaveDatas.empty();
        });
    for (const auto &found : foundList) {
        auto index = s.repoData.model->findIndexByTreeNode(found);
        // repoTreeView->expand(index.parent());
        Solution::ExpandAndSetTreeViewNode(repoTreeView, index);
    }
}

// 一键展开到父节点
void MainWindow::on_expandAllRepoSaveNodeParentBtn_clicked() {
    auto foundList =
        TreeNode::findIfInTree(s.repoData.rootPtr, [](const auto &node) {
            auto repoNode = dynamic_pointer_cast<RepoTreeNode>(node);
            for (const auto &childNode : repoNode->childs) {
                auto childRepoNode =
                    dynamic_pointer_cast<RepoTreeNode>(childNode);
                if (!childRepoNode->nodeSaveDatas.empty())
                    return true;
            }
            return false;
        });
    for (const auto &found : foundList) {
        auto index = s.repoData.model->findIndexByTreeNode(found);
        // repoTreeView->expand(index.parent());
        Solution::ExpandAndSetTreeViewNode(repoTreeView, index);
    }
}

void MainWindow::on_copyHddTreeToRepoBtn_clicked() {
    auto leftIndex = this->repoTreeView->currentIndex();
    auto rightIndex = this->hddTreeView->currentIndex();
    QString hddLabel = ""; // 用不着
    auto rightTreeNodePtr =
        s.hddDataList[this->hddComboBox->currentIndex()].model->GetSharedPtr(
            rightIndex);
    std::shared_ptr<HddTreeNode> hddTreeNodePtr =
        dynamic_pointer_cast<HddTreeNode>(rightTreeNodePtr);
    // 修改左边
    auto newNode = s.repoData.model->CreateAndDeclare(leftIndex, hddLabel,
                                                      rightTreeNodePtr, false);
    if (newNode != nullptr) {
        s.repoData.isDirty = true;
    }
    // 展开左边
    auto newLeftIndex = s.repoData.model->findIndexByTreeNode(newNode);
    s.ExpandAndSetTreeViewNode(this->repoTreeView, newLeftIndex);
}

void MainWindow::on_guessCanDeclareBtn_clicked() {
    auto leftIndex = this->repoTreeView->currentIndex();
    auto rightIndex = this->hddTreeView->currentIndex();
    auto rightPtr = dynamic_pointer_cast<HddTreeNode>(
        s.hddDataList[this->hddComboBox->currentIndex()].model->GetSharedPtr(
            rightIndex));
    QString errName = "";
    auto result = TreeNode::findIfInTree(
        s.repoData.rootPtr, [rightPtr, this, &errName](auto ptr) {
            if (ptr->name != rightPtr->name)
                return false;
            if (s.CheckCanDeclare(ptr, rightPtr, errName)) {
                return true;
            }
            return false;
        });
    if (result.size() == 0)
        return;
    auto repoPtr = dynamic_pointer_cast<RepoTreeNode>(result[0]);
    auto repoIndex = s.repoData.model->findIndexByTreeNode(repoPtr);
    s.ExpandAndSetTreeViewNode(this->repoTreeView, repoIndex);
}

void MainWindow::on_findSameNameBtn_clicked() {
    auto rightIndex = this->hddTreeView->currentIndex();
    auto rightPtr = dynamic_pointer_cast<HddTreeNode>(
        s.hddDataList[this->hddComboBox->currentIndex()].model->GetSharedPtr(
            rightIndex));
    QString errName = "";
    auto result = TreeNode::findIfInTree(s.repoData.rootPtr,
                                         [rightPtr, this, &errName](auto ptr) {
                                             if (ptr->name == rightPtr->name)
                                                 return true;
                                             return false;
                                         });
    if (result.size() == 0)
        return;
    auto repoPtr = dynamic_pointer_cast<RepoTreeNode>(result[0]);
    auto repoIndex = s.repoData.model->findIndexByTreeNode(repoPtr);
    s.ExpandAndSetTreeViewNode(this->repoTreeView, repoIndex);
}

void MainWindow::on_cutHddNodeBtn_clicked() {
    if (s.currCutHddNode == nullptr) {
        auto rightIndex = this->hddTreeView->currentIndex();
        if (!rightIndex.isValid())
            return;
        auto &hddData = s.hddDataList[this->hddComboBox->currentIndex()];
        if (hddData.dirPath.isEmpty()) {
            QMessageBox::warning(this, tr("提示"), tr("还没有连接到本地磁盘"),
                                 QMessageBox::Yes);
            return;
        }
        s.currCutHddNode = dynamic_pointer_cast<HddTreeNode>(
            hddData.model->GetSharedPtr(rightIndex));
        this->cutHddNodeBtn->setText("取消剪切");
        this->pasteHddNodeBtn->setEnabled(true);
    } else {
        s.currCutHddNode = nullptr;
        this->cutHddNodeBtn->setText("剪切");
        this->pasteHddNodeBtn->setEnabled(false);
    }
}

void MainWindow::on_pasteHddNodeBtn_clicked() {
    if (s.currCutHddNode == nullptr)
        return;
    auto &hddData = s.hddDataList[this->hddComboBox->currentIndex()];
    auto rightIndex = this->hddTreeView->currentIndex();
    auto newParentNode = dynamic_pointer_cast<HddTreeNode>(
        hddData.model->GetSharedPtr(rightIndex));
    // 检查是否有同名child
    if (newParentNode->childs.end() !=
        std::find_if(newParentNode->childs.begin(), newParentNode->childs.end(),
                     [=, this](const auto &x) {
                         return x->name == this->s.currCutHddNode->name;
                     })) {
        QMessageBox::StandardButton reply = QMessageBox::warning(
            this, tr("同名子节点"), tr("存在同名子节点"), QMessageBox::Yes);
        return;
    }
    // 移动磁盘中的文件夹
    QString sourcePath =
        QDir(hddData.dirPath).filePath(s.currCutHddNode->getPath(false));
    QString destinationPath =
        QDir(hddData.dirPath)
            .filePath(newParentNode->getPath(false) + QDir::separator() +
                      s.currCutHddNode->name);
    qDebug() << sourcePath;
    qDebug() << destinationPath;
    QDir dir;
    if (!dir.rename(sourcePath, destinationPath)) {
        QMessageBox::StandardButton reply = QMessageBox::warning(
            this, tr("error"), tr("qdir.rename fail"), QMessageBox::Yes);
        return;
    }
    // 通过model移动treeNode
    hddData.model->CutTreeNode(s.currCutHddNode, newParentNode);
    hddData.isDirty = true;
    // 寻找受影响的declare node
    auto changedDeclareHddNodes =
        TreeNode::findIfInTree(s.currCutHddNode, [](const auto &value) {
            auto hddNode = dynamic_pointer_cast<HddTreeNode>(value);
            return hddNode->saveData.path.size() != 0;
        });
    // 修改左边
    for (const auto &treeNode : changedDeclareHddNodes) {
        auto hddTreeNode = dynamic_pointer_cast<HddTreeNode>(treeNode);
        auto repoPath = hddTreeNode->saveData.path;
        auto repoNode = TreeNode::getPtrFromPath(s.repoData.rootPtr, repoPath);
        auto repoIndex = s.repoData.model->findIndexByTreeNode(repoNode);
        bool repoChanged = s.repoData.model->ChangeDeclare(
            repoIndex, hddData.labelName, s.currCutHddNode);
        if (repoChanged) {
            s.repoData.isDirty = true;
        }
    }
    // 展开右边,选中原本选中的父节点
    auto targetIndex =
        hddData.model->findIndexByTreeNode(s.currCutHddNode->parent.lock());
    s.ExpandAndSetTreeViewNode(this->hddTreeView, targetIndex);
    // 恢复UI
    s.currCutHddNode = nullptr;
    this->cutHddNodeBtn->setText("剪切");
    this->pasteHddNodeBtn->setEnabled(false);
}
