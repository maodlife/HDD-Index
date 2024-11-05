#ifndef SOLUTION_H
#define SOLUTION_H

#include "hddtreenode.h"
#include "repotreenode.h"
#include "hddtreemodel.h"
#include "repotreemodel.h"
#include <vector>

struct HddData {
    std::shared_ptr<HddTreeNode> rootPtr;
    std::shared_ptr<HddTreeModel> model;
    QString labelName;
    bool hasLoaded = false;
    bool isDirty = false;
    void LoadJson(QString path);
    void TryLoadJson(QString path);
};

struct RepoData {
    std::shared_ptr<RepoTreeNode> rootPtr;
    std::shared_ptr<RepoTreeModel> model;
    bool hasLoaded = false;
    bool isDirty = false;
    void LoadJson(QString path);
    void TryLoadJson(QString path);
};

class Solution {
public:
    Solution();
    std::vector<HddData> hddDataList;
    RepoData repoData;

    std::shared_ptr<RepoTreeNode> currCutRepoNode;

    // 检查右边能否声明持有左边，要求左边的所有文件和结构都在右边中，但右边可以有多余的
    static bool CheckCanDeclare(std::shared_ptr<TreeNode> leftPtr, std::shared_ptr<TreeNode> rightPtr);

private:
};

#endif // SOLUTION_H
