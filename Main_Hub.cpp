#include <iostream>
#include <string>

// นำเข้าข้อมูลผู้เล่นและระบบจัดการไฟล์
#include "Player.h"
#include "Auth.h" 

// ป้องกันปัญหาขัดแย้งกับ Windows
#if defined(_WIN32)
    #define NOGDI             
    #define NOUSER            
#endif

#include "raylib.h"

// นำเข้าไฟล์ UI มินิเกม
#include "slot.h" 
#include "hilo.h" 
#include "blackjack.h" 
#include "bingo.h" 

#if defined(_WIN32)
    #undef Rectangle
    #undef CloseWindow
    #undef DrawText
    #undef ShowCursor
#endif

using namespace std;

// ฟังก์ชันสร้างสตริงรหัสผ่านที่ซ่อนเป็นเครื่องหมายดอกจัน
string getMaskedPassword(string pass) {
    return string(pass.length(), '*');
}

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;
    
    InitWindow(screenWidth, screenHeight, "SCAMMER CASINO CLUB - Ultimate Edition");
    SetTargetFPS(60);

    // --- โหลดรูปภาพ How To Play ---
    // เตรียมไฟล์รูปชื่อ howtoplay.png ไว้ในโฟลเดอร์เดียวกับโค้ดด้วยนะครับ!
    Texture2D htpImage = LoadTexture("howtoplay.png"); 

    // ตัวแปรระบบเกม
    Player p1("Guest Player", 1000.0, 0, 0);
    string currentPassword = "";
    
    // ตัวแปรจัดการหน้าจอ (0 = เมนูเลือก, 1 = ฟอร์มกรอกข้อมูล, 2 = คาสิโน Hub, 3 = หน้า How To Play)
    int screenState = 0; 
    bool isLoginMode = true; 

    // ตัวแปรฟอร์มกรอกข้อมูล
    string inputUser = "";
    string inputPass = "";
    int activeBox = 0; 
    string sysMsg = "";
    Color msgColor = RAYWHITE;

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        bool isClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        // ==========================================
        // STATE 0: หน้าต่างเลือกระบบ (Login / Register / Guest)
        // ==========================================
        if (screenState == 0) {
            Rectangle btnGoLogin  = { centerX - 150, 300, 300, 60 };
            Rectangle btnGoReg    = { centerX - 150, 380, 300, 60 };
            Rectangle btnGoGuest  = { centerX - 150, 460, 300, 60 };
            Rectangle btnExitGame = { centerX - 150, 540, 300, 60 }; 

            if (isClick) {
                if (CheckCollisionPointRec(mousePos, btnGoLogin)) { screenState = 1; isLoginMode = true; sysMsg = "Enter Credentials to Login"; msgColor = RAYWHITE; activeBox = 1; }
                if (CheckCollisionPointRec(mousePos, btnGoReg))   { screenState = 1; isLoginMode = false; sysMsg = "Create a New Account"; msgColor = SKYBLUE; activeBox = 1; }
                if (CheckCollisionPointRec(mousePos, btnGoGuest)) { screenState = 2; p1.name = "Guest Player"; p1.credit = 1000.0; p1.win_count = 0; p1.loss_count = 0; }
                if (CheckCollisionPointRec(mousePos, btnExitGame)) break; // ปิดเกม
            }

            BeginDrawing();
            ClearBackground(Color{ 15, 25, 50, 255 }); // สีน้ำเงินเข้ม VIP
            
            DrawText("SCAMMER CASINO CLUB", (screenWidth - MeasureText("SCAMMER CASINO CLUB", 55)) / 2, 100, 55, GOLD);
            DrawText("Welcome! Please identify yourself.", (screenWidth - MeasureText("Welcome! Please identify yourself.", 25)) / 2, 180, 25, RAYWHITE);

            // วาดกรอบพื้นหลังดำโปร่งแสง
            DrawRectangle(centerX - 190, 270, 380, 360, Color{0, 0, 0, 150});

            // ปุ่ม LOGIN
            Color cLogin = CheckCollisionPointRec(mousePos, btnGoLogin) ? LIGHTGRAY : RAYWHITE;
            DrawRectangleRec(btnGoLogin, cLogin);
            if (!CheckCollisionPointRec(mousePos, btnGoLogin)) DrawRectangle(btnGoLogin.x, btnGoLogin.y + btnGoLogin.height - 5, btnGoLogin.width, 5, Color{0,0,0,100});
            DrawRectangleLinesEx(btnGoLogin, 3, BLACK);
            DrawText("LOGIN", btnGoLogin.x + (btnGoLogin.width - MeasureText("LOGIN", 25))/2, btnGoLogin.y + 18, 25, BLACK);

            // ปุ่ม REGISTER
            Color cReg = CheckCollisionPointRec(mousePos, btnGoReg) ? LIGHTGRAY : RAYWHITE;
            DrawRectangleRec(btnGoReg, cReg);
            if (!CheckCollisionPointRec(mousePos, btnGoReg)) DrawRectangle(btnGoReg.x, btnGoReg.y + btnGoReg.height - 5, btnGoReg.width, 5, Color{0,0,0,100});
            DrawRectangleLinesEx(btnGoReg, 3, BLACK);
            DrawText("REGISTER", btnGoReg.x + (btnGoReg.width - MeasureText("REGISTER", 25))/2, btnGoReg.y + 18, 25, BLACK);

            // ปุ่ม GUEST
            Color cGuest = CheckCollisionPointRec(mousePos, btnGoGuest) ? LIGHTGRAY : RAYWHITE;
            DrawRectangleRec(btnGoGuest, cGuest);
            if (!CheckCollisionPointRec(mousePos, btnGoGuest)) DrawRectangle(btnGoGuest.x, btnGoGuest.y + btnGoGuest.height - 5, btnGoGuest.width, 5, Color{0,0,0,100});
            DrawRectangleLinesEx(btnGoGuest, 3, BLACK);
            DrawText("PLAY AS GUEST", btnGoGuest.x + (btnGoGuest.width - MeasureText("PLAY AS GUEST", 25))/2, btnGoGuest.y + 18, 25, BLACK);

            // ปุ่ม EXIT
            Color cExit = CheckCollisionPointRec(mousePos, btnExitGame) ? MAROON : RED;
            DrawRectangleRec(btnExitGame, cExit);
            if (!CheckCollisionPointRec(mousePos, btnExitGame)) DrawRectangle(btnExitGame.x, btnExitGame.y + btnExitGame.height - 5, btnExitGame.width, 5, Color{0,0,0,100});
            DrawRectangleLinesEx(btnExitGame, 3, GOLD);
            DrawText("EXIT GAME", btnExitGame.x + (btnExitGame.width - MeasureText("EXIT GAME", 25))/2, btnExitGame.y + 18, 25, WHITE);

            EndDrawing();
        }
        // ==========================================
        // STATE 1: ฟอร์มกรอกข้อมูล (Username / Password)
        // ==========================================
        else if (screenState == 1) {
            Rectangle boxUser   = { centerX - 150, 250, 300, 50 };
            Rectangle boxPass   = { centerX - 150, 350, 300, 50 };
            Rectangle btnSubmit = { centerX - 150, 450, 300, 60 };
            Rectangle btnBack   = { 30, 30, 100, 40 };

            if (isClick) {
                if (CheckCollisionPointRec(mousePos, boxUser)) activeBox = 1;
                else if (CheckCollisionPointRec(mousePos, boxPass)) activeBox = 2;
                else activeBox = 0;

                if (CheckCollisionPointRec(mousePos, btnBack)) { screenState = 0; inputUser = ""; inputPass = ""; }

                if (CheckCollisionPointRec(mousePos, btnSubmit)) {
                    if (inputUser.empty() || inputPass.empty()) {
                        sysMsg = "Fields cannot be empty!"; msgColor = RED;
                    } else {
                        if (isLoginMode) {
                            if (Auth::LoginUser(inputUser, inputPass, p1)) {
                                currentPassword = inputPass;
                                screenState = 2; // เข้าเกม
                            } else {
                                sysMsg = "Invalid Username or Password!"; msgColor = RED;
                            }
                        } else {
                            if (Auth::RegisterUser(inputUser, inputPass)) {
                                sysMsg = "Success! You can now Login."; msgColor = GREEN;
                                isLoginMode = true; 
                                inputPass = ""; 
                                activeBox = 2;
                            } else {
                                sysMsg = "Username already exists!"; msgColor = RED;
                            }
                        }
                    }
                }
            }

            if (activeBox > 0) {
                int key = GetCharPressed();
                while (key > 0) {
                    if ((key >= 32) && (key <= 125)) {
                        if (activeBox == 1 && inputUser.length() < 15) inputUser += (char)key;
                        if (activeBox == 2 && inputPass.length() < 15) inputPass += (char)key;
                    }
                    key = GetCharPressed(); 
                }

                if (IsKeyPressed(KEY_BACKSPACE)) {
                    if (activeBox == 1 && inputUser.length() > 0) inputUser.pop_back();
                    if (activeBox == 2 && inputPass.length() > 0) inputPass.pop_back();
                }
                if (IsKeyPressed(KEY_TAB)) activeBox = (activeBox == 1) ? 2 : 1;
            }

            BeginDrawing();
            ClearBackground(Color{ 15, 25, 50, 255 });

            DrawText(isLoginMode ? "ACCOUNT LOGIN" : "ACCOUNT REGISTER", (screenWidth - MeasureText(isLoginMode ? "ACCOUNT LOGIN" : "ACCOUNT REGISTER", 40)) / 2, 80, 40, GOLD);
            DrawText(sysMsg.c_str(), (screenWidth - MeasureText(sysMsg.c_str(), 20)) / 2, 150, 20, msgColor);

            DrawRectangle(centerX - 190, 210, 380, 340, Color{0, 0, 0, 150});

            DrawText("Username:", centerX - 150, 225, 20, RAYWHITE);
            DrawRectangleRec(boxUser, (activeBox == 1) ? RAYWHITE : LIGHTGRAY);
            DrawRectangleLinesEx(boxUser, 3, (activeBox == 1) ? GOLD : BLACK);
            string dUser = inputUser; if(activeBox == 1 && (int)(GetTime()*2)%2==0) dUser += "_";
            DrawText(dUser.c_str(), boxUser.x + 10, boxUser.y + 15, 20, BLACK);

            DrawText("Password:", centerX - 150, 325, 20, RAYWHITE);
            DrawRectangleRec(boxPass, (activeBox == 2) ? RAYWHITE : LIGHTGRAY);
            DrawRectangleLinesEx(boxPass, 3, (activeBox == 2) ? GOLD : BLACK);
            string dPass = getMaskedPassword(inputPass); if(activeBox == 2 && (int)(GetTime()*2)%2==0) dPass += "_";
            DrawText(dPass.c_str(), boxPass.x + 10, boxPass.y + 15, 20, BLACK); 

            Color cSub = CheckCollisionPointRec(mousePos, btnSubmit) ? MAROON : RED;
            DrawRectangleRec(btnSubmit, cSub);
            if (!CheckCollisionPointRec(mousePos, btnSubmit)) DrawRectangle(btnSubmit.x, btnSubmit.y + btnSubmit.height - 5, btnSubmit.width, 5, Color{0,0,0,100});
            DrawRectangleLinesEx(btnSubmit, 3, GOLD);
            const char* subTxt = isLoginMode ? "LOGIN" : "REGISTER";
            DrawText(subTxt, btnSubmit.x + (btnSubmit.width - MeasureText(subTxt, 25))/2, btnSubmit.y + 18, 25, GOLD);

            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON);
            DrawRectangleLinesEx(btnBack, 2, WHITE);
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);

            EndDrawing();
        }
        // ==========================================
        // STATE 2: หน้าคาสิโนหลัก (Main Hub)
        // ==========================================
        else if (screenState == 2) {
            // พิกัดปุ่มหน้าคาสิโน Hub
            Rectangle btnSlot      = { centerX - 170, 240, 340, 50 };
            Rectangle btnHilo      = { centerX - 170, 300, 340, 50 };
            Rectangle btnBlackjack = { centerX - 170, 360, 340, 50 };
            Rectangle btnBingo     = { centerX - 170, 420, 340, 50 };
            
            // เพิ่มปุ่ม HOW TO PLAY
            Rectangle btnHTP       = { centerX - 170, 490, 340, 50 }; 
            Rectangle btnLogout    = { centerX - 170, 560, 340, 50 }; 
            Rectangle btnBegMoney  = { centerX - 170, 400, 340, 60 }; // ปุ่มขอเงินตอนล้มละลาย

            // เช็คว่าเงินหมดหรือยัง (น้อยกว่า 1 แปลว่าแทงไม่ได้แล้ว)
            bool isBankrupt = (p1.credit < 1.0f);

            if (isClick) {
                if (isBankrupt) {
                    // ถังแตก กดได้แค่ปุ่มขอเงิน, วิธีเล่น หรือ ออกจากระบบ
                    if (CheckCollisionPointRec(mousePos, btnBegMoney)) {
                        p1.credit = 1000.0f; // รีเซ็ตเงิน
                        p1.win_count = 0;    // รีเซ็ตสถิติเพื่อความแฟร์
                        p1.loss_count = 0;
                    }
                    if (CheckCollisionPointRec(mousePos, btnHTP)) screenState = 3;
                } else {
                    // เล่นได้ปกติ
                    if (CheckCollisionPointRec(mousePos, btnSlot))      playSlotUI(p1); 
                    if (CheckCollisionPointRec(mousePos, btnHilo))      playHiloUI(p1); 
                    if (CheckCollisionPointRec(mousePos, btnBlackjack)) playBlackjackUI(p1); 
                    if (CheckCollisionPointRec(mousePos, btnBingo))     playBingoUI(p1); 
                    
                    // กดปุ่ม HOW TO PLAY
                    if (CheckCollisionPointRec(mousePos, btnHTP)) screenState = 3; 
                }

                // ปุ่ม Logout กดได้เสมอ
                if (CheckCollisionPointRec(mousePos, btnLogout)) {
                    if (p1.name != "Guest Player") Auth::SaveUser(p1, currentPassword);
                    inputUser = ""; inputPass = ""; currentPassword = "";
                    screenState = 0; 
                }
            }

            BeginDrawing();
            ClearBackground(Color{ 10, 30, 20, 255 }); // สีเขียวเข้ม VIP ให้ความรู้สึกโต๊ะคาสิโน
            
            DrawText("SCAMMER CASINO CLUB", (screenWidth - MeasureText("SCAMMER CASINO CLUB", 50)) / 2, 40, 50, GOLD);
            
            // แผงโปรไฟล์ผู้เล่น
            DrawRectangle(centerX - 300, 110, 600, 100, Color{0, 0, 0, 150});
            DrawRectangleLines(centerX - 300, 110, 600, 100, GOLD);
            DrawText(TextFormat("Player: %s", p1.name.c_str()), centerX - 270, 125, 25, RAYWHITE);
            DrawText(TextFormat("Total Credits: $%.2f", p1.credit), centerX - 270, 160, 35, (isBankrupt ? RED : LIME));
            
            DrawText(TextFormat("Wins: %d | Losses: %d", p1.win_count, p1.loss_count), centerX + 70, 125, 20, LIGHTGRAY);

            if (isBankrupt) {
                // --- หน้าจอตอนถังแตก ---
                DrawText("BANKRUPT!", (screenWidth - MeasureText("BANKRUPT!", 60)) / 2, 260, 60, RED);
                DrawText("You are out of money. The house always wins!", (screenWidth - MeasureText("You are out of money. The house always wins!", 25)) / 2, 330, 25, RAYWHITE);

                Color cBeg = CheckCollisionPointRec(mousePos, btnBegMoney) ? ORANGE : GOLD;
                DrawRectangleRec(btnBegMoney, cBeg);
                if (!CheckCollisionPointRec(mousePos, btnBegMoney)) DrawRectangle(btnBegMoney.x, btnBegMoney.y + btnBegMoney.height - 5, btnBegMoney.width, 5, Color{0,0,0,100});
                DrawRectangleLinesEx(btnBegMoney, 3, BLACK);
                DrawText("BEG FOR BAILOUT ($1000)", btnBegMoney.x + (btnBegMoney.width - MeasureText("BEG FOR BAILOUT ($1000)", 22))/2, btnBegMoney.y + 20, 22, BLACK);
            
                // ปุ่ม HOW TO PLAY (ตอนถังแตก)
                Color cHtp = CheckCollisionPointRec(mousePos, btnHTP) ? DARKGRAY : GRAY;
                DrawRectangleRec(btnHTP, cHtp);
                if (!CheckCollisionPointRec(mousePos, btnHTP)) DrawRectangle(btnHTP.x, btnHTP.y + btnHTP.height - 4, btnHTP.width, 4, Color{0,0,0,100});
                DrawRectangleLinesEx(btnHTP, 2, WHITE);
                DrawText("HOW TO PLAY", btnHTP.x + (btnHTP.width - MeasureText("HOW TO PLAY", 20))/2, btnHTP.y + 15, 20, WHITE);

            } else {
                // --- วาดปุ่มเกม (เล่นได้ปกติ) ---
                Color cSlot = CheckCollisionPointRec(mousePos, btnSlot) ? SKYBLUE : RAYWHITE;
                DrawRectangleRec(btnSlot, cSlot);
                if (!CheckCollisionPointRec(mousePos, btnSlot)) DrawRectangle(btnSlot.x, btnSlot.y + btnSlot.height - 5, btnSlot.width, 5, Color{0,0,0,100});
                DrawRectangleLinesEx(btnSlot, 3, BLACK);
                DrawText("PLAY SLOT MACHINE", btnSlot.x + (btnSlot.width - MeasureText("PLAY SLOT MACHINE", 22))/2, btnSlot.y + 15, 22, BLACK);

                Color cHilo = CheckCollisionPointRec(mousePos, btnHilo) ? SKYBLUE : RAYWHITE;
                DrawRectangleRec(btnHilo, cHilo);
                if (!CheckCollisionPointRec(mousePos, btnHilo)) DrawRectangle(btnHilo.x, btnHilo.y + btnHilo.height - 5, btnHilo.width, 5, Color{0,0,0,100});
                DrawRectangleLinesEx(btnHilo, 3, BLACK);
                DrawText("PLAY HI-LO GAME", btnHilo.x + (btnHilo.width - MeasureText("PLAY HI-LO GAME", 22))/2, btnHilo.y + 15, 22, BLACK);

                Color cBj = CheckCollisionPointRec(mousePos, btnBlackjack) ? SKYBLUE : RAYWHITE;
                DrawRectangleRec(btnBlackjack, cBj);
                if (!CheckCollisionPointRec(mousePos, btnBlackjack)) DrawRectangle(btnBlackjack.x, btnBlackjack.y + btnBlackjack.height - 5, btnBlackjack.width, 5, Color{0,0,0,100});
                DrawRectangleLinesEx(btnBlackjack, 3, BLACK);
                DrawText("PLAY BLACKJACK 21", btnBlackjack.x + (btnBlackjack.width - MeasureText("PLAY BLACKJACK 21", 22))/2, btnBlackjack.y + 15, 22, BLACK);

                Color cBingo = CheckCollisionPointRec(mousePos, btnBingo) ? SKYBLUE : RAYWHITE;
                DrawRectangleRec(btnBingo, cBingo);
                if (!CheckCollisionPointRec(mousePos, btnBingo)) DrawRectangle(btnBingo.x, btnBingo.y + btnBingo.height - 5, btnBingo.width, 5, Color{0,0,0,100});
                DrawRectangleLinesEx(btnBingo, 3, BLACK);
                DrawText("PLAY BINGO BONANZA", btnBingo.x + (btnBingo.width - MeasureText("PLAY BINGO BONANZA", 22))/2, btnBingo.y + 15, 22, BLACK);
                
                // ปุ่ม HOW TO PLAY
                Color cHtp = CheckCollisionPointRec(mousePos, btnHTP) ? DARKGRAY : GRAY;
                DrawRectangleRec(btnHTP, cHtp);
                if (!CheckCollisionPointRec(mousePos, btnHTP)) DrawRectangle(btnHTP.x, btnHTP.y + btnHTP.height - 4, btnHTP.width, 4, Color{0,0,0,100});
                DrawRectangleLinesEx(btnHTP, 2, WHITE);
                DrawText("HOW TO PLAY", btnHTP.x + (btnHTP.width - MeasureText("HOW TO PLAY", 20))/2, btnHTP.y + 15, 20, WHITE);
            }

            // ปุ่ม Logout (อยู่ล่างสุดเสมอ)
            Color cLog = CheckCollisionPointRec(mousePos, btnLogout) ? MAROON : RED;
            DrawRectangleRec(btnLogout, cLog);
            if (!CheckCollisionPointRec(mousePos, btnLogout)) DrawRectangle(btnLogout.x, btnLogout.y + btnLogout.height - 5, btnLogout.width, 5, Color{0,0,0,100});
            DrawRectangleLinesEx(btnLogout, 3, GOLD);
            DrawText("LOGOUT & SAVE", btnLogout.x + (btnLogout.width - MeasureText("LOGOUT & SAVE", 22))/2, btnLogout.y + 15, 22, GOLD);

            EndDrawing();
        }
        // ==========================================
        // STATE 3: หน้า HOW TO PLAY (โชว์รูปภาพ)
        // ==========================================
        else if (screenState == 3) {
            Rectangle btnBack = { 30, 30, 100, 40 };

            if (isClick && CheckCollisionPointRec(mousePos, btnBack)) {
                screenState = 2; // กลับไปหน้าคาสิโน Hub
            }

            BeginDrawing();
            ClearBackground(BLACK);

            if (htpImage.id != 0) {
                // วาดรูปภาพ ถ้าโหลดสำเร็จ
                DrawTexture(htpImage, 0, 0, WHITE);
            } else {
                // แจ้งเตือนถ้ารูปโหลดไม่ขึ้น
                DrawText("IMAGE NOT FOUND", centerX - MeasureText("IMAGE NOT FOUND", 40)/2, centerY, 40, RED);
                DrawText("Make sure 'howtoplay.png' is in the exact same folder as the .exe", centerX - MeasureText("Make sure 'howtoplay.png' is in the exact same folder as the .exe", 20)/2, centerY + 50, 20, LIGHTGRAY);
            }

            // วาดปุ่ม BACK ทับรูป
            DrawRectangleRec(btnBack, CheckCollisionPointRec(mousePos, btnBack) ? RED : MAROON);
            DrawRectangleLinesEx(btnBack, 2, WHITE);
            DrawText("BACK", btnBack.x + 20, btnBack.y + 10, 20, WHITE);

            EndDrawing();
        }
    }

    // =========================================================
    // ส่วนที่ 4: คืนหน่วยความจำและเซฟก่อนออก
    // =========================================================
    UnloadTexture(htpImage); // ลบรูปออกจากแรม

    if (screenState == 2 && p1.name != "Guest Player") {
        Auth::SaveUser(p1, currentPassword);
    }

    CloseWindow();
    return 0;
}