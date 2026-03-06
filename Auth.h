#ifndef AUTH_H
#define AUTH_H

#include <iostream>
#include <fstream>
#include <string>
#include "Player.h"

using namespace std;

class Auth {
public:
    // 1. ระบบสมัครสมาชิก
    static bool RegisterUser(string username, string password) {
        ifstream inFile("users.txt");
        string u, p;
        double c; int w, l;
        
        // เช็คว่ามีชื่อนี้ในระบบหรือยัง
        while (inFile >> u >> p >> c >> w >> l) {
            if (u == username) return false; 
        }
        inFile.close();

        // บันทึกผู้ใช้ใหม่ (เงินเริ่มต้น 1000)
        ofstream outFile("users.txt", ios::app);
        outFile << username << " " << password << " 1000.0 0 0\n";
        outFile.close();
        return true;
    }

    // 2. ระบบเข้าสู่ระบบ
    static bool LoginUser(string username, string password, Player &player) {
        ifstream inFile("users.txt");
        string u, p;
        double c; int w, l;
        
        // ค้นหาชื่อและรหัสผ่าน
        while (inFile >> u >> p >> c >> w >> l) {
            if (u == username && p == password) {
                player.name = u;
                player.credit = c;
                player.win_count = w;
                player.loss_count = l;
                return true;
            }
        }
        return false;
    }

    // 3. ระบบเซฟข้อมูล (ตอนปิดเกม)
    static void SaveUser(Player &player, string password) {
        if (player.name == "Guest Player") return; // ไม่เซฟไอดีชั่วคราว
        
        ifstream inFile("users.txt");
        ofstream outFile("temp.txt");
        string u, p;
        double c; int w, l;

        // อัปเดตข้อมูลของคนที่กำลังเล่นอยู่
        while (inFile >> u >> p >> c >> w >> l) {
            if (u == player.name) {
                outFile << player.name << " " << password << " " << player.credit << " " << player.win_count << " " << player.loss_count << "\n";
            } else {
                outFile << u << " " << p << " " << c << " " << w << " " << l << "\n";
            }
        }
        inFile.close();
        outFile.close();
        
        // ลบไฟล์เก่าแล้วเอาไฟล์ใหม่ไปแทนที่
        remove("users.txt");
        rename("temp.txt", "users.txt");
    }
};

#endif