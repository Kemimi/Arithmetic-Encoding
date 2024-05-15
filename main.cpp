
//
//  main.cpp
//  arithmetic_encodec
//
//  Created by KemingChen in 2022/06
//

#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <ctime>
#include <stack>

using namespace std;

class Arithmetic
{
public:
    Arithmetic() {
        length = 0;
    }
    ~Arithmetic() = default;

    void getProbabilities(char* str);
    double encode(string str);
    void runArithmetic();
    void dec2bin(double n);
    string decode(double value);


private:
    class Range
    {
    public:
        Range() {
            low = 0.0;
            high = 1.0;
            deltaLevel = 1.0;
        }
        ~Range() = default;
        double getLow() {
            return this->low;
        }

        double getHigh() {
            return this->high;
        }

        void setLow(double low) {
            this->low = low;
        }

        void setHigh(double high) {
            this->high = high;
        }

        void setDelta(double delta) {
            this->deltaLevel = delta;
        }
    private:
        double low;
        double high;
        double deltaLevel;

    };

    int binLength = 0;
    int length = 0;
    double code;
    std::map<char, Range> map;
};


void Arithmetic::runArithmetic() {
    cout << "Please enter a string:";
    long beginT = clock();
    char str[1024];
    cin >> str;
    getProbabilities(str);
    long endT = clock();
    long costT = endT - beginT;

    double num = encode(str);

    cout << "Resulting code: " << num << endl;
    cout << "Binary code: ";
    dec2bin(num);
    cout << endl;
    cout << "Decode: " << decode(num) << endl;
    cout << "Compression rate: " << double(binLength) / double(strlen(str) * 8) << endl;
    cout << endl << "Time costs: " << costT << " ms" << endl;
    cin.clear();
    cin.sync();
}


//计算信源str的出现概率，划分区间
void Arithmetic::getProbabilities(char* str) {
    char ch;
    double low = 0.0;
    double high = 0.0;
    double probability;
    std::map <char, int> freq;

    for (int i = 0; str[i]; i++)
        freq[str[i]]++;

    double len = strlen(str);
    cout << "The probability of each char: " << endl;
    for (auto p = freq.begin(); p != freq.end(); p++) {
        ch = (char)p->first;
        probability = (double)p->second / (double)len;
        cout << ch << ": " << probability << endl;

        low = high;
        high = low + probability;
        Range range;
        range.setLow(low);
        range.setHigh(high);
        range.setDelta(probability);
        map.insert(std::pair<char, Range>(ch, range));
    }
}


//对str进行编码
double Arithmetic::encode(string str) {
    double lowRange = 0.0, highRange = 1.0;
    for (char& i : str) {
        double delta = highRange - lowRange;
        highRange = lowRange + delta * map[i].getHigh();
        lowRange = lowRange + delta * map[i].getLow();
        ++length;
    }
    return lowRange;
}

//转换为二进制
void Arithmetic::dec2bin(double n) {
    stack<char> s;
    int m = (int)n;                   //取整数部分
    double t = n - m;                  //取小数部分
    binLength = 0;
    while (m)                          // 处理整数
    {
        s.push(m % 2);
        m /= 2;
    }
    while (!s.empty()) {
        printf("%d", s.top());

        s.pop();
    }

    while (t - int(t) != 0)                //处理小数点后的位数，乘2取整法 ，当乘2变为整数后结束
    {
        int temp = int(t * 2);
        printf("%d", temp);
        binLength++;
        t = 2 * t - int(2 * t);
    }
    //由于这里假设译码时已知信源序列的长度，故不用截取码长
    //溢出问题？

}

//译码
string  Arithmetic::decode(double value) {
    double lowInterval = 0.0;
    double highInterval = 0.0;
    string symbol = "";

    // 通过前面的hash表，译出第一个编码
    for (auto i = map.begin(); i != map.end(); i++) {
        if ((i->second).getLow() <= value && (i->second).getHigh() > value) {
            lowInterval = (i->second).getLow();
            highInterval = (i->second).getHigh();
            symbol += (i->first);
            break;
        }
    }

    for (int i = 0; i < length - 1; i++) {
        /* decode code */
        double deltaInterval;
        deltaInterval = highInterval - lowInterval;
        value -= lowInterval;
        value /= deltaInterval;                                   //放缩到[0,1]区间上

        for (auto i = map.begin(); i != map.end(); i++) {
            if ((i->second).getLow() <= value && (i->second).getHigh() > value) {
                lowInterval = (i->second).getLow();
                highInterval = (i->second).getHigh();
                symbol += (i->first);
                break;
            }
        }
    }

    return symbol;
}

int main() {

    Arithmetic a;
    a.runArithmetic();
}