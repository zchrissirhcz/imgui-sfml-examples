#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

std::unordered_map<std::string, int> readLabelsFromFile(const std::string& filename)
{
    std::unordered_map<std::string, int> labels;
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "failed to open file " << filename << std::endl;
        return labels;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string imageName;
        int imageLabel;

        if (!(iss >> imageName >> imageLabel))
        {
            std::cerr << "Failed to parse line: " << line << std::endl;
        }

        labels[imageName] = imageLabel;
    }

    file.close();
    return labels;
}

int main()
{
    constexpr int win_width = 960;
    constexpr int win_height = 640;
    sf::RenderWindow window(sf::VideoMode(win_width, win_height), "mnist viewer");
    window.setFramerateLimit(60);
    bool ret = ImGui::SFML::Init(window); // [imgui-SFML]
    if (!ret)
        return -1;

    sf::Texture texture;
    if (!texture.loadFromFile("../train/1.png"))
    {
        std::cerr << "Error loading image" << std::endl;
        return -1;
    }
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setScale(20, 20);
    sprite.setPosition(20, 20);

    // 加载字体
    //const std::string asset_dir = "../../games/Resources";
    const std::string asset_dir = "../../Resources";
    const std::string font_path = asset_dir + "/Arial.ttf";

    // 在这里添加字体
    ImFont* bigFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(font_path.c_str(), 30.0f);
    if (!bigFont)
    {
        std::cerr << "Failed to load font." << std::endl;
    }

    // 构建字体图集
    ImGui::GetIO().Fonts->Build();
    // 更新SFML中的字体纹理
    ret = ImGui::SFML::UpdateFontTexture();
    if (!ret)
        return -2;

    std::unordered_map<std::string, int> label_map = readLabelsFromFile("../train/train-label.txt");

    sf::Clock deltaClock;
    // 记录当前选中的文件名
    std::string currentFile;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(window, event); // [imgui-SFML]

            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart()); // [imgui-SFML]

        // 使用ImGui创建一个简单的列表
        ImGui::Begin("Image List");

        for (const auto& kv : label_map)
        {
            const std::string fileName = kv.first;

            // 如果列表项被点击
            if (ImGui::Selectable(fileName.c_str(), currentFile == fileName))
            {
                currentFile = fileName; // 更新当前选中的文件名

                // 加载对应的纹理
                const std::string imagePath = "../train/" + currentFile;
                if (!texture.loadFromFile(imagePath))
                {
                    std::cerr << "Failed to load " << imagePath << std::endl;
                }
                else
                {
                    sprite.setTexture(texture);
                }
            }
        }

        ImGui::End();

        // 显示当前图像的标签
        if (!currentFile.empty())
        {
            int currentLabel = label_map[currentFile];
            ImGui::Begin("Label", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::PushFont(bigFont); // 使用更大的字体
            //ImGui::Text("File: %s", currentFile.c_str());
            ImGui::Text("    %d    ", currentLabel);
            ImGui::PopFont(); // 恢复默认字体
            ImGui::End();
        }

        window.clear();
        window.draw(sprite);
        ImGui::SFML::Render(window); // [imgui-SFML]
        window.display();
    }

    ImGui::SFML::Shutdown(); // [imgui-SFML]

    return 0;
}
