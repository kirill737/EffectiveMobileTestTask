#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
    
struct ListNode { // ListNode модифицировать нельзя
    ListNode* prev = nullptr; // указатель на предыдущий элемент или nullptr
    ListNode* next = nullptr;
    ListNode* rand = nullptr; // указатель на произвольный элемент данного списка, либо `nullptr`
    std::string data; // произвольные пользовательские данные
};


size_t getDelPos(const std::string& rawStr) {
    size_t lastDelPos = rawStr.rfind(";");
    return lastDelPos;
}

void serializeToText(ListNode* head, const std::string& filePath) {
    std::ofstream outputFile(filePath);
    if (!outputFile.is_open()) {
        std::cout << "Ошибка открытия файла\n";
        return;
    }

    std::vector<ListNode*> nodes;

    // Собираем все узлы в вектор
    for (ListNode* cur = head; cur != nullptr; cur = cur->next)
        nodes.push_back(cur);


    // Записываем данные
    int nodeAmount = nodes.size();
    int randIndex;
    
    for (int i = 0; i < nodeAmount; ++i) {
        randIndex = -1;

        if (nodes[i]->rand) {
            for (int j = 0; j < nodeAmount; ++j) {
                if (nodes[j] == nodes[i]->rand) {
                    randIndex = j;
                    break;
                }
            }
        }

        outputFile << nodes[i]->data << ";" << randIndex << "\n";
    }
}

ListNode* deserializeFromText(const std::string& filePath) {
    
    std::fstream inputFile("inlet.in");
    if (!inputFile.is_open()) {
        std::cout << "Ошибка открытия файла\n";
        return nullptr;
    }

    std::string line;
    
    ListNode* head = nullptr;
    ListNode* cur = nullptr;

    std::vector<ListNode*> nodes;
    std::vector<int> rands;

    while (std::getline(inputFile, line)) {
        size_t delPos = getDelPos(line);
        if (delPos == std::string::npos) {
            std::cout << "Некорректные данные\n";
            for(auto el : nodes) {
                delete el;
            }
            return nullptr;
        }

        ListNode* newNode = new ListNode;
        newNode->data = line.substr(0, delPos);

        if (!head) {
            head = newNode;
            cur = newNode;
        } else {
            cur->next = newNode;
            newNode->prev = cur;
            cur = newNode;
        }

        nodes.push_back(newNode);
        rands.push_back(std::stoi(line.substr(delPos + 1)));
    }

    // Заполнение rand для всех нод
    for (int i = 0; i < nodes.size(); ++i) {
        if (rands[i] != -1)
            nodes[i]->rand = nodes[rands[i]];
    }
    return head;
}

void serializeToBin(ListNode* head, const std::string& filePath) {
    std::ofstream outputFile(filePath, std::ios::binary);

    std::vector<ListNode*> nodes;
    
    for (ListNode* cur = head; cur != nullptr; cur = cur->next)
        nodes.push_back(cur);

    int nodeAmount = nodes.size();
    outputFile.write(reinterpret_cast<char*>(&nodeAmount), sizeof(nodeAmount));

    for (int i = 0; i < nodeAmount; ++i) {
        int len = nodes[i]->data.size();
        outputFile.write(reinterpret_cast<char*>(&len), sizeof(len));

        outputFile.write(nodes[i]->data.c_str(), len);

        // Заполнение rand для всех нод
        int randIndex = -1;
        if (nodes[i]->rand) {
            for (int j = 0; j < nodeAmount; ++j)
                if (nodes[j] == nodes[i]->rand)
                    randIndex = j;
        }

        outputFile.write(reinterpret_cast<char*>(&randIndex), sizeof(randIndex));
    }
}

ListNode* deserializeFromBin(const std::string& filePath) {
    std::ifstream inputFile(filePath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cout << "Ошибка открытия файла\n";
        return nullptr;
    }

    // Чтение количества нод
    int nodeAmount = 0;
    inputFile.read(reinterpret_cast<char*>(&nodeAmount), sizeof(nodeAmount));
    if (nodeAmount <= 0)
        return nullptr;

    std::vector<ListNode*> nodes;
    std::vector<int> rands;

    ListNode* head = nullptr;
    ListNode* cur = nullptr;

    // Чтение данных всех нод
    for (int i = 0; i < nodeAmount; ++i) {
        int len = 0;
        inputFile.read(reinterpret_cast<char*>(&len), sizeof(len));
        if (len < 0) {
            for (auto node : nodes) delete node;
            return nullptr;
        }

        std::string data(len, '\0');
        inputFile.read(&data[0], len);

        int randIndex = -1;
        inputFile.read(reinterpret_cast<char*>(&randIndex), sizeof(randIndex));

        // Создаём узел
        ListNode* newNode = new ListNode;
        newNode->data = data;

        // Запись данных о prev, next в ноды
        if (!head) {
            head = newNode;
            cur = newNode;
        } else {
            cur->next = newNode;
            newNode->prev = cur;
            cur = newNode;
        }

        nodes.push_back(newNode);
        rands.push_back(randIndex);
    }

    // Заполнение rand для всех нод
    for (int i = 0; i < nodes.size(); ++i) {
        if (rands[i] >= 0 && rands[i] < nodes.size())
            nodes[i]->rand = nodes[rands[i]];
        else
            nodes[i]->rand = nullptr;
    }

    return head;
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    ListNode* head = deserializeFromText("inlet.in");
    serializeToBin(head, "outlet.out");

    head = deserializeFromBin("outlet.out");
    serializeToText(head, "outlet.in");

    ListNode* cur = head;
    while (cur != nullptr) {
        if (cur->rand)
            std::cout << cur->data << " -> " << cur->rand->data << std::endl;
        else
            std::cout << cur->data << " -> nullptr" << std::endl;

        cur = cur->next;
    }

    return 0;
}