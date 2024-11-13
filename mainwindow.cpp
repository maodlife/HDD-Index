#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "treenode.h"
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <algorithm>

using namespace std;

const QString MainWindow::JsonFileDirName = "/JsonFiles";
const QString MainWindow::RepoJsonFileName = "RepoTreeData.txt";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    if (JsonFileDirPath.size() == 0){
        JsonFileDirPath = QCoreApplication::applicationDirPath();
    }
    // load json file
    QDir jsonFileDir(JsonFileDirPath + JsonFileDirName);
    auto fileList = jsonFileDir.entryInfoList(QDir::Files);
    for (const auto &file : std::as_const(fileList)) {
        if (file.fileName() == RepoJsonFileName) {
            // repository
            QString path = JsonFileDirPath + JsonFileDirName +
                           "/" + RepoJsonFileName;
            s.repoData.TryLoadJson(path);
        } else {
            HddData hddData;
            hddData.labelName = file.fileName();
            hddData.labelName.chop(4);
            s.hddDataList.push_back(hddData);
        }
    }
    // init reposity if empty
    if (s.repoData.hasLoaded == false) {
        std::shared_ptr<RepoTreeNode> rootPtr = std::make_shared<RepoTreeNode>();
        rootPtr->isDir = true;
        rootPtr->name = "Repository";
        s.repoData.rootPtr = rootPtr;
        s.repoData.hasLoaded = true;
    }
    // set hdd combobox
    setHddComboboxView();
    // set repository tree view
    s.repoData.model = make_shared<RepoTreeModel>(s.repoData.rootPtr);
    ui->repoTreeView->setModel(s.repoData.model.get());
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::closeEvent(QCloseEvent *event) {
    bool haveUnsaved = false;
    if (s.repoData.isDirty)
        haveUnsaved = true;
    for (const auto &hdd : s.hddDataList) {
        if (hdd.isDirty)
            haveUnsaved = true;
    }
    if (haveUnsaved) { // 检查是否有未保存的内容
        QMessageBox::StandardButton reply = QMessageBox::warning(
            this, tr("未保存的更改"), tr("有未保存的更改。你确定要退出吗？"),
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            event->ignore(); // 取消关闭
            return;
        }
    }
    event->accept(); // 允许关闭
}

