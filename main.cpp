#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include <stack>

using namespace std;

union {
    unsigned long long ull;
    int i;
} uli;

union {
    unsigned long long ull;
    long long ll;
}lul;

union {
    unsigned long long ull;
    double d;
} uld;

int main() {
    //ассемблирование
    map<string, unsigned int> key;
    key["halt"] = 0;
    key["syscall"] = 1;
    key["add"] = 2;
    key["addi"] = 3;
    key["sub"] = 4;
    key["subi"] = 5;
    key["mul"] = 6;
    key["muli"] = 7;
    key["div"] = 8;
    key["divi"] = 9;
    key["lc"] = 12;
    key["shl"] = 13;
    key["shli"] = 14;
    key["shr"] = 15;
    key["shri"] = 16;
    key["and"] = 17;
    key["andi"] = 18;
    key["or"] = 19;
    key["ori"] = 20;
    key["xor"] = 21;
    key["xori"] = 22;
    key["not"] = 23;
    key["mov"] = 24;
    key["addd"] = 32;
    key["subd"] = 33;
    key["muld"] = 34;
    key["divd"] = 35;
    key["itod"] = 36;
    key["dtoi"] = 37;
    key["push"] = 38;
    key["pop"] = 39;
    key["call"] = 40;
    key["calli"] = 41;
    key["ret"] = 42;
    key["cmp"] = 43;
    key["cmpi"] = 44;
    key["cmpd"] = 45;
    key["jmp"] = 46;
    key["jne"] = 47;
    key["jeq"] = 48;
    key["jle"] = 49;
    key["jl"] = 50;
    key["jge"] = 51;
    key["jg"] = 52;
    key["load"] = 64;
    key["store"] = 65;
    key["load2"] = 66;
    key["store2"] = 67;
    key["loadr"] = 68;
    key["storer"] = 69;
    key["loadr2"] = 70;
    key["storer2"] = 71;

    int u = 1;

    int flags = 0;
    int count = 0; // для заполнения mem
    vector <unsigned int> mem(1024 * 1024 );
    map<string, int> func;
    vector<int> regs(16); // регистр
    map<int, string> dop; // которые надо дополнить функциями
    vector<int> rr {2, 4, 6, 8, 13, 15, 17, 19, 21, 24, 32, 33, 34, 35, 36, 37, 40, 43, 45, 68, 69, 70, 71}; // rr тип
    vector<int> ri {0, 1, 3, 5, 7, 9, 12, 14, 16, 18, 20, 22, 23, 38, 39, 44}; // ri тип
    int start = 0, endp = -1;
    //int rst = -2, pr = 0; // для jmp

    regs[15] = 0;
    regs[14] = 1048575;

    // proc/sys/kernel/core_pattern/
    ifstream fin("input.fasm");
    string s;

    FILE *out;

    out = fopen("output.txt", "w");

    //1 проход - считаю функции и то что можно посчитать
    //2 проход - дополню недостающее
    while (getline(fin, s)){
        vector<string> words;
        string str = "";
        for (int i = 0; i < s.length(); i ++){
            if (s[i] == ' ' or s[i] == ',' or s[i] == ':'){
                if (str.length() != 0) {
                    words.push_back(str);
                }
                str = "";
            } else {
                str += s[i];
            }
        }
        words.push_back(str);
        /*for(auto v : words){
            cout << v << " ";
        }
        cout<<'\n';*/
        if (str.length() != 0){
            words.push_back(str);
        }
        if (words[0] == "end"){
            start = func[words[1]];
            endp = count;
            continue;
        }
        //создал набор слов в каждой строке
        //пусть теперь встретилось слово, которого нет в строке
        //проверим words[0]
        if (key.count(words[0]) == 0){
            func[words[0]] = count;
            continue;
        }
        int j = key[words[0]];
        mem[count] = j << 24;
        if (j == 41 or (j >= 46 and j <= 52)){ // j тип
            dop[count] = words[1];
        }
        if (j == 42){ // ret
            int a = stoi(words[1]);
            mem[count] |= abs(a);
            if (a < 0){
                mem[count] |= (1 << 19);
            }
        }
        if (j >= 64 and j <= 67){ // rm тип
            mem[count] |= int(stoi(words[1]));
        }
        if (find(rr.begin(), rr.end(), j) != rr.end()){ // rr тип
            int a1, a2;
            if ((words[1].length() == 2)){
                a1 = words[1][1] - '0';
            } else {
                a1 = (words[1][1] - '0') * 10 + words[1][2] - '0';
            }
            if ((words[2].length() == 2)){
                a2 = words[2][1] - '0';
            } else {
                a2 = (words[2][1] - '0') * 10 + words[2][2] - '0';
            }
            mem[count] |= (a1 << 20);
            mem[count] |= (a2 << 16);
            mem[count] |= (abs(stoi(words[3])));
            if (stoi(words[3]) < 0){
                mem[count] |= (1 << 15);
            }
        }
        if (find(ri.begin(), ri.end(), j) != ri.end()) {
            int a;
            if ((words[1].length() == 2)) {
                a = words[1][1] - '0';
            } else {
                a = (words[1][1] - '0') * 10 + words[1][2] - '0';
            }
            mem[count] |= (a << 20);
            if (isdigit(words[2][0])) {
                mem[count] |= (abs(stoi(words[2])));
                if (stoi(words[2]) < 0) {
                    mem[count] |= (1 << 19);
                }
            } else {
                dop[count] = words[2];
            }
        }
        count ++;
    }

    map<int, string> :: iterator it = dop.begin();

//    for (auto i : dop){
//        mem[i.first] |= func[i.second];
//    }

    for (int i = 0; it != dop.end(); i++, it++){
        mem[it -> first] |= func[it -> second];
    }

    /*for (int i = 0; i <= 17; i++){
        cout << mem[i]  << "\n";
    }*/
    fin.close();
    //cout << start << " " << endp << "\n";

    //исполнение
    unsigned int code = 0; // маска для кода команды
    for (int i = 31; i >= 24; --i){
        code |= (1 << i);
    }

    unsigned int r1 = 0; // маска для 1 регистра rr и ri
    unsigned int r2 = 0; // маска для 2 регистра rr
    unsigned int zn = 0; // маска для знакак числа rr
    unsigned int num = 0; // маска для числа rr
    unsigned int zni = 0; //маска для знака числа ri
    unsigned int numi = 0; // маска для числа ri
    unsigned int nj = (1 << 21) - 1; // маска для числа j

    zni = (1 << 19);

    numi = (1 << 19) - 1;

    for (int i = 23; i >= 20; i--){
        r1 |= (1 << i);
    }

    for (int i = 19; i >= 16; i--){
        r2 |= (1 << i);
    }

    zn = (1 << 15);

    num = (1 << 15) - 1;

    regs[15] = start;

    while  (true){ // основная программа
        //cout << "str = " << regs[15] << " | ";
//        if (rst != -2){
//            if (pr == 1){
//                regs[15] = rst;
//                rst = -1;
//                pr = 0;
//            } else {
//                pr ++;
//            }
//        }
        unsigned int com = mem[regs[15]];
        if (regs[15] == endp){
            break;
        }

        unsigned int j = (com & code) >> 24; //код команды

        if (j == 12){ // lc
            int reg = (com & r1) >> 20;
            int ch = (com & numi);
            if ((com & zni) != 0) {
                ch = -ch;
            }
            regs[reg] = ch;
        }

        if (j == 2) { // add
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            int ch = (com & num);
            if ((com & zn) != 0){
                ch = -ch;
            }
            regs[reg1] += regs[reg2] + ch;
        }

        if (j == 3) { //addi
            int reg = (com & r1) >> 20;
            int ch = (com & num);
            if ((com & zni) != 0){
                ch = -ch;
            }
            regs[reg] += ch;
        }

        if (j == 4) { //sub
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            int ch = (com & num);
            if ((com & zn) != 0){
                ch = -ch;
            }
            regs[reg1] -= (regs[reg2] + ch);
        }

        if (j == 5) { //subi
            int reg = (com & r1) >> 20;
            int ch = (com & numi);
            if ((com & zni) != 0){
                ch = -ch;
            }
            regs[reg] -= ch;
        }

        if (j == 13) { // shl - битовый сдвиг
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            regs[reg1] <<= regs[reg2];
        }

        if (j == 14) { //shli - битовый сдвиг
            int reg = (com & r1) >> 20;
            int ch = (com & numi);
            regs[reg] <<= ch;
        }

        if (j == 15){ // shr - битовый сдвиг
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            regs[reg1] >>= regs[reg2];
        }

        if (j == 16) { // shri
            int reg = (com & r1) >> 20;
            int ch = (com & numi);
            regs[reg] >>= ch;
        }

        if (j == 17) { // and
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            regs[reg1] = regs[reg1] & regs[reg2];
        }

        if (j == 18) { //andi
            int reg = (com & r1) >> 20;
            int ch = (com & numi);
            regs[reg] &= ch;
        }

        if (j == 19) { // or
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            regs[reg1] |= regs[reg2];
        }

        if (j == 20) { // ori
            int reg = (com & r1) >> 20;
            int ch = (com & numi);
            regs[reg] |= ch;
        }

        if (j == 21) { // xor
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            regs[reg1] ^= regs[reg2];
        }

        if (j == 22) { // xori
            int reg = (com & r1) >> 20;
            int ch = (com & numi);
            regs[reg] ^= ch;
        }

        if (j == 1) { //syscall
            int reg = (com & r1) >> 20;
            int ch = (com & num);
            if ((com & zni) != 0){
                ch = -ch;
            }

            if (ch == 100) { //scanint
                int s;
                cin >> s;
                regs[reg] = s;
            }

            if (ch == 102) { //printint
                fprintf(out, "%d", regs[reg]);
            }

            if (ch == 101) { //scandouble
                double s;
                cin >> s;
                uld.d = s;
                uli.ull = uld.ull;
                regs[reg] = uli.i;
                regs[reg + 1] = uli.ull >> 32;
            }

            if (ch == 103) { //printdouble
                unsigned long long s = (((unsigned long long)regs[reg + 1]) << 32) + (unsigned int)regs[reg];
                uld.ull = s;
                double ans = uld.d;
                fprintf(out ,"%lg", ans);
            }

            if (ch == 105) {
                fprintf(out ,"%c", char(regs[reg]));
            }

            if (ch == 0){
                fclose(out);
                return 0;
            }
        }

        if (j == 6) { // mul
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            int ch = (com & num);
            if ((com & zn) != 0){
                ch = -ch;
            }
            long long ans = (long long)regs[reg1] * (long long)(regs[reg2] + ch);
            lul.ll = ans;
            uli.ull = lul.ull;
            regs[reg1] = uli.i;
            regs[reg1 + 1] = uli.ull >> 32;
        }

        if (j == 7) { //mulli
            int reg = (com & r1) >> 20;
            int ch = (com & num);
            if ((com & zni) != 0){
                ch = -ch;
            }
            long long ans = (long long)regs[reg] * (long long)ch;
            lul.ll = ans;
            uli.ull = lul.ull;
            regs[reg] = uli.i;
            regs[reg + 1] = uli.ull >> 32;
        }

        if (j == 8) { //div
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            int ch = (com & num);
            if ((com & zn) != 0){
                ch = -ch;
            }
            long long ans1 = 0;
            if (regs[reg1 + 1] != 0) {
                ans1 =
                        (((unsigned long long) regs[reg1 + 1]) << 32) + (unsigned int) regs[reg1];
            } else {
                ans1 = regs[reg1];
            }
            lul.ull = ans1;
            long long ans = lul.ll;
            regs[reg1] = ans / regs[reg2];
            regs[reg1 + 1] = ans % regs[reg2];
        }

        if (j == 9) { //divi
            int reg = (com & r1) >> 20;
            int ch = (com & num);
            if ((com & zni) != 0){
                ch = -ch;
            }
            long long ans1 = 0;
            if (regs[reg + 1] != 0) {
                ans1 =
                        (((unsigned long long) regs[reg + 1]) << 32) + (unsigned int) regs[reg];
            } else {
                ans1 = regs[reg];
            }
            lul.ull = ans1;
            long long ans = lul.ll;
            regs[reg] = ans / ch;
            regs[reg + 1] = ans % ch;
        }

        if (j == 24) { //mov
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            int ch = (com & num);
            if ((com & zn) != 0){
                ch = -ch;
            }
            regs[reg1] = regs[reg2] + ch;
        }

        if (j == 32) { //addd
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            int ch = (com & num);
            if ((com & zn) != 0){
                ch = -ch;
            }
            unsigned long long ll1 = (((unsigned long long)regs[reg1 + 1]) << 32) + (unsigned int)regs[reg1];
            unsigned long long ll2 = (((unsigned long long)regs[reg2 + 1]) << 32) + (unsigned int)regs[reg2];
            uld.ull = ll1;
            double a1 = uld.d;
            uld.ull = ll2;
            double a2 = uld.d;
            double ans = a1 + a2;
            uld.d = ans;
            uli.ull = uld.ull;
            regs[reg1] = uli.i;
            regs[reg1 + 1] = uli.ull >> 32;
        }

        if (j == 33) { //subd
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            int ch = (com & num);
            if ((com & zn) != 0){
                ch = -ch;
            }
            unsigned long long ll1 = (((unsigned long long)regs[reg1 + 1]) << 32) + (unsigned int)regs[reg1];
            unsigned long long ll2 = (((unsigned long long)regs[reg2 + 1]) << 32) + (unsigned int)regs[reg2];
            uld.ull = ll1;
            double a1 = uld.d;
            uld.ull = ll2;
            double a2 = uld.d;
            double ans = a1 - a2;
            uld.d = ans;
            uli.ull = uld.ull;
            regs[reg1] = uli.i;
            regs[reg1 + 1] = uli.ull >> 32;
        }

        if (j == 34) { // muld
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            int ch = (com & num);
            if ((com & zn) != 0){
                ch = -ch;
            }
            unsigned long long ll1 = (((unsigned long long)regs[reg1 + 1]) << 32) + (unsigned int)regs[reg1];
            unsigned long long ll2 = (((unsigned long long)regs[reg2 + 1]) << 32) + (unsigned int)regs[reg2];
            uld.ull = ll1;
            double a1 = uld.d;
            uld.ull = ll2;
            double a2 = uld.d;
            double ans = a1 * a2;
            uld.d = ans;
            uli.ull = uld.ull;
            regs[reg1] = uli.i;
            regs[reg1 + 1] = uli.ull >> 32;
        }

        if (j == 35) { //divd
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            int ch = (com & num);
            if ((com & zn) != 0){
                ch = -ch;
            }
            unsigned long long ll1 = (((unsigned long long)regs[reg1 + 1]) << 32) + (unsigned int)regs[reg1];
            unsigned long long ll2 = (((unsigned long long)regs[reg2 + 1]) << 32) + (unsigned int)regs[reg2];
            uld.ull = ll1;
            double a1 = uld.d;
            uld.ull = ll2;
            double a2 = uld.d;
            double ans = a1 / a2;
            uld.d = ans;
            uli.ull = uld.ull;
            regs[reg1] = uli.i;
            regs[reg1 + 1] = uli.ull >> 32;
        }

        if (j == 36) { // itod
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            int ch = (com & num);
            if ((com & zn) != 0){
                ch = -ch;
            }
            double s = double (regs[reg2]);
            uld.d = s;
            uli.ull = uld.ull;
            regs[reg1] = uli.i;
            regs[reg1 + 1] = uli.ull >> 32;
        }

        if (j == 44) { //cmpi
            int reg = (com & r1) >> 20;
            int ch = (com & num);
            if ((com & zni) != 0){
                ch = -ch;
            }
            flags = 0;
            if (regs[reg] > ch){
                flags = 1;
            }
            if (regs[reg] < ch){
                flags = -1;
            }
        }

        if (j == 43) { //cmp
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            int ch = (com & num);
            if ((com & zn) != 0){
                ch = -ch;
            }
            flags = 0;
            if (regs[reg1] > regs[reg2]){
                flags = 1;
            }
            if (regs[reg1] < regs[reg2]){
                flags = -1;
            }
        }

        if (j == 45) { //cmpd
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            int ch = (com & num);
            if ((com & zn) != 0){
                ch = -ch;
            }
            unsigned long long ll1 = (((unsigned long long)regs[reg1 + 1]) << 32) + (unsigned int)regs[reg1];
            unsigned long long ll2 = (((unsigned long long)regs[reg2 + 1]) << 32) + (unsigned int)regs[reg2];
            uld.ull = ll1;
            double a1 = uld.d;
            uld.ull = ll2;
            double a2 = uld.d;
            flags = 0;
            if (a1 > a2){
                flags = 1;
            }
            if (a1 < a2){
                flags = -1;
            }
        }

        if (j == 40){ // call
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            int ch = (com & num);
            if ((com & zn) != 0){
                ch = -ch;
            }
            regs[14] --;
            mem[regs[14]] = regs[15];
            regs[15] = regs[reg2] + ch - 1;
            regs[reg1] = regs[reg2] + ch ;
        }

        if (j == 41){ // calli
            int r = (com & nj);
            regs[14] --;
            mem[regs[14]] = regs[15];
            regs[15] = r - 1;
        }

        if (j == 42) { // ret
            int ch = (com & num);
            if ((com & zni) != 0){
                ch = -ch;
            }
            regs[15] = mem[regs[14]];
            regs[14] ++;
            for (int i = 0; i < ch; i++){
                regs[14] ++;
            }
        }

        if (j == 68) { // loadr
            int reg1 = (com & r1) >> 20;
            int reg2 = (com & r2) >> 16;
            int ch = (com & num);
            if ((com & zn) != 0){
                ch = -ch;
            }
            regs[reg1] = mem[regs[reg2] + ch];
        }

        if (j == 38) { // push
            int reg = (com & r1) >> 20;
            int ch = (com & num);
            if ((com & zni) != 0){
                ch = -ch;
            }
            regs[14] --;
            mem[regs[14]] = regs[reg] + ch;
            //cout << "push(" << regs[reg] + ch << " " << reg << ") ";
        }

        if (j == 52){ //jg
            int r = (com & nj);
            if (flags == 1){
                regs[15] = r - 1;
            }
        }

        if (j == 39) { //pop
            int reg = (com & r1) >> 20;
            int ch = (com & num);
            if ((com & zni) != 0){
                ch = -ch;
            }
            regs[reg] = mem[regs[14]] + ch;
            regs[14] ++;
        }

        if (j == 46) { //jmp
            int r = (com & nj);
            regs[15] = r - 1;
        }

        if (j == 47) { //jne
            int r = (com & nj);
            if (flags != 0) {
                regs[15] = r - 1;
            }
        }

        if (j == 48) { //jeq
            int r = (com & nj);
            if (flags == 0) {
                regs[15] = r - 1;
            }
        }

        if (j == 49) { //jle
            int r = (com & nj);
            if (flags <= 0) {
                regs[15] = r - 1;
            }
        }

        if (j == 50) { //jl
            int r = (com & nj);
            if (flags == -1) {
                regs[15] = r - 1;
            }
        }

        if (j == 51) { //jge
            int r = (com & nj);
            if (flags >= 0) {
                regs[15] = r - 1;
            }
        }


        regs[15] ++;

    }

    fclose(out);
    return 0;
}
