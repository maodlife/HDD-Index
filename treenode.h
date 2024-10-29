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
    TreeNode();
    std::weak_ptr<TreeNode> parent;                // 父目录
    std::vector<std::shared_ptr<TreeNode>> childs; // 子目录/文件
    QString name;                                  // 自己的名字
    QString dirPath;                               // 绝对路径  todo: delete
    bool isDir;                                    // 是否是目录

    // 保存树结构到 JSON 文件
    template <typename NodeType>
    requires std::derived_from<NodeType, TreeNode>
    static void saveTreeToFile(const std::shared_ptr<NodeType> &root,
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
    static std::shared_ptr<TreeNode> get_shared_ptr(TreeNode* ptr);

    // 从root开始到自己的Path
    QString getPath();

    // 传入Path返回节点指针，Path的开头是this的name
    static std::shared_ptr<TreeNode> getPtrFromPath(std::shared_ptr<TreeNode> rootPtr, QString path);
};

template <typename NodeType>
    requires std::derived_from<NodeType, TreeNode>
void TreeNode::saveTreeToFile(const std::shared_ptr<NodeType> &root,
                              const QString &filePath) {
    QJsonDocument doc(root->toJsonObject());
    QFile file(filePath);
    QDir dir;
    if (!dir.mkpath(QFileInfo(filePath).path())) {  // 创建所有必要的中间路径
        qWarning("Failed to create directory path");
        return;
    }
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

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
    node->dirPath = json["dirPath"].toString();
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

#endif // TREENODE_H
