#include <iostream>
#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

void processFile(const std::string &inputFile, const std::string &outputFile) {
    // 打开原始ROOT文件
    TFile *inputRoot = TFile::Open(inputFile.c_str(), "READ");
    if (inputRoot == nullptr || inputRoot->IsZombie()) {
        std::cerr << "Error opening file: " << inputFile << std::endl;
        return; // 出错则退出函数
    }
    
    TTree *inputTree;
    inputRoot->GetObject("tree", inputTree); // 使用实际的树名称

    if (inputTree == nullptr) {
        std::cerr << "Error: Tree not found in " << inputFile << std::endl;
        inputRoot->Close();
        return; // 没有找到树则退出函数
    }

    // 禁用原有的 jet_pt 分支
    inputTree->SetBranchStatus("jet_pt", 0);

    // 准备读取 jet_pt 分支
    float jet_pt;
    TBranch *br = inputTree->GetBranch("jet_pt");
    if (br) {
        br->SetAddress(&jet_pt);
    }

    // 创建新的ROOT文件
    TFile *outputRoot = new TFile(outputFile.c_str(), "RECREATE");
    
    // 克隆原始树
    TTree *clonedTree = inputTree->CloneTree(0);
    clonedTree->SetDirectory(outputRoot);
    
    // 现在添加我们的新 jet_pt 分支，类型为 vector<float>
    std::vector<float> jet_pt_vec;
    clonedTree->Branch("jet_pt", &jet_pt_vec);

    // 转换数据 - 复制旧的 branches 并填充新的 jet_pt 分支
    Long64_t nEntries = inputTree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        inputTree->GetEntry(i);
        jet_pt_vec.clear();
        jet_pt_vec.push_back(jet_pt); // 将值添加到 vector 中
        clonedTree->Fill();
    }

    // 保存到新的 ROOT 文件
    clonedTree->AutoSave(); // 自动保存到文件
    delete clonedTree; // 正确删除克隆的树以释放内存
    delete outputRoot; // 删除文件对象会自动执行 CleanUp 和关闭文件
    delete inputRoot; // 关闭并释放原始文件对象
}


int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input ROOT file> <output ROOT file>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    processFile(inputFile, outputFile);

    return EXIT_SUCCESS;
}

