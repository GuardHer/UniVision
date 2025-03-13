#include <QString>
#include <QVector>
#include <QMap>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <random>
#include "Test.h"

using namespace std::chrono;

const int TEST_SIZE = 1000000;
const int MAP_SIZE = 100000;

// 测试QString和std::string拼接性能
void testStringPerf() {
    // QString测试
    auto start = high_resolution_clock::now();
    QString qstr;
    for (int i = 0; i < TEST_SIZE; ++i) {
        qstr.append("a");
    }
    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    std::cout << "QString append: " << duration.count() << " ms\n";

    // std::string测试
    start = high_resolution_clock::now();
    std::string str;
    for (int i = 0; i < TEST_SIZE; ++i) {
        str += "a";
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    std::cout << "std::string append: " << duration.count() << " ms\n\n";
}

// 测试向量插入性能
void testVectorPerf() {
    // QVector测试
    auto start = high_resolution_clock::now();
    QVector<int> qvec;
	qvec.reserve(TEST_SIZE);
    for (int i = 0; i < TEST_SIZE; ++i) {
        qvec.append(i);
    }
    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    std::cout << "QVector append: " << duration.count() << " ms\n";

    // std::vector测试
    start = high_resolution_clock::now();
    std::vector<int> vec;
    vec.reserve(TEST_SIZE);

    for (int i = 0; i < TEST_SIZE; ++i) {
        vec.push_back(i);
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    std::cout << "std::vector push_back: " << duration.count() << " ms\n\n";
}

// 测试关联容器性能
void testMapPerf() {
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(0, MAP_SIZE * 10);

    // QMap插入测试
    auto start = high_resolution_clock::now();
    QMap<int, int> qmap;
    for (int i = 0; i < MAP_SIZE; ++i) {
        qmap.insert(dis(gen), i);
    }
    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    std::cout << "QMap insert: " << duration.count() << " ms\n";

    // std::map插入测试
    start = high_resolution_clock::now();
    std::map<int, int> stdmap;
    for (int i = 0; i < MAP_SIZE; ++i) {
        stdmap[dis(gen)] = i;
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    std::cout << "std::map insert: " << duration.count() << " ms\n";

    // std::unordered_map插入测试
    start = high_resolution_clock::now();
    std::unordered_map<int, int> umap;
    for (int i = 0; i < MAP_SIZE; ++i) {
        umap[dis(gen)] = i;
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    std::cout << "std::unordered_map insert: " << duration.count() << " ms\n\n";

    // 查找性能测试
    const int search_times = 100000;
    std::vector<int> keys;
    for (int i = 0; i < search_times; ++i) {
        keys.push_back(dis(gen));
    }

    // QMap查找
    start = high_resolution_clock::now();
    for (int key : keys) {
        volatile auto val = qmap.value(key);
        (void)val;
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    std::cout << "QMap lookup: " << duration.count() << " ms\n";

    // std::map查找
    start = high_resolution_clock::now();
    for (int key : keys) {
        volatile auto it = stdmap.find(key);
        (void)it;
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    std::cout << "std::map lookup: " << duration.count() << " ms\n";

    // std::unordered_map查找
    start = high_resolution_clock::now();
    for (int key : keys) {
        volatile auto it = umap.find(key);
        (void)it;
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    std::cout << "std::unordered_map lookup: " << duration.count() << " ms\n\n";
}

int TestAll() {
    std::cout << "=== String Performance Test ===\n";
    testStringPerf();

    std::cout << "=== Vector Performance Test ===\n";
    testVectorPerf();

    std::cout << "=== Map Performance Test ===\n";
    testMapPerf();

    return 0;
}


