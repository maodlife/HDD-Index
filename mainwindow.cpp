#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include "treenode.h"
#include "treemodel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_addNewBtn_clicked() {
    auto hddLabel = ui->hddLabelInputTextEdit->toPlainText();
    QString selectDirPath = QFileDialog::getExistingDirectory(
        nullptr, "Select Folder", "", QFileDialog::DontResolveSymlinks);
    auto rootPtr = TreeNode::CreateTreeNodeByDirPath(selectDirPath);
    TreeModel *model = new TreeModel(rootPtr);
    ui->hddTreeView->setModel(model);
}
