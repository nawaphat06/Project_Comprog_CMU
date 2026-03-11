//มุซันจะกลับมาในอเวนเจอร์ดูมเดย์
#include <iostream>
#include <string>

// นำเข้าข้อมูลระบบหลังบ้าน
#include "Player.h"
#include "Auth.h" 


// นำเข้าlibกราฟิกและไฟล์มินิเกมทั้งหมด
#include "raylib.h"
#include "slot.h" 
#include "hilo.h" 
#include "blackjack.h" 
#include "bingo.h" 


using namespace std;

// ซ่อนรหัสผ่านเป็น *
string getMaskedPassword(string pass) {
    return string(pass.length(), '*');
}

int main() {
    // ตั้งค่าพื้นฐานของหน้าต่างเกม
    const int screenWidth = 1280;
    const int screenHeight = 720;
    float centerX = screenWidth / float(2.0);
    float centerY = screenHeight / float(2.0);
    

    InitWindow(screenWidth, screenHeight, "SCAMMER CASINO CLUB - Handsome Edition");//ค.กว้าง,ค.สูง,ชื่อหน้าต่าง
    SetTargetFPS(60); // ล็อคค.เร็ว 60 fps ให้ไม่กินเครื่องเกินไป

    Player p1("Guest Player", 1000.0, 0, 0); //setค่าเริ่มต้น ชื่อ, เงิน, win, loss ชั่วคราว
    string currentPassword = ""; //เก็บรหัสผ่านชั่วคราวเอาไว้เซฟข้อมูลตอนปิดเกม
    
    //screenState 0 = หน้าแรกสุด, 1 = ฟอร์มกรอกชื่อและรหัส, 2 = คาสิโน Hub
    int screenState = 0; 
    bool isLoginMode = true; // แยกโหมดระหว่าง Login หรือ Register

    //ตัวแปรสำหรับการพิมพ์ข้อความด้วยคีย์บอร์ด
    string inputUser = "";
    string inputPass = "";
    int activeBox = 0; // 0 = ไม่เลือก, 1 = กำลังพิมพ์ Username, 2 = กำลังพิมพ์ Password
    string sysMsg = ""; // ข้อความแจ้งเตือน
    Color msgColor = RAYWHITE; //สีข้อความ

    // ===========
    //  GAME LOOP
    // ===========
    while (!WindowShouldClose()) { //ทำงานวนไปเรื่อยๆ จนกว่าผู้เล่นจะกดปิดหน้าต่างๆ
        
        //รับค่าตำแหน่งเมาส์และคลิกซ้าย
        Vector2 mousePos = GetMousePosition(); //ตน.mouse(X,Y)
        bool isClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        // ---------------------
        // screenState 0 หน้าแรก
        // ---------------------
        if (screenState == 0) {
            // ประกาศ Hitbox ของปุ่มทั้ง 4 ปุ่ม 
            //{ พิกัด X, พิกัด Y, ความกว้าง, ความสูง }
            Rectangle btnGoLogin  = { centerX - 150, 320, 300, 60 }; 
            Rectangle btnGoReg    = { centerX - 150, 400, 300, 60 };
            Rectangle btnGoGuest  = { centerX - 150, 480, 300, 60 };
            Rectangle btnExitGame = { centerX - 150, 560, 300, 60 }; 

            //Update: เช็คว่าเมาส์คลิกโดนปุ่มไหน
            if (isClick) {
                if (CheckCollisionPointRec(mousePos, btnGoLogin)) { screenState = 1; isLoginMode = true; sysMsg = "Enter Credentials to Login"; msgColor = RAYWHITE; activeBox = 1; }
                if (CheckCollisionPointRec(mousePos, btnGoReg))   { screenState = 1; isLoginMode = false; sysMsg = "Create a New Account"; msgColor = SKYBLUE; activeBox = 1; }
                if (CheckCollisionPointRec(mousePos, btnGoGuest)) { screenState = 2; p1.name = "Guest Player"; p1.credit = 1000.0; p1.win_count = 0; p1.loss_count = 0; }
                if (CheckCollisionPointRec(mousePos, btnExitGame)) break; //ออกจาก Game Loop เพื่อปิดเกม
            }

            // ขั้นตอน Draw: เริ่มวาดกราฟิกบนหน้าจอ
            BeginDrawing();
            ClearBackground(Color{ 15, 25, 50, 255 }); //สีพื้นหลัง = Midnight Blue
            
            // วาดแสงสะท้อนพื้นหลังแบบไล่ระดับ
            DrawCircleGradient(centerX, centerY, 600, Color{ 30, 50, 100, 255 }, Color{ 15, 25, 50, 255 });

            // วาดชื่อเกม (วาดสีดำก่อนเพื่อทำเป็นเงาตกกระทบ)
            DrawText("SCAMMER CASINO CLUB", centerX - MeasureText("SCAMMER CASINO CLUB", 60)/2 + 4, 104, 60, BLACK); //เงา
            DrawText("SCAMMER CASINO CLUB", centerX - MeasureText("SCAMMER CASINO CLUB", 60)/2, 100, 60, GOLD); //ตัวอักษร

            // วาดกรอบสี่เหลี่ยมพื้นหลังของเมนู
            DrawRectangleRounded({ centerX - 190, 290, 380, 360 }, 0.1f, 10, Color{ 0, 0, 0, 180 });
            DrawRectangleRoundedLines({ centerX - 190, 290, 380, 360 }, 0.1f, 10, GOLD); //กรอบทอง

            // ใช้ Loop วาดปุ่มทั้ง 4 ปุ่ม
            Rectangle btns[] = { btnGoLogin, btnGoReg, btnGoGuest, btnExitGame };
            const char* lbls[] = { "LOGIN", "REGISTER", "PLAY AS GUEST", "EXIT GAME" }; //label
            Color baseColors[] = { RAYWHITE, RAYWHITE, RAYWHITE, Color{ 180, 40, 40, 255 } };

            for(int i=0; i<4; i++) {
                bool hover = CheckCollisionPointRec(mousePos, btns[i]);
                //ถ้านำเมาส์ไปชี้ ให้เปลี่ยนเป็นสีทอง ถ้าไม่ชี้ให้ใช้สีปกติ
                DrawRectangleRounded(btns[i], 0.2f, 10, hover ? GOLD : baseColors[i]);
                DrawText(lbls[i],
                         btns[i].x + (btns[i].width - MeasureText(lbls[i], 25))/2,
                         btns[i].y + 18, 25,
                         (i == 3) ? WHITE : BLACK);
                // Rectangle btnGoLogin  = { centerX - 150, 320, 300, 60 }; (x,y,width,height)
                // Rectangle btnGoReg    = { centerX - 150, 400, 300, 60 };
                // Rectangle btnGoGuest  = { centerX - 150, 480, 300, 60 };
                // Rectangle btnExitGame = { centerX - 150, 560, 300, 60 }; 

            }

            EndDrawing();
        }
        
        // ---------------------------
        // screenState 1 ฟอร์มกรอกข้อมูล 
        // ---------------------------
        else if (screenState == 1) {
            Rectangle boxUser   = { centerX - 150, 260, 300, 50 };
            Rectangle boxPass   = { centerX - 150, 360, 300, 50 };
            Rectangle btnSubmit = { centerX - 150, 470, 300, 60 };
            Rectangle btnBack   = { 30, 30, 100, 40 };

            // Update: การคลิก
            if (isClick) {
                // เช็คว่าผู้เล่นกดกล่องข้อความอันไหน เพื่อเปิดการพิมพ์
                if (CheckCollisionPointRec(mousePos, boxUser)) activeBox = 1;
                else if (CheckCollisionPointRec(mousePos, boxPass)) activeBox = 2;
                else activeBox = 0;
                
                if (CheckCollisionPointRec(mousePos, btnBack)) { screenState = 0; inputUser = ""; inputPass = ""; }
                
                //กดปุ่มยืนยัน(Submit) ที่เชื่อมกับฐานข้อมูล Auth
                if (CheckCollisionPointRec(mousePos, btnSubmit)) {
                    if (inputUser.empty() || inputPass.empty()) { sysMsg = "Fields cannot be empty!"; msgColor = RED; }
                    else {
                        if (isLoginMode) {
                            // ใช้การส่งตัวแปร p1 แบบ Reference เข้าไปให้ฟังก์ชัน LoginUser จัดการยัดข้อมูลทับ
                            if (Auth::LoginUser(inputUser, inputPass, p1)) { 
                                currentPassword = inputPass; 
                                screenState = 2; }
                            else { 
                                sysMsg = "Invalid Username or Password!"; 
                                msgColor = RED; }
                        } else {
                            // ถ้าเป็นโหมด Register จะบันทึกข้อมูลต่อท้ายไฟล์ .txt
                            if (Auth::RegisterUser(inputUser, inputPass)) { 
                                sysMsg = "Success! Login now."; 
                                msgColor = GREEN; isLoginMode = true; 
                                inputPass = ""; activeBox = 2; 
                            }
                            else { 
                                sysMsg = "Username already exists!"; 
                                msgColor = RED; }
                        }
                    }
                }
            }

            // ระบบรับค่าจากคีย์บอร์ด
            if (activeBox > 0) {
                int key = GetCharPressed(); // อ่านค่าปุ่มที่ถูกกด
                while (key > 0) {
                    // อนุญาตเฉพาะตัวอักษรภาษาอังกฤษ ตัวเลข และสัญลักษณ์พื้นฐาน
                    if ((key >= 32) && (key <= 125)) {
                        //ตัวอักษรต่อท้ายตัวเก่า เกิน 15 ตัวห้ามพิมพ์ต่อ
                        if (activeBox == 1 && inputUser.length() < 15) inputUser += (char)key;
                        if (activeBox == 2 && inputPass.length() < 15) inputPass += (char)key;
                    }
                    key = GetCharPressed(); 
                }
                // ถ้ากด Backspace ให้ลบตัวอักษรล่าสุดออก 1 ตัว
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    if (activeBox == 1 && inputUser.length() > 0) inputUser.pop_back();
                    if (activeBox == 2 && inputPass.length() > 0) inputPass.pop_back();
                }
            }

            // ขั้นตอน Draw: วาดหน้าฟอร์ม
            BeginDrawing();
            ClearBackground(Color{ 15, 25, 50, 255 });//พื้นหลัง
            //(ข้อความ, แกน x (แบบกึ่งกลาง) , แกน y + พิกเซล, ขนาด front , สี )
            DrawText(isLoginMode ? "ACCOUNT LOGIN" : "ACCOUNT REGISTER", centerX - MeasureText(isLoginMode ? "ACCOUNT LOGIN" : "ACCOUNT REGISTER", 40)/2, 80, 40, GOLD);
            //ข้อความแจ้งเตือน
            DrawText(sysMsg.c_str(), centerX - MeasureText(sysMsg.c_str(), 20)/2, 150, 20, msgColor);
            // x y กว้าง สูง สี
            DrawRectangle(centerX - 190, 210, 380, 340, Color{0, 0, 0, 150});

            // วาดกล่อง Username และข้อความด้านใน
            DrawText("Username:", centerX - 150, 225, 20, RAYWHITE);
            DrawRectangleRec(boxUser, activeBox == 1 ? RAYWHITE : LIGHTGRAY);
            DrawRectangleLinesEx(boxUser, 3, activeBox == 1 ? GOLD : BLACK);
            DrawText(inputUser.c_str(), boxUser.x + 10, boxUser.y + 15, 20, BLACK);

            // วาดกล่อง Password ซ่อนรหัสเป็น ***
            DrawText("Password:", centerX - 150, 325, 20, RAYWHITE);
            DrawRectangleRec(boxPass, activeBox == 2 ? RAYWHITE : LIGHTGRAY);
            DrawRectangleLinesEx(boxPass, 3, activeBox == 2 ? GOLD : BLACK);
            DrawText(getMaskedPassword(inputPass).c_str(), boxPass.x + 10, boxPass.y + 15, 20, BLACK); 

            //แตะปุ่ม เปลี่ยนสี
            bool subHover = CheckCollisionPointRec(mousePos, btnSubmit);
            DrawRectangleRounded(btnSubmit, 0.2f, 10, subHover ? LIME : GREEN);
            DrawText(isLoginMode ? "LOGIN" : "REGISTER", btnSubmit.x + (btnSubmit.width - MeasureText(isLoginMode ? "LOGIN" : "REGISTER", 25))/2, btnSubmit.y + 18, 25, BLACK);
            
            DrawRectangleRec(btnBack, MAROON);
            DrawText("BACK", 50, 40, 20, WHITE);
            EndDrawing();
        }
        
        // --------------------
        // screenState 2 หน้าคาสิโนหลัก
        // ----------------------------
        else if (screenState == 2) {
            Rectangle btnSlot      = { centerX - 180, 270, 360, 55 };
            Rectangle btnHilo      = { centerX - 180, 340, 360, 55 };
            Rectangle btnBlackjack = { centerX - 180, 410, 360, 55 };
            Rectangle btnBingo     = { centerX - 180, 480, 360, 55 };
            Rectangle btnLogout    = { centerX - 180, 580, 360, 55 }; 
            
            Rectangle btnBegMoney  = { centerX - 180, 400, 360, 60 }; 

            // ตรวจสอบสถานะการเงิน ถ้าเงินหมด (ต่ำกว่า 1) จะถือว่าล้มละลาย
            bool isBankrupt = (p1.credit < 1.0f);

            // Update: เช็คการคลิกเข้าเกม
            if (isClick) {
                if (isBankrupt) {
                    // ถ้าถังแตก ให้กดได้แค่ปุ่มขอเงิน (รีเซ็ตข้อมูล)
                    if (CheckCollisionPointRec(mousePos, btnBegMoney)) { 
                        p1.credit = 1000.0f; 
                        p1.win_count = 0; 
                        p1.loss_count = 0; 
                    }
                } else {
                    // ถ้าเงินยังเหลือ กดเข้าเกมย่อยได้ p1 ข้อมูลผู้เล่ย
                    if (CheckCollisionPointRec(mousePos, btnSlot))      playSlotUI(p1); 
                    if (CheckCollisionPointRec(mousePos, btnHilo))      playHiloUI(p1); 
                    if (CheckCollisionPointRec(mousePos, btnBlackjack)) playBlackjackUI(p1); 
                    if (CheckCollisionPointRec(mousePos, btnBingo))     playBingoUI(p1); 
                }
                
                // ออกจากระบบและเซฟข้อมูล
                if (CheckCollisionPointRec(mousePos, btnLogout)) {
                    //ทำการบันทึกข้อมูลลงไฟล์ก่อนออกจากระบบ (เว้นแต่เป็น Guest)
                    if (p1.name != "Guest Player") Auth::SaveUser(p1, currentPassword);
                    inputUser = ""; 
                    inputPass = ""; 
                    currentPassword = ""; 
                    screenState = 0; 
                }
            }

            // ขั้นตอน Draw: วาดหน้าเมนูหลัก
            BeginDrawing();
            ClearBackground(Color{ 10, 40, 20, 255 }); // โทนสีเขียวคาสิโน
            DrawRectangleGradientV(0, 0, screenWidth, screenHeight, Color{ 20, 60, 30, 255 }, Color{ 10, 30, 15, 255 }); //ไล่สีแบบบนลงล่าง, สีบน, สีล่าง

            DrawText("SCAMMER CASINO CLUB", centerX - MeasureText("SCAMMER CASINO CLUB", 50)/2 + 3, 43, 50, BLACK);//เงา
            DrawText("SCAMMER CASINO CLUB", centerX - MeasureText("SCAMMER CASINO CLUB", 50)/2, 40, 50, GOLD);
            
            // แผงข้อมูลผู้เล่น (Player Profile)
            DrawRectangleRounded({ centerX - 320, 110, 640, 110 }, 0.1f, 10, Color{ 0, 0, 0, 160 });
            DrawRectangleRoundedLines({ centerX - 320, 110, 640, 110 }, 0.1f, 10, GOLD);
            DrawText(TextFormat("WELCOME, %s", p1.name.c_str()), centerX - 290, 130, 22, RAYWHITE);
            // แสดงยอดเงิน ถ้าถังแตกเป็นสีแดง ถ้าปกติเป็นสีเขียว
            DrawText(TextFormat("CREDITS: $%.2f", p1.credit), centerX - 290, 165, 35, isBankrupt ? RED : LIME);
            DrawText(TextFormat("W: %d  L: %d", p1.win_count, p1.loss_count), centerX +120, 135, 20, LIGHTGRAY);

            // เงื่อนไขการวาดปุ่ม: ถ้าล้มละลายซ่อนปุ่มเกมทั้งหมด
            if (isBankrupt) {
                DrawText("BANKRUPT!", centerX - MeasureText("BANKRUPT!", 60)/2, 280, 60, RED);
                DrawText("The house always wins...", centerX - MeasureText("The house always wins...", 20)/2, 350, 20, GRAY);
                
                bool begHover = CheckCollisionPointRec(mousePos, btnBegMoney);
                DrawRectangleRounded(btnBegMoney, 0.2f, 10, begHover ? GOLD : YELLOW);
                DrawText("BEG FOR $1000", btnBegMoney.x + (btnBegMoney.width - MeasureText("BEG FOR $1000", 25))/2, btnBegMoney.y + 18, 25, BLACK);
            } else {
                Rectangle mBtns[] = { btnSlot, btnHilo, btnBlackjack, btnBingo };
                const char* mLbls[] = { "SLOT MACHINE", "HI-LO GAME", "BLACKJACK 21", "BINGO BONANZA" };
                for(int i=0; i<4; i++) {
                    bool h = CheckCollisionPointRec(mousePos, mBtns[i]);
                    DrawRectangleRounded({ mBtns[i].x + 3, mBtns[i].y + 3, mBtns[i].width, mBtns[i].height }, 0.2f, 10, Color{ 0, 0, 0, 100 }); 
                    DrawRectangleRounded(mBtns[i], 0.2f, 10, h ? SKYBLUE : RAYWHITE);
                    DrawText(mLbls[i], mBtns[i].x + (mBtns[i].width - MeasureText(mLbls[i], 22))/2, mBtns[i].y + 16, 22, BLACK);
                }
            }

            // ปุ่ม Logout
            bool logHover = CheckCollisionPointRec(mousePos, btnLogout);
            DrawRectangleRounded(btnLogout, 0.2f, 10, logHover ? RED : MAROON);
            DrawText("LOGOUT & SAVE", btnLogout.x + (btnLogout.width - MeasureText("LOGOUT & SAVE", 22))/2, btnLogout.y + 16, 22, WHITE);
            
            EndDrawing(); // จบการวาด
        }
    }
    
    // ถ้าผู้เล่นกดปิดหน้าต่างตอนหน้า main menu ไปเลย ไม่ได้กด logout ให้เซฟข้อมูลเผื่อก่อนถ้าไม่ได้เป็น Guest
    if(screenState == 2 && p1.name != "Guest Player") Auth::SaveUser(p1, currentPassword);
    
    CloseWindow(); //ปิดหน้าต่าง
    return 0;
}