// 添加HDD
void MainWindow::on_addNewBtn_clicked() {
    auto hddLabel = ui->hddLabelInputTextEdit->toPlainText();
    // empty label
    if (hddLabel.size() == 0) {
        return;
    }
    // same label
    if (find_if(s.hddDataList.begin(), s.hddDataList.end(),
                [=](const auto &value) { return value.labelName == hddLabel; }) !=
        s.hddDataList.end()) {
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
    QString path = JsonFileDirPath + JsonFileDirName +
                   "/" + hddData.labelName + ".txt";
    hddData.TryLoadJson(path);
    if (hddData.model == nullptr)
        hddData.model = make_shared<HddTreeModel>(hddData.rootPtr);
    ui->hddTreeView->setModel(hddData.model.get());
}

// 创建子目录
void MainWindow::on_createDirBtn_clicked()
{
    if (ui->createDirNameLineEdit->text().isEmpty()){
        return;
    }
    auto currRepoTreeIdx = ui->repoTreeView->currentIndex();
    s.repoData.model->MakeDir(currRepoTreeIdx, ui->createDirNameLineEdit->text());
}

void MainWindow::setHddComboboxView()
{
    ui->hddComboBox->clear();
    for (const auto &hddData : std::as_const(s.hddDataList)) {
        ui->hddComboBox->addItem(hddData.labelName);
    }
}

// 新建至Repo，并声明持有
void MainWindow::on_AddToRepoAndDeclareBtn_clicked() {
    auto leftIndex = ui->repoTreeView->currentIndex();
    auto rightIndex = ui->hddTreeView->currentIndex();
    QString hddLabel = ui->hddComboBox->currentText();
    auto rightTreeNodePtr =
        s.hddDataList[ui->hddComboBox->currentIndex()].model->GetSharedPtr(
        rightIndex);
    std::shared_ptr<HddTreeNode> hddTreeNodePtr = dynamic_pointer_cast<HddTreeNode>(rightTreeNodePtr);
    if (hddTreeNodePtr->saveData.path.isEmpty() == false)
        return;
    // 修改左边
    auto newNode = s.repoData.model->CreateAndDeclare(leftIndex, hddLabel, rightTreeNodePtr);
    if (newNode != nullptr) {
        s.repoData.isDirty = true;
    }
    // 修改右边
    s.hddDataList[ui->hddComboBox->currentIndex()].model->Declare(
        rightIndex, newNode->getPath());
    s.hddDataList[ui->hddComboBox->currentIndex()].isDirty = true;
    // 左边展开
    auto targetRepoIndex = s.repoData.model->findIndexByTreeNode(newNode);
    // 展开到目标节点
    ui->repoTreeView->expand(targetRepoIndex.parent());
    // 选中目标节点
    ui->repoTreeView->setCurrentIndex(targetRepoIndex);
    // 确保目标节点可见
    ui->repoTreeView->scrollTo(targetRepoIndex);
}

// 声明持有
void MainWindow::on_declareBtn_clicked() {
    auto leftIndex = ui->repoTreeView->currentIndex();
    auto rightIndex = ui->hddTreeView->currentIndex();
    QString hddLabel = ui->hddComboBox->currentText();
    auto leftTreeNodePtr = dynamic_pointer_cast<RepoTreeNode>(
        s.repoData.model->GetSharedPtr(leftIndex));
    auto rightTreeNodePtr = dynamic_pointer_cast<HddTreeNode>(
        s.hddDataList[ui->hddComboBox->currentIndex()].model->GetSharedPtr(
            rightIndex));
    if (!rightTreeNodePtr->saveData.path.isEmpty()){
        QMessageBox::information(this, "提示", "不能声明持有");
        return;
    }
    bool checkCanDeclare =
        Solution::CheckCanDeclare(leftTreeNodePtr, rightTreeNodePtr);
    if (!checkCanDeclare) {
        QMessageBox::information(this, "提示", "不能声明持有");
        return;
    }
    // 修改左边
    s.repoData.model->AddDeclare(leftIndex, hddLabel, rightTreeNodePtr);
    // 修改右边
    s.hddDataList[ui->hddComboBox->currentIndex()].model->Declare(
        rightIndex, leftTreeNodePtr->getPath());
    s.hddDataList[ui->hddComboBox->currentIndex()].isDirty = true;
    // 左边展开
    auto targetRepoIndex = s.repoData.model->findIndexByTreeNode(leftTreeNodePtr);
    // 展开到目标节点
    ui->repoTreeView->expand(targetRepoIndex.parent());
    // 选中目标节点
    ui->repoTreeView->setCurrentIndex(targetRepoIndex);
    // 确保目标节点可见
    ui->repoTreeView->scrollTo(targetRepoIndex);
}

// 保存所有HDD
void MainWindow::on_saveHddBtn_clicked()
{
    for (auto hddData : s.hddDataList){
        if (hddData.isDirty == false)
            continue;
        QString filePath = JsonFileDirPath + JsonFileDirName
                           + "/" + hddData.labelName + ".txt";
        TreeNode::saveTreeToFile(hddData.rootPtr, filePath);
        hddData.isDirty = false;
    }
}

// repo tree view点击
void MainWindow::on_repoTreeView_clicked(const QModelIndex &index)
{
    // 显示saveData信息
    auto ptr = s.repoData.model->GetSharedPtr(index);
    ui->saveHddComboBox->clear();
    shared_ptr<RepoTreeNode> repoPtr = dynamic_pointer_cast<RepoTreeNode>(ptr);
    for (const auto &nodeSaveData : repoPtr->nodeSaveDatas){
        ui->saveHddComboBox->addItem(nodeSaveData.hddLabel);
    }
}

// 跳转到存储了这个节点的HDD
void MainWindow::on_jumpToSaveHddNodeBtn_clicked()
{
    // 跳转到HDD Tree View中对应节点
    auto ptr = s.repoData.model->GetSharedPtr(ui->repoTreeView->currentIndex());
    shared_ptr<RepoTreeNode> repoPtr = dynamic_pointer_cast<RepoTreeNode>(ptr);
    auto selectHDDIndex = ui->saveHddComboBox->currentIndex();
    auto &saveData = repoPtr->nodeSaveDatas[selectHDDIndex];
    if (ui->hddComboBox->currentText() != saveData.hddLabel){
        // 不是当前选择的HDD,切换过去
        // todo
        return;
    }
    auto targetPtr = TreeNode::getPtrFromPath(s.hddDataList[ui->hddComboBox->currentIndex()].rootPtr, saveData.treePath);
    auto targetIndex = s.hddDataList[ui->hddComboBox->currentIndex()].model->findIndexByTreeNode(targetPtr);
    // 展开到目标节点
    ui->hddTreeView->expand(targetIndex.parent());
    // 选中目标节点
    ui->hddTreeView->setCurrentIndex(targetIndex);
    // 确保目标节点可见
    ui->hddTreeView->scrollTo(targetIndex);
}

// 跳转到持有的Repo节点
void MainWindow::on_jumpToRepoNodeBtn_clicked()
{
    auto hddTreeViewIndex = ui->hddTreeView->currentIndex();
    if (hddTreeViewIndex.isValid() == false)
        return;
    auto treeNodePtr = s.hddDataList[ui->hddComboBox->currentIndex()].model->GetSharedPtr(hddTreeViewIndex);
    auto hddTreeNodePtr = dynamic_pointer_cast<HddTreeNode>(treeNodePtr);
    auto repoNodePath = hddTreeNodePtr->saveData.path;
    auto repoNode = TreeNode::getPtrFromPath(s.repoData.rootPtr, repoNodePath);
    auto targetRepoIndex = s.repoData.model->findIndexByTreeNode(repoNode);
    // 展开到目标节点
    ui->repoTreeView->expand(targetRepoIndex.parent());
    // 选中目标节点
    ui->repoTreeView->setCurrentIndex(targetRepoIndex);
    // 确保目标节点可见
    ui->repoTreeView->scrollTo(targetRepoIndex);
}

// 保存Repository到Json文件
void MainWindow::on_pushButton_3_clicked()
{
    if (s.repoData.isDirty == false)
        return;
    QString filePath = JsonFileDirPath+ JsonFileDirName
                       + "/" + RepoJsonFileName;
    TreeNode::saveTreeToFile(s.repoData.rootPtr, filePath);
    s.repoData.isDirty = false;
}

// 删除repo节点
void MainWindow::on_deleteRepoNodeBtn_clicked()
{
    auto leftIndex = ui->repoTreeView->currentIndex();
    // auto repoTreeNode = s.repoData.model->GetSharedPtr(leftIndex);
    // todo: 查找saveData信息并移除，先不做了
    s.repoData.model->removeTreeNode(leftIndex);
    s.repoData.isDirty = true;
}

// 剪切repo节点
void MainWindow::on_cutBtn_clicked() {
    if (s.currCutRepoNode == nullptr) {
        auto leftIndex = ui->repoTreeView->currentIndex();
        s.currCutRepoNode = dynamic_pointer_cast<RepoTreeNode>(
            s.repoData.model->GetSharedPtr(leftIndex));
        ui->cutBtn->setText("Paste");
    } else {
        auto leftIndex = ui->repoTreeView->currentIndex();
        auto newParentNode = dynamic_pointer_cast<RepoTreeNode>(
            s.repoData.model->GetSharedPtr(leftIndex));
        // 修改左边
        s.repoData.model->CutRepoNode(s.currCutRepoNode, newParentNode);
        // 寻找受影响的saveDatas repo node
        auto changedSaveDataRepoNode = TreeNode::findIfInTree(
            s.currCutRepoNode,
            [](const auto &value) {
                auto repoValue = dynamic_pointer_cast<RepoTreeNode>(value);
                return repoValue->nodeSaveDatas.size() != 0;
            });
        // todo
        ui->cutBtn->setText("Cut");
    }
}

// 重命名repo节点
void MainWindow::on_renameRepoBtn_clicked()
{
    // 实现上可以做成原地剪切成另一个名字
}

