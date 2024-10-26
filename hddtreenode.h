#ifndef HDDTREENODE_H
#define HDDTREENODE_H

#include "treenode.h"

class DeclareSaveData {
    QString path;
    std::weak_ptr<TreeNode> lazyNodePtr;
};

class HddTreeNode : public TreeNode
{
public:
    HddTreeNode();

    // 读文件夹路径, 并访问磁盘目录, 构造一棵树
    static std::shared_ptr<HddTreeNode> CreateTreeNodeByDirPath(QString path);
};

#endif // HDDTREENODE_H
