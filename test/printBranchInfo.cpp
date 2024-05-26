#include <iostream>
#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

void printBranchInfo(const char* rootFileName) {
    // 打开ROOT文件
    TFile file(rootFileName);
    if (file.IsZombie()) {
        std::cerr << "Error opening file: " << rootFileName << std::endl;
        return; // 出错则退出函数
    }
    
    // 获取TTree
    TTree* tree = (TTree*)file.Get("tree");  // 这里使用您TTree的具体名称
    if (!tree) {
        std::cerr << "Error: Tree not found in " << rootFileName << std::endl;
        return;
    }

    // 获取分支并检查它们是否存在
    TBranch* branchJetPt_padded = tree->GetBranch("jet_pt_padded");
    TBranch* branchPartPx = tree->GetBranch("part_px");
    if (!branchJetPt_padded || !branchPartPx) {
        std::cerr << "Error: Branches not found." << std::endl;
        return;
    }

    // 打印分支的数据类型
    std::cout << "Branch 'jet_pt_padded' data type: " << branchJetPt_padded->GetClassName() << std::endl;
    std::cout << "Branch 'part_px' data type: " << branchPartPx->GetClassName() << std::endl;

    // 设置用于读取分支数据的变量
    std::vector<float>* jetPtValues = nullptr;
    std::vector<float>* partPxValues = nullptr;

    // 将变量与分支连接起来
    tree->SetBranchAddress("jet_pt_padded", &jetPtValues);
    tree->SetBranchAddress("part_px", &partPxValues);

    // 读取并打印前5个条目的分支值
    std::cout << "First 5 entries for branches 'jet_pt_paddded' and 'part_px':" << std::endl;
    for (int i = 0; i < 5; ++i) {
        tree->GetEntry(i); // 获取第i个条目

        // 打印 jet_pt 分支的前几个值
        std::cout << "Entry " << i << " - jet_pt_padded size: " << jetPtValues->size();
        if (!jetPtValues->empty()) {
            std::cout << " values: [ ";
            for (size_t j = 0; j < jetPtValues->size(); ++j) {
                std::cout << jetPtValues->at(j) << " ";
            }
            std::cout << "]";
        }
        std::cout << std::endl;

        // 打印 part_px 分支的前几个值
        std::cout << "Entry " << i << " - part_px size: " << partPxValues->size();
        if (!partPxValues->empty()) {
            std::cout << " values: [ ";
            for (size_t j = 0; j < partPxValues->size(); ++j) {
                std::cout << partPxValues->at(j) << " ";
            }
            std::cout << "]";
        }
        std::cout << std::endl;
    }

    // 关闭文件
    file.Close();
}

int main() {
    const char* rootFileName = "/home/liang/Workingspace/weaver-core-main/data/val_5M_new/HToBB_122.root";  // 这里替换为您的ROOT文件名
    printBranchInfo(rootFileName);
    return 0;
}

