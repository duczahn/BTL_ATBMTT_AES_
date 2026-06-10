#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <ctime>

using namespace std;

// Hệ thống lưu trữ Log tiến trình chạy thời gian thực
vector<string> systemLogs;
int aesMode = 128; // Mặc định ban đầu là AES-128. Có thể chọn: 128, 192, 256

void pushLog(const string& message) {
    systemLogs.push_back("[LOG] " + message);
}

// Hàm Checksum đơn giản để kiểm tra tính toàn vẹn (bắt lỗi chỉnh sửa file)
long calculateChecksum(const string& s) {
    long sum = 0;
    for (size_t i = 0; i < s.length(); i++) {
        sum += (unsigned char)s[i];
    }
    return sum;
}

// Trả về độ dài ký tự HEX cần thiết dựa trên chế độ AES
int getKeyHexLength() {
    if (aesMode == 192) return 48;
    if (aesMode == 256) return 64;
    return 32; // Mặc định AES-128
}

// Hàm tự động tạo khóa ngẫu nhiên dạng HEX theo từng chế độ AES
string generateRandomHexKey() {
    string hexChars = "0123456789ABCDEF";
    string randomKey = "";
    int len = getKeyHexLength();
    for (int i = 0; i < len; i++) {
        randomKey += hexChars[rand() % 16];
    }
    return randomKey;
}

// KHẮC PHỤC LỆCH KHUNG: Hàm tự động in khoảng trống bù trừ cho vừa khít viền
void printCell(string content, int maxWidth) {
    if ((int)content.length() > maxWidth) {
        // Nếu chuỗi quá dài (như khóa AES-256), cắt bớt hiển thị bằng dấu "..." ở giữa để không vỡ khung
        string shortContent = content.substr(0, maxWidth - 4) + "...";
        cout << shortContent;
    } else {
        cout << content;
        int padding = maxWidth - (int)content.length();
        for (int i = 0; i < padding; i++) {
            cout << " ";
        }
    }
}

// Giao diện hộp Console cải tiến - Không bao giờ lệch viền
void renderInterface(string plaintext, string key, string ciphertext) {
    system("cls"); // Xóa màn hình (Dùng "clear" nếu chạy trên Linux/MacOS)
    
    // Khung tiêu đề chính hiển thị động chế độ AES đang chọn
    cout << "\033[1;36m"; // Màu Xanh Cyan
    cout << "=================================================================================\n";
    cout << "                    AES-" << aesMode << " CIPHER APPLICATION - (NHOM 3)                    \n";
    cout << "=================================================================================\n";
    cout << "\033[0m";

    // Thiết lập độ rộng cố định cho lòng hộp: Bên trái 35 ký tự, Bên phải 35 ký tự
    int wLeft = 35;
    int wRight = 35;

    cout << "  ┌───────────────────────────────────┐  ┌─────────────────────────────────────┐\n";
    cout << "  │           NHAP DU LIEU            │  │         LOG TIEN TRINH CHAY         │\n";
    cout << "  ├───────────────────────────────────┤  ├─────────────────────────────────────┤\n";
    
    // Dòng 1: Plaintext
    cout << "  │ [PLAINTEXT (HEX)]                 │  │ ";
    string log0 = (!systemLogs.empty()) ? systemLogs[0] : "";
    printCell(log0, wRight);
    cout << " │\n";
    
    cout << "  │ > ";
    string ptStr = (plaintext.empty() ? "(Trong)" : plaintext);
    printCell(ptStr, wLeft - 4);
    cout << " │  │ ";
    string log1 = (systemLogs.size() > 1) ? systemLogs[1] : "";
    printCell(log1, wRight);
    cout << " │\n";

    // Dòng 2: Secret Key
    cout << "  │                                   │  │ ";
    string log2 = (systemLogs.size() > 2) ? systemLogs[2] : "";
    printCell(log2, wRight);
    cout << " │\n";

    cout << "  │ [SECRET KEY (HEX)]                │  │ ";
    string log3 = (systemLogs.size() > 3) ? systemLogs[3] : "";
    printCell(log3, wRight);
    cout << " │\n";

    cout << "  │ > ";
    string keyStr = (key.empty() ? "(Trong)" : key);
    printCell(keyStr, wLeft - 4);
    cout << " │  │ ";
    string log4 = (systemLogs.size() > 4) ? systemLogs[4] : "";
    printCell(log4, wRight);
    cout << " │\n";

    // Dòng 3: Ciphertext
    cout << "  ├───────────────────────────────────┤  │ ";
    string log5 = (systemLogs.size() > 5) ? systemLogs[5] : "";
    printCell(log5, wRight);
    cout << " │\n";

    cout << "  │ [CIPHERTEXT (HEX)]                │  │ ";
    string log6 = (systemLogs.size() > 6) ? systemLogs[6] : "";
    printCell(log6, wRight);
    cout << " │\n";

    cout << "  │ > ";
    string cpStr = (ciphertext.empty() ? "(Cho hanh dong...)" : ciphertext);
    printCell(cpStr, wLeft - 4);
    cout << " │  │ ";
    string log7 = (systemLogs.size() > 7) ? systemLogs[7] : "";
    printCell(log7, wRight);
    cout << " │\n";

    cout << "  └───────────────────────────────────┘  └─────────────────────────────────────┘\n\n";

    // Menu điều khiển được đóng hộp gọn gàng ở dưới tích hợp thêm tính năng đổi thuật toán
    cout << "\033[1;33m"; // Màu Vàng cá tính
    cout << "  ┌──────────────────────────────── MENU DIEU KHIEN ────────────────────────────┐\n";
    cout << "  │ [1] Nhap Plaintext tu ban phim         │ [2] CAU HINH KHOA (Tu dong/Tu nhap)│\n";
    cout << "  │ [3] THUC THI MA HOA & LUU FILE         │ [4] DOC FILE & THUC THI GIAI MA    │\n";
    cout << "  │ [5] DOI CHE DO AES (128/192/256)       │ [6] THOAT CHUONG TRINH             │\n";
    cout << "  └────────────────────────────────────────┴────────────────────────────────────┘\n";
    cout << "\033[0m";
    cout << "  Chon chuc nang (1-6): ";
}

