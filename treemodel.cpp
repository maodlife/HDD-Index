#include "treemodel.h"
#include <algorithm>

using namespace std;

TreeModel::TreeModel(std::shared_ptr<TreeNode> ptr) { this->_rootPtr = ptr; }

TreeModel::~TreeModel() { this->_rootPtr = nullptr; }

int TreeModel::rowCount(const QModelIndex &parent) const {
    if (!parent.isValid()) {
        return _rootPtr->childs.size();
    }
    TreeNode *ptr = static_cast<TreeNode *>(parent.internalPointer());
    return ptr->childs.size();
}

int TreeModel::columnCount(const QModelIndex &parent) const { return 1; }

QModelIndex TreeModel::parent(const QModelIndex &index) const {
    TreeNode *child = static_cast<TreeNode *>(index.internalPointer());
    auto parent = child->parent;
    auto sp = parent.lock();
    if (sp == this->_rootPtr) {
        return QModelIndex();
    } else {
        auto par_par_ptr = sp->parent.lock();
        auto found = std::find(par_par_ptr->childs.begin(),
                               par_par_ptr->childs.end(), par_par_ptr);
        return createIndex(std::distance(par_par_ptr->childs.begin(), found), 0,
                           sp.get());
    }
}

QModelIndex TreeModel::index(int row, int column,
                             const QModelIndex &parent) const {
    TreeNode *ptr;
    if (!parent.isValid())
        ptr = this->_rootPtr.get();
    else {
        ptr = static_cast<TreeNode *>(parent.internalPointer());
    }
    auto child = ptr->childs[row];
    return createIndex(row, column, child.get());
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    TreeNode *ptr = static_cast<TreeNode *>(index.internalPointer());
    return ptr->name;
}

void TreeModel::MakeDir(const QModelIndex &index, QString name) {
    std::shared_ptr<TreeNode> treeNodePtr = nullptr; // index节点的treenode节点指针
    if (index.isValid() == false) {
        // 取根节点
        treeNodePtr = _rootPtr;
    } else {
        treeNodePtr = TreeNode::get_shared_ptr(static_cast<TreeNode*>(index.internalPointer()));
    }
    // 判断有没有同名子目录
    if (find_if(treeNodePtr->childs.begin(), treeNodePtr->childs.end(),
                [name](auto value) { return value->name == name; }) !=
        treeNodePtr->childs.end()) {
        return;
    }
    // 寻找插入位置
    auto point = partition_point(treeNodePtr->childs.begin(), treeNodePtr->childs.end(), [=](auto value){
        return value->name < name;
    });
    auto point_idx = distance(treeNodePtr->childs.begin(), point);
    this->beginInsertRows(index, point_idx, point_idx);
    // 实际插入操作
    treeNodePtr->AddChild(name, _rootPtr);
    this->endInsertRows();
}

std::shared_ptr<TreeNode> TreeModel::GetSharedPtr(const QModelIndex &index)
{
    if (index.isValid() == false)
        return _rootPtr;
    else
        return TreeNode::get_shared_ptr(static_cast<TreeNode*>(index.internalPointer()));
}
