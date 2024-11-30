#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "hddtreenode.h"
#include "repotreenode.h"
#include "treenode.h"
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QtCore/qstringliteral.h>
#include <QtWidgets/qpushbutton.h>
#include <algorithm>
#include <memory>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    s.uiData->CreataUIData(this);
    connectUiData();
    // set hdd combobox
    setHddComboboxView();  // 此时会触发combo box的回调来设置hdd tree view
    // set repository tree view
    s.uiData->repoTreeView->setModel(s.repoData.model.get());
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

void MainWindow::connectUiData() {
    connect(s.uiData->saveRepoBtn, &QPushButton::clicked, this,
            &MainWindow::on_pushButton_3_clicked);
    connect(s.uiData->searchRepoNodeLineEdit, &QLineEdit::textChanged, this,
            &MainWindow::on_searchRepoNodeLineEdit_textChanged);
    connect(s.uiData->createRepoSubDirBtn, &QPushButton::clicked, this,
            &MainWindow::on_createDirBtn_clicked);
    connect(s.uiData->renameRepoDirBtn, &QPushButton::clicked, this,
            &MainWindow::on_renameRepoBtn_clicked);
    connect(s.uiData->jmpRepoSaveDataHddBtn, &QPushButton::clicked, this,
            &MainWindow::on_jumpToSaveHddNodeBtn_clicked);
    connect(s.uiData->deleteRepoNodeBtn, &QPushButton::clicked, this,
            &MainWindow::on_deleteRepoNodeBtn_clicked);
    connect(s.uiData->cutRepoNodeBtn, &QPushButton::clicked, this,
            &MainWindow::on_cutBtn_clicked);
    connect(s.uiData->pasteRepoNodeBtn, &QPushButton::clicked, this,
            &MainWindow::on_pasteRepoBtn_clicked);
    // connect(s.uiData->refreshHddBtn, &QPushButton::clicked, this,
    // &MainWindow::on_cutBtn_clicked);
    connect(s.uiData->saveHddBtn, &QPushButton::clicked, this,
            &MainWindow::on_saveHddBtn_clicked);
    connect(s.uiData->addHddBtn, &QPushButton::clicked, this,
            &MainWindow::on_addNewBtn_clicked);
    // connect(s.uiData->deleteHddBtn, &QPushButton::clicked, this,
    // &MainWindow::on_saveHddBtn_clicked);
    connect(s.uiData->createRepoAndDeclareBtn, &QPushButton::clicked, this,
            &MainWindow::on_AddToRepoAndDeclareBtn_clicked);
    connect(s.uiData->declareBtn, &QPushButton::clicked, this,
            &MainWindow::on_declareBtn_clicked);
    connect(s.uiData->nodeclareBtn, &QPushButton::clicked, this,
            &MainWindow::on_nodeclareBtn_clicked);
    connect(s.uiData->jmpToRepoNodeBtn, &QPushButton::clicked, this,
            &MainWindow::on_jumpToRepoNodeBtn_clicked);
    connect(s.uiData->copyHddTreeToRepoBtn, &QPushButton::clicked, this,
            &MainWindow::on_copyHddTreeToRepoBtn_clicked);
    connect(s.uiData->hddComboBox, &QComboBox::currentIndexChanged, this,
            &MainWindow::on_hddComboBox_currentIndexChanged);
    connect(s.uiData->repoTreeView, &QTreeView::clicked, this,
            &MainWindow::on_repoTreeView_clicked);
    connect(s.uiData->guessCanDeclareBtn, &QPushButton::clicked, this,
            &MainWindow::on_guessCanDeclareBtn_clicked);
    connect(s.uiData->findSameNameBtn, &QPushButton::clicked, this,
            &MainWindow::on_findSameNameBtn_clicked);
}

// 添加HDD
void MainWindow::on_addNewBtn_clicked() {
    auto hddLabel = s.uiData->hddLabelNameLineEdit->text();
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
    s.hddDataList.push_back(hddData);
    // set hdd combobox
    setHddComboboxView();
}

