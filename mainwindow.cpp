#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "treemodel.h"
#include "treenode.h"
#include <QDir>
#include <QFileDialog>

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
        std::shared_ptr<TreeNode> rootPtr = std::make_shared<TreeNode>();
        rootPtr->isDir = true;
        rootPtr->name = "Repository";
        s.repoData.rootPtr = rootPtr;
        s.repoData.hasLoaded = true;
    }
    // set hdd combobox
    for (const auto &hddData : std::as_const(s.hddDataList)) {
        ui->hddComboBox->addItem(hddData.labelName);
    }
    // set repository tree view
    TreeModel *model = new TreeModel(s.repoData.rootPtr);
    ui->repoTreeView->setModel(model);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_addNewBtn_clicked() {
    // auto hddLabel = ui->hddLabelInputTextEdit->toPlainText();
    // QString selectDirPath = QFileDialog::getExistingDirectory(
    //     nullptr, "Select Folder", "", QFileDialog::DontResolveSymlinks);
    // auto rootPtr = TreeNode::CreateTreeNodeByDirPath(selectDirPath);
    // TreeNode::saveTreeToFile(rootPtr, QCoreApplication::applicationDirPath() +
    // "/testjson.txt"); auto loadRootPtr =
    // TreeNode::loadTreeFromFile(QCoreApplication::applicationDirPath() +
    // "/testjson.txt"); TreeModel *model = new TreeModel(loadRootPtr);
    // ui->hddTreeView->setModel(model);
}

void MainWindow::on_hddComboBox_currentIndexChanged(int index) {
    auto hddData = s.hddDataList[index];
    QString path = QCoreApplication::applicationDirPath() + JsonFileDirPath +
                   "/" + hddData.labelName + ".txt";
    hddData.TryLoadJson(path);
    TreeModel *model = new TreeModel(hddData.rootPtr);
    ui->hddTreeView->setModel(model);
}
