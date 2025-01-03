#ifndef TREENODE_H
#define TREENODE_H

#include <QAbstractItemModel>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <concepts>
#include <memory>
#include <vector>

class TreeNode {
public:
    std::weak_ptr<TreeNode> parent;                // 父目录
    std::vector<std::shared_ptr<TreeNode>> childs; // 子目录/文件
    QString name;                                  // 自己的名字
    bool isDir;                                    // 是否是目录

    TreeNode();

    // 保存树结构到 JSON 文件
    static void saveTreeToFile(const std::shared_ptr<TreeNode> &root,
                               const QString &filePath);

    // 从 JSON 文件加载树结构
    template <typename NodeType>
        requires std::derived_from<NodeType, TreeNode>
    static std::shared_ptr<NodeType> loadTreeFromFile(const QString &filePath);

    // 将当前节点转换为 QJsonObject
    virtual QJsonObject toJsonObject() const;

    // 从 QJsonObject 中构建 TreeNode
    template <typename NodeType>
        requires std::derived_from<NodeType, TreeNode>
    static std::shared_ptr<NodeType> fromJsonObject(const QJsonObject &json);

    virtual void fromJsonObjectExtend(const QJsonObject &json) = 0;

    virtual void AddChild(QString name, std::shared_ptr<TreeNode> rootPtr);

    void sortChildByName();

    // 返回裸指针的智能指针，不能是根节点, 否则返回nullptr
    static std::shared_ptr<TreeNode> get_shared_ptr(TreeNode *ptr);

    // 从root开始到自己的Path, 包括自己
    QString getPath(bool includeRoot = true);

    // 从root开始到Target的Path, 不含target, 包括自己
    QString getPath(std::shared_ptr<TreeNode> target);

    // 传入Path返回节点指针，Path的开头是rootPtr的name
    static std::shared_ptr<TreeNode>
    getPtrFromPath(std::shared_ptr<TreeNode> rootPtr, QString path);

    // 基于path递归创建节点, path开头和rootPtr不同，直接就是第一个孩子
    template <typename NodeType>
        requires std::derived_from<NodeType, TreeNode>
    static std::shared_ptr<TreeNode>
    CreatePtrByPath(std::shared_ptr<TreeNode> rootPtr, QString path);

    // 从指定节点递归查找，返回所有满足条件的指针
    static std::vector<std::shared_ptr<TreeNode>> findIfInTree(
        std::shared_ptr<TreeNode> rootPtr,
        std::function<bool(const std::shared_ptr<TreeNode> &)> predicate);
};

template <typename NodeType>
    requires std::derived_from<NodeType, TreeNode>
std::shared_ptr<NodeType> TreeNode::loadTreeFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return nullptr;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    return TreeNode::fromJsonObject<NodeType>(doc.object());
}

template <typename NodeType>
    requires std::derived_from<NodeType, TreeNode>
std::shared_ptr<NodeType> TreeNode::fromJsonObject(const QJsonObject &json) {
    auto node = std::make_shared<NodeType>();
    node->name = json["name"].toString();
    node->isDir = json["isDir"].toBool();
    node->fromJsonObjectExtend(json); // 额外数据恢复

    QJsonArray childArray = json["childs"].toArray();
    for (const auto &childValue : std::as_const(childArray)) {
        auto childPtr = fromJsonObject<NodeType>(childValue.toObject());
        childPtr->parent = node;
        node->childs.push_back(childPtr);
    }

    return node;
}

template <typename NodeType>
    requires std::derived_from<NodeType, TreeNode>
std::shared_ptr<TreeNode>
TreeNode::CreatePtrByPath(std::shared_ptr<TreeNode> rootPtr, QString path) {
    auto split_path = path.split('/');
    auto &curr = rootPtr;
    for (const auto &name : split_path) {
        auto it = std::find_if(curr->childs.begin(), curr->childs.end(),
                               [=](const auto &x) { return x->name == name; });
        if (it != curr->childs.end()) {
            curr = *it;
        } else {
            auto newNode = std::make_shared<NodeType>();
            newNode->name = name;
            newNode->parent = curr;
            curr->childs.push_back(newNode);
            curr->sortChildByName();
            curr = newNode;
        }
    }
    return curr;
}

#endif // TREENODE_H
