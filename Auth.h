#ifndef AUTH_H
#define AUTH_H

#include <iostream>
#include <fstream>
#include <string>
#include "Player.h"

using namespace std;

class Auth {
public:
    // register
    static bool RegisterUser(string username, string password) {
        ifstream inFile("users.txt");
        string u, p; //username , password 
        double c; int w, l; //money , win, loss
        
        // เช็คว่ามีชื่อนี้ในระบบหรือยัง (อ่านข้อมูลทีละบรรทัดจนกว่าจะหมดไฟล์)
        while (inFile >> u >> p >> c >> w >> l) {
            if (u == username) return false; // ถ้าเจอชื่อซ้ำ ให้เด้งออกและส่งค่า false
        }
        inFile.close();

        // ios::app ให้เขียนต่อท้ายไฟล์เดิม
        ofstream outFile("users.txt", ios::app);
        outFile << username << " " << password << " 1000.0 0 0\n";
        outFile.close();
        return true; //สมัครสำเร็จ
    }

    // login
    static bool LoginUser(string username, string password, Player &player) {
        ifstream inFile("users.txt");
        string u, p;
        double c; int w, l;
        
        // ค้นหาชื่อและรหัสผ่าน
        while (inFile >> u >> p >> c >> w >> l) {
            if (u == username && p == password) {
                // โอนข้อมูลจากไฟล์ .txt เข้าไปเก็บในตัวแปร player
                player.name = u;
                player.credit = c;
                player.win_count = w;
                player.loss_count = l;
                return true; // ล็อกอินสำเร็จ
            }
        }
        return false; // ไม่มีข้อมูลหรือรหัสผิด
    }

    // ระบบเซฟข้อมูล (ตอนปิดเกม)
    static void SaveUser(Player &player, string password) {
        if (player.name == "Guest Player") return; //ถ้าเป็น Guest ไม่ต้องเซฟให้เปลืองพื้นที่
        
        ifstream inFile("users.txt");   // เปิดไฟล์เก่าเพื่ออ่าน
        ofstream outFile("temp.txt");   // สร้างไฟล์ชั่วคราว เพื่อเตรียมเขียนข้อมูลใหม่
        string u, p;
        double c; int w, l;

        // อัปเดตข้อมูลของคนที่กำลังเล่นอยู่
        while (inFile >> u >> p >> c >> w >> l) {
                //เขียนทับข้อมููลเก่า
            if (u == player.name) {
                outFile << player.name << " " << password << " " << player.credit << " " << player.win_count << " " << player.loss_count << "\n";
            } else {
                // ถ้าเป็นชื่อคนอื่น -> ให้ก๊อปปี้ข้อมูลเดิมเขียนลงไฟล์ temp ไปเลย
                outFile << u << " " << p << " " << c << " " << w << " " << l << "\n";
            }
        }
        inFile.close();
        outFile.close();
        
        remove("users.txt");               // ลบไฟล์เก่าทิ้ง
        rename("temp.txt", "users.txt");   // เปลี่ยนชื่อไฟล์ temp -> users.txt แทน
    }
};

#endif