// 选择hddComboBox
void MainWindow::on_hddComboBox_currentIndexChanged(int index) {
    if (index < 0 || index >= s.hddDataList.size())
        return;
    auto &hddData = s.hddDataList[index];
    s.uiData->hddTreeView->setModel(hddData.model.get());
}

// 创建子目录
void MainWindow::on_createDirBtn_clicked() {
    if (s.uiData->createDirNameLineEdit->text().isEmpty()) {
        return;
    }
    auto currRepoTreeIdx = s.uiData->repoTreeView->currentIndex();
    s.repoData.model->MakeDir(currRepoTreeIdx,
                              s.uiData->createDirNameLineEdit->text());
}

void MainWindow::setHddComboboxView() {
    s.uiData->hddComboBox->clear();
    for (const auto &hddData : std::as_const(s.hddDataList)) {
        s.uiData->hddComboBox->addItem(hddData.labelName);
    }
}

// 新建至Repo，并声明持有
void MainWindow::on_AddToRepoAndDeclareBtn_clicked() {
    auto leftIndex = s.uiData->repoTreeView->currentIndex();
    auto rightIndex = s.uiData->hddTreeView->currentIndex();
    QString hddLabel = s.uiData->hddComboBox->currentText();
    auto rightTreeNodePtr = s.hddDataList[s.uiData->hddComboBox->currentIndex()]
                                .model->GetSharedPtr(rightIndex);
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
    s.hddDataList[s.uiData->hddComboBox->currentIndex()].model->Declare(
        rightIndex, newNode->getPath());
    s.hddDataList[s.uiData->hddComboBox->currentIndex()].isDirty = true;
    // 左边展开
    auto targetRepoIndex = s.repoData.model->findIndexByTreeNode(newNode);
    // 展开到目标节点
    s.uiData->repoTreeView->expand(targetRepoIndex.parent());
    // 选中目标节点
    s.uiData->repoTreeView->setCurrentIndex(targetRepoIndex);
    // 确保目标节点可见
    s.uiData->repoTreeView->scrollTo(targetRepoIndex);
}

// 声明持有
void MainWindow::on_declareBtn_clicked() {
    auto leftIndex = s.uiData->repoTreeView->currentIndex();
    auto rightIndex = s.uiData->hddTreeView->currentIndex();
    QString hddLabel = s.uiData->hddComboBox->currentText();
    auto leftTreeNodePtr = dynamic_pointer_cast<RepoTreeNode>(
        s.repoData.model->GetSharedPtr(leftIndex));
    auto rightTreeNodePtr = dynamic_pointer_cast<HddTreeNode>(
        s.hddDataList[s.uiData->hddComboBox->currentIndex()]
            .model->GetSharedPtr(rightIndex));
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
    s.hddDataList[s.uiData->hddComboBox->currentIndex()].model->Declare(
        rightIndex, leftTreeNodePtr->getPath());
    s.hddDataList[s.uiData->hddComboBox->currentIndex()].isDirty = true;
    // 左边展开
    auto targetRepoIndex =
        s.repoData.model->findIndexByTreeNode(leftTreeNodePtr);
    // 展开到目标节点
    s.uiData->repoTreeView->expand(targetRepoIndex.parent());
    // 选中目标节点
    s.uiData->repoTreeView->setCurrentIndex(targetRepoIndex);
    // 确保目标节点可见
    s.uiData->repoTreeView->scrollTo(targetRepoIndex);
}

void MainWindow::on_nodeclareBtn_clicked() {
    auto rightIndex = s.uiData->hddTreeView->currentIndex();
    QString hddLabel = s.uiData->hddComboBox->currentText();
    auto &rightModel =
        s.hddDataList[s.uiData->hddComboBox->currentIndex()].model;
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
        s.hddDataList[s.uiData->hddComboBox->currentIndex()].isDirty = true;
    }
    // 展开左边
    s.ExpandAndSetTreeViewNode(s.uiData->repoTreeView, targetRepoIndex);
    // 展开右边
    s.ExpandAndSetTreeViewNode(s.uiData->hddTreeView, rightIndex);
}

