#include "treemodel.h"
#include <algorithm>
#include <stack>

using namespace std;

TreeModel::TreeModel(std::shared_ptr<TreeNode> ptr) { this->_rootPtr = ptr; }

TreeModel::~TreeModel() { this->_rootPtr = nullptr; }

int TreeModel::rowCount(const QModelIndex &parent) const {
    if (!parent.isValid()) {
        return 1;
    }
    TreeNode *ptr = static_cast<TreeNode *>(parent.internalPointer());
    return ptr->childs.size();
}

int TreeModel::columnCount(const QModelIndex &parent) const { return 1; }

QModelIndex TreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return QModelIndex();
    TreeNode *ptr = static_cast<TreeNode *>(index.internalPointer());
    if (ptr->parent.lock() == nullptr){
        return QModelIndex();
    }
    auto parentPtr = ptr->parent.lock();  // 这是要返回的父项的内部指针
    // 求父项在他的父项中的row
    auto parentparentPtr = parentPtr->parent.lock();
    if (parentparentPtr == nullptr){
        // 说明父项就是树的root
        return createIndex(0, 0, _rootPtr.get());
    }
    auto childs = parentparentPtr->childs;
    auto found = find_if(childs.begin(),
                         childs.end(),
                         [=](auto value){
                             return value->name == parentPtr->name;
                         });
    auto dist = distance(childs.begin(), found);
    return createIndex(dist, 0, ptr->parent.lock().get());
}

QModelIndex TreeModel::index(int row, int column,
                             const QModelIndex &parent) const {
    TreeNode *ptr;
    if (!parent.isValid())
        return createIndex(row, column, _rootPtr.get());
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
        treeNodePtr = TreeNode::get_shared_ptr(
            static_cast<TreeNode *>(index.internalPointer()));
        if (treeNodePtr == nullptr) {
            treeNodePtr = _rootPtr;
        }
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
        return nullptr;
    TreeNode *ptr = static_cast<TreeNode*>(index.internalPointer());
    if (ptr->parent.lock() == nullptr){
        return _rootPtr;
    }
    return TreeNode::get_shared_ptr(ptr);
}

QModelIndex TreeModel::findIndexByTreeNode(std::shared_ptr<TreeNode> ptr)
{
    stack<QString> stack;
    stack.push(ptr->name);
    auto curr = ptr->parent.lock();
    while (curr != nullptr) {
        stack.push(curr->name);
        curr = curr->parent.lock();
    }
    return findIndexByTreeNode(stack);
}

void TreeModel::removeTreeNode(const QModelIndex &index)
{
    auto treeNode = GetSharedPtr(index);
    if (treeNode == nullptr)
        return;
    auto parent = treeNode->parent.lock();
    auto found = find_if(parent->childs.begin(),
                         parent->childs.end(),
                         [=](auto &value){
                             return value->name == treeNode->name;
                         });
    auto row = distance(parent->childs.begin(), found);
    this->beginRemoveRows(index.parent(), row, row);
    parent->childs.erase(found);
    this->endRemoveRows();
}

QModelIndex TreeModel::findIndexByTreeNode(std::stack<QString> stack) {
    auto curr = QModelIndex();
    while (!stack.empty()) {
        auto name = stack.top();
        stack.pop();
        int rowCount = this->rowCount(curr);
        for (int row = 0; row < rowCount; ++row) {
            QModelIndex index = this->index(row, 0, curr);
            TreeNode *currentNode = static_cast<TreeNode *>(index.internalPointer());
            if (currentNode->name == name) {
                curr = index;
                break;
            }
        }
    }
    return curr;
}
