#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "treemodel.h"
#include "treenode.h"
#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <algorithm>

using namespace std;

const QString MainWindow::JsonFileDirPath = "/JsonFiles";
const QString MainWindow::RepoJsonFileName = "RepoTreeData.txt";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    // load json file
    QDir jsonFileDir(QCoreApplication::applicationDirPath() + JsonFileDirPath);
    auto fileList = jsonFileDir.entryInfoList(QDir::Files);
    for (const auto &file : std::as_const(fileList)) {
        if (file.fileName() == RepoJsonFileName) {
            // repository
            QString path = QCoreApplication::applicationDirPath() + JsonFileDirPath +
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
    TreeModel *model = new TreeModel(rootPtr);
    ui->hddTreeView->setModel(model);
    // set hdd combobox
    setHddComboboxView();
}

void MainWindow::on_hddComboBox_currentIndexChanged(int index) {
    auto hddData = s.hddDataList[index];
    QString path = QCoreApplication::applicationDirPath() + JsonFileDirPath +
                   "/" + hddData.labelName + ".txt";
    hddData.TryLoadJson(path);
    TreeModel *model = new TreeModel(hddData.rootPtr);
    ui->hddTreeView->setModel(model);
}

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


void MainWindow::on_AddToRepoAndDeclareBtn_clicked()
{
    auto leftIndex = ui->repoTreeView->currentIndex();
    auto rightIndex = ui->hddTreeView->currentIndex();
    QString hddLabel = ui->hddComboBox->currentText();
    auto rightTreeNodePtr = s.hddDataList[ui->hddComboBox->currentIndex()].model->GetSharedPtr(rightIndex);
    // 修改左边
    s.repoData.model->CreateAndDeclare(leftIndex, hddLabel, rightTreeNodePtr);
    // 修改右边
    // todo
}


void MainWindow::on_saveHddBtn_clicked()
{
    for (auto hddData : s.hddDataList){
        if (hddData.isDirty == false)
            continue;
        QString filePath = QCoreApplication::applicationDirPath() + JsonFileDirPath
                           + "/" + hddData.labelName + ".txt";
        TreeNode::saveTreeToFile(hddData.rootPtr, filePath);
        hddData.isDirty = false;
    }
}