// 保存所有HDD
void MainWindow::on_saveHddBtn_clicked() {
    s.SaveAllHddData();
}

// repo tree view点击
void MainWindow::on_repoTreeView_clicked(const QModelIndex &index) {
    // 显示saveData信息
    auto ptr = s.repoData.model->GetSharedPtr(index);
    s.uiData->repoSaveDataHddComboBox->clear();
    shared_ptr<RepoTreeNode> repoPtr = dynamic_pointer_cast<RepoTreeNode>(ptr);
    for (const auto &nodeSaveData : repoPtr->nodeSaveDatas) {
        s.uiData->repoSaveDataHddComboBox->addItem(nodeSaveData.hddLabel);
    }
}

// 跳转到存储了这个节点的HDD
void MainWindow::on_jumpToSaveHddNodeBtn_clicked() {
    // 跳转到HDD Tree View中对应节点
    auto ptr =
        s.repoData.model->GetSharedPtr(s.uiData->repoTreeView->currentIndex());
    shared_ptr<RepoTreeNode> repoPtr = dynamic_pointer_cast<RepoTreeNode>(ptr);
    auto selectHDDIndex = s.uiData->repoSaveDataHddComboBox->currentIndex();
    auto &saveData = repoPtr->nodeSaveDatas[selectHDDIndex];
    if (s.uiData->hddComboBox->currentText() != saveData.hddLabel) {
        // 不是当前选择的HDD,切换过去
        // todo
        return;
    }
    auto targetPtr = TreeNode::getPtrFromPath(
        s.hddDataList[s.uiData->hddComboBox->currentIndex()].rootPtr,
        saveData.treePath);
    auto targetIndex = s.hddDataList[s.uiData->hddComboBox->currentIndex()]
                           .model->findIndexByTreeNode(targetPtr);
    // 展开到目标节点
    s.uiData->hddTreeView->expand(targetIndex.parent());
    // 选中目标节点
    s.uiData->hddTreeView->setCurrentIndex(targetIndex);
    // 确保目标节点可见
    s.uiData->hddTreeView->scrollTo(targetIndex);
}

// 跳转到持有的Repo节点
void MainWindow::on_jumpToRepoNodeBtn_clicked() {
    auto hddTreeViewIndex = s.uiData->hddTreeView->currentIndex();
    if (hddTreeViewIndex.isValid() == false)
        return;
    auto treeNodePtr = s.hddDataList[s.uiData->hddComboBox->currentIndex()]
                           .model->GetSharedPtr(hddTreeViewIndex);
    auto hddTreeNodePtr = dynamic_pointer_cast<HddTreeNode>(treeNodePtr);
    auto repoNodePath = hddTreeNodePtr->saveData.path;
    auto repoNode = TreeNode::getPtrFromPath(s.repoData.rootPtr, repoNodePath);
    auto targetRepoIndex = s.repoData.model->findIndexByTreeNode(repoNode);
    // 展开到目标节点
    s.uiData->repoTreeView->expand(targetRepoIndex.parent());
    // 选中目标节点
    s.uiData->repoTreeView->setCurrentIndex(targetRepoIndex);
    // 确保目标节点可见
    s.uiData->repoTreeView->scrollTo(targetRepoIndex);
}

// 保存Repository到Json文件
void MainWindow::on_pushButton_3_clicked() {
    if (s.repoData.isDirty == false)
        return;
    s.SaveRepoData();
}

