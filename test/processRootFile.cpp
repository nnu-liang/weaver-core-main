#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include <vector>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

void padVectorToSize(std::vector<float>& vec, size_t targetSize, float padValue) {
    if (vec.size() < targetSize) {
        vec.resize(targetSize, padValue);
    }
}

void processAndSave(const fs::path& inputFilePath, const fs::path& outputFilePath) {
    // 打开输入ROOT文件并检索树和分支
    TFile inputFile(inputFilePath.string().c_str(), "READ");
    if (!inputFile.IsOpen()) {
        std::cerr << "Error: Unable to open input file " << inputFilePath << std::endl;
        return;
    }
    TTree* inputTree = (TTree*)inputFile.Get("tree"); // 使用实际树名称替换"tree"
    if (!inputTree) {
        std::cerr << "Error: Unable to get the tree from " << inputFilePath << std::endl;
        inputFile.Close();
        return;
    }

    // 检索jet_pt分支，假设它是float类型
    float jetPtValue = 0;
    TBranch* branchJetPt = inputTree->GetBranch("jet_pt");
    if (branchJetPt) {
        inputTree->SetBranchAddress("jet_pt", &jetPtValue);
    } else {
        std::cerr << "Error: Branch jet_pt not found!" << std::endl;
        inputFile.Close();
        return;
    }

    // 创建一个新的ROOT文件和克隆的树来添加填充的jet_pt分支
    TFile outputFile(outputFilePath.string().c_str(), "RECREATE");
    TTree* outputTree = inputTree->CloneTree(0); // 克隆结构，无条目

    // 为填充的jet_pt创建一个新的分支
    std::vector<float> paddedJetPt(128, 0);
    outputTree->Branch("jet_pt_padded", &paddedJetPt);

    // 处理条目
    Long64_t nEntries = inputTree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        inputTree->GetEntry(i); // 获取当前条目

        // 复制原始值并将其放入vector中
        paddedJetPt[0] = jetPtValue;
        padVectorToSize(paddedJetPt, 128, 0.0); // 用0填充到128长度

        outputTree->Fill(); // 在输出树中填充当前条目
    }

    // 写入新树到文件并关闭文件
    outputTree->Write("", TObject::kOverwrite);
    outputFile.Close();
    inputFile.Close();
}
int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input directory> <output directory>" << std::endl;
        return 1;
    }

    fs::path inputDir = argv[1];
    fs::path outputDir = argv[2];

    // Check if the input directory exists
    if (!fs::exists(inputDir) || !fs::is_directory(inputDir)) {
        std::cerr << "Error: The input directory does not exist." << std::endl;
        return 1;
    }

    // Check if the output directory exists, if not, create it
    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir);
    }

    // Iterate over all the ROOT files in the input directory
    for (const auto& entry : fs::directory_iterator(inputDir)) {
        const auto& path = entry.path();
        if (path.extension() == ".root") {
            std::cout << "Processing: " << path.filename() << std::endl;

            // Construct the corresponding output file path
            fs::path outputFilePath = outputDir / path.filename();

            // Process and save each ROOT file
            processAndSave(path, outputFilePath);
        }
    }

    std::cout << "All files have been processed." << std::endl;

    return 0;
}

