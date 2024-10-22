#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include "treenode.h"
#include "treemodel.h"
#include <QDir>

const QString MainWindow::JsonFileDirPath = "/JsonFiles";
const QString MainWindow::RepoJsonFileName = "RepoTreeData.txt";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    // load json file
    QDir jsonFileDir(QCoreApplication::applicationDirPath() + JsonFileDirPath);
    auto fileList = jsonFileDir.entryInfoList(QDir::Files);
    for (const auto &file : std::as_const(fileList)){
        if (file.fileName() == RepoJsonFileName){
            // todo
        }
        else{
            HddData hddData;
            hddData.labelName = file.fileName();
            hddData.labelName.chop(4);
            s.hddDataList.push_back(hddData);
        }
    }
    // set hdd combobox
    for (const auto &hddData : std::as_const(s.hddDataList)){
        ui->hddComboBox->addItem(hddData.labelName);
    }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_addNewBtn_clicked() {
    auto hddLabel = ui->hddLabelInputTextEdit->toPlainText();
    QString selectDirPath = QFileDialog::getExistingDirectory(
        nullptr, "Select Folder", "", QFileDialog::DontResolveSymlinks);
    auto rootPtr = TreeNode::CreateTreeNodeByDirPath(selectDirPath);
    TreeNode::saveTreeToFile(rootPtr, QCoreApplication::applicationDirPath() + "/testjson.txt");
    auto loadRootPtr = TreeNode::loadTreeFromFile(QCoreApplication::applicationDirPath() + "/testjson.txt");
    TreeModel *model = new TreeModel(loadRootPtr);
    ui->hddTreeView->setModel(model);
}