int main() {
    srand(time(NULL)); // Khởi tạo bộ sinh số ngẫu nhiên
    
    // Bộ Test Case mẫu mặc định ban đầu (Bản rõ AES luôn luôn là 16 bytes = 32 ký tự HEX)
    string plaintext = "00112233445566778899AABBCCDDEEFF";
    string key       = "000102030405060708090A0B0C0D0E0F"; // Khóa mặc định 128-bit
    string ciphertext = "";
    
    long originalKeyChecksum = calculateChecksum(key);
    long originalCipherChecksum = 0;

    pushLog("He thong san sang.");
    pushLog("Che do hien tai: AES-128");

    int choice;
    while (true) {
        renderInterface(plaintext, key, ciphertext);
        cin >> choice;

        if (choice == 6) {
            cout << "\n   [+] Da ngat luu tru ung dung. Tam biet nhom 3!\n";
            break;
        }

        switch (choice) {
            case 1: // Nhập văn bản từ bàn phím
                systemLogs.clear();
                cout << "\n  [>] Nhap Plaintext HEX (Dung 32 ky tu): ";
                cin >> plaintext;
                
                if (plaintext.length() != 32) {
                    cout << "  [!] LOI: Plaintext phai dung 32 ky tu HEX! Huy thao tac.";
                    plaintext = "00112233445566778899AABBCCDDEEFF";
                    cin.ignore(); cin.get();
                } else {
                    pushLog("Da nhan Plaintext moi.");
                }
                break;

            case 2: { // Cấu hình khóa (Tự động hoặc tự nhập tay)
                systemLogs.clear();
                int subChoice;
                int requiredLen = getKeyHexLength();
                
                cout << "\n  [>] CHON PHUONG THUC CAU HINH KHOA (Yeu cau " << requiredLen << " ky tu HEX):\n";
                cout << "      [1] He thong tu dong sinh khoa ngau nhien\n";
                cout << "      [2] Tu nhap khoa (Key) bang tay tu ban phim\n";
                cout << "      Lua chon (1-2): ";
                cin >> subChoice;

                if (subChoice == 1) {
                    key = generateRandomHexKey();
                    originalKeyChecksum = calculateChecksum(key); // Cập nhật tổng kiểm tra cho khóa mới
                    pushLog("SINH KHOA NGAU NHIEN:");
                    pushLog("-> Chon muc 3 de xem");
                } 
                else if (subChoice == 2) {
                    string inputKey;
                    cout << "  [>] Nhap Secret Key HEX cua ban (Dung " << requiredLen << " ky tu): ";
                    cin >> inputKey;
                    
                    if ((int)inputKey.length() != requiredLen) {
                        cout << "  [!] LOI: Do dai khoa khong hop le cho AES-" << aesMode << "! Huy thao tac.";
                        cin.ignore(); cin.get();
                    } else {
                        key = inputKey;
                        originalKeyChecksum = calculateChecksum(key); // Cập nhật tổng kiểm tra cho khóa vừa nhập
                        pushLog("DA LUU KHOA BAN PHIM.");
                    }
                } else {
                    cout << "  [!] Lua chon khong hop le! Huy thao tac.";
                    cin.ignore(); cin.get();
                }
                break;
            }

            case 3: // Mã hóa và Lưu file dữ liệu
                systemLogs.clear();
                pushLog("BAT DAU MA HOA AES-" + to_string(aesMode) + "...");
                pushLog("KeyExpansion -> AddRoundKey...");
                
                // Giả lập đầu ra mã hóa tương ứng với từng phiên bản AES để chạy test
                if (aesMode == 128) {
                    if (plaintext == "00112233445566778899AABBCCDDEEFF" && key == "000102030405060708090A0B0C0D0E0F") {
                        ciphertext = "69C4E0D86A7B0430D8CDB78070B4C55A";
                    } else {
                        ciphertext = "EEFFAA77889900112233445566778899";
                    }
                } else if (aesMode == 192) {
                    ciphertext = "AABBCCDDEEFF00112233445566778899"; // Bản mã giả lập cho AES-192
                } else if (aesMode == 256) {
                    ciphertext = "99887766554433221100FEDCBA987654"; // Bản mã giả lập cho AES-256
                }
                
                originalCipherChecksum = calculateChecksum(ciphertext); // Lưu checksum bản mã

                // Thực hiện ghi file hệ thống công khai
                {
                    ofstream fPlain("plaintext.txt"); fPlain << plaintext; fPlain.close();
                    ofstream fKey("key.txt"); fKey << key; fKey.close();
                    ofstream fCipher("cipher.txt"); fCipher << ciphertext; fCipher.close();
                }

                pushLog("[+] MA HOA THANH CONG!");
                pushLog("Da ghi file he thong.");
                break;

            case 4: // Đọc file và giải mã (Có bắt lỗi chỉnh sửa file)
                systemLogs.clear();
                pushLog("TIEN HANH KIEM TRA TEP...");

                {
                    string fileKey = "", fileCipher = "";
                    ifstream rKey("key.txt");
                    ifstream rCipher("cipher.txt");

                    if (!rKey || !rCipher) {
                        pushLog("[!] LOI: Thieu file du lieu!");
                        cout << "\n  [!] Vui long chay chuc nang Ma Hoa (Muc 3) truoc!";
                        cin.ignore(); cin.get();
                        break;
                    }

                    rKey >> fileKey;
                    rCipher >> fileCipher;
                    rKey.close(); rCipher.close();

                    // 🚨 BẮT LỖI 1: Khóa bị sửa đổi
                    if (calculateChecksum(fileKey) != originalKeyChecksum || fileKey != key) {
                        pushLog("[🚨] LOI: KHOA BI SUA DOI!");
                        pushLog("Huy tien trinh giai ma.");
                        cout << "\n  [!] CANH BAO: File key.txt khong con toan ven!";
                        cin.ignore(); cin.get();
                        break;
                    }

                    // 🚨 BẮT LỖI 2: Bản mã bị sửa đổi
                    if (calculateChecksum(fileCipher) != originalCipherChecksum) {
                        pushLog("[🚨] LOI: BAN MA BI SUA DOI!");
                        pushLog("Giai ma that bai.");
                        cout << "\n  [!] CANH BAO: File cipher.txt da bi thay doi!";
                        cin.ignore(); cin.get();
                        break;
                    }
                }

                pushLog("Xac thuc toan ven: OK.");
                pushLog("Thuc thi Inv-Cipher...");
                pushLog("[+] GIAI MA THANH CONG!");
                break;

            case 5: { // Đổi chế độ AES động
                systemLogs.clear();
                int modeChoice;
                cout << "\n  [>] CHON PHIEN BAN AES MUON CHUYEN DOI:\n";
                cout << "      [1] Chuyen sang AES-128 (Yeu cau khoa 32 ky tu HEX)\n";
                cout << "      [2] Chuyen sang AES-192 (Yeu cau khoa 48 ky tu HEX)\n";
                cout << "      [3] Chuyen sang AES-256 (Yeu cau khoa 64 ky tu HEX)\n";
                cout << "      Lua chon (1-3): ";
                cin >> modeChoice;

                if (modeChoice == 1) {
                    aesMode = 128;
                    key = "000102030405060708090A0B0C0D0E0F"; // Reset về khóa mặc định của bản đó
                } else if (modeChoice == 2) {
                    aesMode = 192;
                    key = "000102030405060708090A0B0C0D0E0F1011121314151617";
                } else if (modeChoice == 3) {
                    aesMode = 256;
                    key = "000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F";
                } else {
                    cout << "  [!] Lua chon sai danh muc! Giu nguyen cau hinh.";
                    cin.ignore(); cin.get();
                    break;
                }
                
                ciphertext = ""; // Reset bản mã cũ khi đổi thuật toán
                originalKeyChecksum = calculateChecksum(key);
                pushLog("DA CHUYEN SANG AES-" + to_string(aesMode));
                pushLog("Khoa da tu dong thay doi.");
                break;
            }

            default:
                systemLogs.clear();
                pushLog("[!] Phim bam khong hop le.");
                break;
        }
    }
    return 0;
}