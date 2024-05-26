#include <vector>
#include <string>
#include <iostream>
#include <dirent.h>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TSystem.h"

void AddBranches(std::string inputDir, std::string outputDir) {
    // 先定义将被添加的数据。
    std::vector<float> test_part_pt = {1.2, 1.3, 1.4, 1.5, 1.6};
    std::vector<int> test_part_jetid = {1, 1, 2, 2, 3};
    std::vector<float> test_jet_pt = {6, 7, 8};

    // 打开目录
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(inputDir.c_str())) != NULL) {
        // 遍历目录中的所有文件
        while ((ent = readdir(dir)) != NULL) {
            std::string file_name = ent->d_name;
            // 确保是ROOT文件
            if (file_name.find(".root") != std::string::npos) {
                // 生成输入和输出文件的完整路径
                std::string inputPath = inputDir + "/" + file_name;
                std::string outputPath = outputDir + "/" + file_name;

                // 建立新树
                TFile *inputFile = TFile::Open(inputPath.c_str(), "READ");
                TTree *inputTree = (TTree*)inputFile->Get("tree"); // 替换treeName为你的树名称

                TFile *outputFile = new TFile(outputPath.c_str(), "RECREATE");
                TTree *outputTree = inputTree->CloneTree(-1);

                // 添加新的Branch
                TBranch *branch1 = outputTree->Branch("test_part_pt", &test_part_pt);
                TBranch *branch2 = outputTree->Branch("test_part_jetid", &test_part_jetid);
                TBranch *branch3 = outputTree->Branch("test_jet_pt", &test_jet_pt);

                Long64_t nentries = inputTree->GetEntries();

                // 为每一个entry设置新Branch的值
                for (Long64_t i = 0; i < nentries; i++) {
                    inputTree->GetEntry(i);

                    // 填充新Branch并写入
                    branch1->Fill();
                    branch2->Fill();
                    branch3->Fill();

                    outputTree->Fill();
                }

                // 写文件
                outputTree->Write();
                outputFile->Close();
                inputFile->Close();
            }
        }
        closedir(dir);
    } else {
        // 无法打开目录
        std::cerr << "Cannot open directory: " << inputDir << std::endl;
    }
}

int main() {
    std::string inputDir = "/home/liang/Workingspace/weaver-core-main/data/val_5M";  // 替换为源文件所在目录的路径
    std::string outputDir = "/home/liang/Workingspace/weaver-core-main/data/val_5M_addbranch"; // 替换为输出目录的路径

    // 确保输出目录存在，如果不存在则创建
    gSystem->mkdir(outputDir.c_str(), true);

    AddBranches(inputDir, outputDir);

    return 0;
}