void MainWindow::on_searchRepoNodeLineEdit_textChanged()
{
    auto text = s.uiData->searchRepoNodeLineEdit->text();
    if (text.isEmpty())
        return;
    auto founds = TreeNode::findIfInTree(s.repoData.rootPtr,
                                         [=](const auto &ptr){
                                             return ptr->name.contains(text);
                                         });
    if (founds.empty())
        return;
    auto found = founds[0];
    auto index = s.repoData.model->findIndexByTreeNode(found);
    s.ExpandAndSetTreeViewNode(s.uiData->repoTreeView, index);
}

// 删除repo节点
void MainWindow::on_deleteRepoNodeBtn_clicked() {
    auto leftIndex = s.uiData->repoTreeView->currentIndex();
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
        auto leftIndex = s.uiData->repoTreeView->currentIndex();
        s.currCutRepoNode = dynamic_pointer_cast<RepoTreeNode>(
            s.repoData.model->GetSharedPtr(leftIndex));
        s.uiData->cutRepoNodeBtn->setText("取消剪切");
        s.uiData->pasteRepoNodeBtn->setEnabled(true);
    } else {
        s.currCutRepoNode = nullptr;
        s.uiData->cutRepoNodeBtn->setText("剪切");
        s.uiData->pasteRepoNodeBtn->setEnabled(false);
    }
}

void MainWindow::on_pasteRepoBtn_clicked() {
    if (s.currCutRepoNode == nullptr)
        return;

    auto leftIndex = s.uiData->repoTreeView->currentIndex();
    auto newParentNode = dynamic_pointer_cast<RepoTreeNode>(
        s.repoData.model->GetSharedPtr(leftIndex));
    // 修改左边
    s.repoData.model->CutRepoNode(s.currCutRepoNode, newParentNode);
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
            hddTargetPtr->saveData.path = leftNewPathStr;
            // 不能直接用model改，因为可能没有model（没有选中）
            // hddData.model->ChangeDeclareRepoPath(hddTargetPtr,
            // leftNewPathStr);
            hddData.isDirty = true;
        }
    }
    // 左边展开
    auto currCutIndex =
        s.repoData.model->findIndexByTreeNode(s.currCutRepoNode);
    s.ExpandAndSetTreeViewNode(s.uiData->repoTreeView, currCutIndex);
    // 还原数据
    s.uiData->cutRepoNodeBtn->setText("剪切");
    s.uiData->pasteRepoNodeBtn->setEnabled(false);
    s.currCutRepoNode = nullptr;
}

// 重命名repo节点
void MainWindow::on_renameRepoBtn_clicked() {
    // 实现上可以做成原地剪切成另一个名字
}

void MainWindow::on_copyHddTreeToRepoBtn_clicked() {
    auto leftIndex = s.uiData->repoTreeView->currentIndex();
    auto rightIndex = s.uiData->hddTreeView->currentIndex();
    QString hddLabel = ""; // 用不着
    auto rightTreeNodePtr = s.hddDataList[s.uiData->hddComboBox->currentIndex()]
                                .model->GetSharedPtr(rightIndex);
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
    s.ExpandAndSetTreeViewNode(s.uiData->repoTreeView, newLeftIndex);
}

void MainWindow::on_guessCanDeclareBtn_clicked() {
    auto leftIndex = s.uiData->repoTreeView->currentIndex();
    auto rightIndex = s.uiData->hddTreeView->currentIndex();
    auto rightPtr = dynamic_pointer_cast<HddTreeNode>(
        s.hddDataList[s.uiData->hddComboBox->currentIndex()]
            .model->GetSharedPtr(rightIndex));
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
    s.ExpandAndSetTreeViewNode(s.uiData->repoTreeView, repoIndex);
}

void MainWindow::on_findSameNameBtn_clicked() {
    auto rightIndex = s.uiData->hddTreeView->currentIndex();
    auto rightPtr = dynamic_pointer_cast<HddTreeNode>(
        s.hddDataList[s.uiData->hddComboBox->currentIndex()]
            .model->GetSharedPtr(rightIndex));
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
    s.ExpandAndSetTreeViewNode(s.uiData->repoTreeView, repoIndex);
}
