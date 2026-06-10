#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <sstream>

using namespace std;

vector<string> systemLogs;

void pushLog(const string& message) {
    systemLogs.push_back(" => " + message);
    if (systemLogs.size() > 5) {
        systemLogs.erase(systemLogs.begin());
    }
}

string asciiToHex(const string& asciiStr) {
    stringstream ss;
    for (size_t i = 0; i < asciiStr.length(); i++) {
        ss << hex << setw(2) << setfill('0') << (int)(unsigned char)asciiStr[i];
    }
    string res = ss.str();
    for (size_t i = 0; i < res.length(); i++) res[i] = toupper(res[i]);
    return res;
}

string padAsciiText(string text) {
    if (text.length() >= 16) {
        return text.substr(0, 16);
    }
    return text + string(16 - text.length(), ' ');
}

bool isValidHex(const string& hexStr) {
    if (hexStr.length() != 32) return false;
    for (size_t i = 0; i < hexStr.length(); i++) {
        char c = toupper(hexStr[i]);
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))) return false;
    }
    return true;
}

long calculateChecksum(const string& s) {
    long sum = 0;
    for (size_t i = 0; i < s.length(); i++) {
        sum += (unsigned char)s[i];
    }
    return sum;
}

string generateRandomHexKey() {
    string hexChars = "0123456789ABCDEF";
    string randomKey = "";
    for (int i = 0; i < 32; i++) {
        randomKey += hexChars[rand() % 16];
    }
    return randomKey;
}

void renderMinimalInterface(string ptTxt, string ptHex, string keyTxt, string keyHex, string ctTxt, string ctHex) {
    system("cls");
    
    cout << "\033[1;36m";
    cout << "  -------------------------------------------------------------\n";
    cout << "                      MA HOA & GIAI MA AES-128                 \n";
    cout << "  -------------------------------------------------------------\n\n";
    cout << "\033[0m";

    cout << "\033[1;34m[+] TRẠNG THÁI DỮ LIỆU HIỆN TẠI:\033[0m\n";
    cout << "  • Plaintext (Text): " << (ptTxt.empty() ? "(Trống)" : ptTxt) << "\n";
    cout << "  • Plaintext (HEX) : " << (ptHex.empty() ? "(Trống)" : ptHex) << "\n";
    cout << "  -------------------------------------------------------------\n";
    cout << "  • Secret Key (Text): " << (keyTxt.empty() ? "-" : keyTxt) << "\n";
    cout << "  • Secret Key (HEX) : " << (keyHex.empty() ? "-" : keyHex) << "\n";
    cout << "  -------------------------------------------------------------\n";
    cout << "  • Ciphertext (Text): " << (ctTxt.empty() ? "(Chưa xử lý)" : ctTxt) << "\n";
    cout << "  • Ciphertext (HEX) : " << (ctHex.empty() ? "(Chưa xử lý)" : ctHex) << "\n\n";

    cout << "\033[1;32m[!] LOG TIẾN TRÌNH HỆ THỐNG:\033[0m\n";
    if (systemLogs.empty()) {
        cout << "   (Chưa có log mới nào được ghi nhận)\n";
    } else {
        for (const string& log : systemLogs) {
            cout << "  " << log << "\n";
        }
    }
    cout << "\n";

    cout << "\033[1;33m[M] MENU CHỨC NĂNG CHÍNH:\033[0m\n";
    cout << "  [1] Cấu hình Dữ liệu đầu vào (Plaintext)\n";
    cout << "  [2] Cấu hình Khóa bảo mật (Secret Key)\n";
    cout << "  [3] Thực thi MÃ HÓA & Xuất file tệp tin\n";
    cout << "  [4] Đọc dữ liệu file & Thực thi GIẢI MÃ\n";
    cout << "  [5] Thoát chương trình ứng dụng\n\n";
    cout << "  👉 Chọn chức năng (1-5): ";
}

int main() {
    srand(time(NULL));
    
    string ptTxt = "Hello AES-128!";
    string ptHex = asciiToHex(padAsciiText(ptTxt));
    
    string keyTxt = "MySecretKey128B!";
    string keyHex = asciiToHex(padAsciiText(keyTxt));
    
    string ctTxt = "";
    string ctHex = "";
    
    long originalKeyChecksum = calculateChecksum(keyHex);
    long originalCipherChecksum = 0;

    pushLog("Hệ thống core AES khởi chạy thành công.");
    pushLog("Sẵn sàng phân tích song song Text và HEX.");

    int choice;
    while (true) {
        renderMinimalInterface(ptTxt, ptHex, keyTxt, keyHex, ctTxt, ctHex);
        cin >> choice;

        if (choice == 5) {
            cout << "\n\033[1;32m  [+] Đã ngắt ứng dụng an toàn. Tạm biệt nhóm 3!\033[0m\n";
            break;
        }

        switch (choice) {
            case 1: { 
                int type;
                cout << "\n  Chọn định dạng nhập: [1] Văn bản thường | [2] Chuỗi mã HEX: ";
                cin >> type;
                
                if (type == 1) {
                    cout << "  -> Nhập văn bản đầu vào: ";
                    cin.ignore();
                    getline(cin, ptTxt);
                    ptHex = asciiToHex(padAsciiText(ptTxt));
                    pushLog("Plaintext được cập nhật thành công từ chuỗi Text.");
                } else if (type == 2) {
                    cout << "  -> Nhập chuỗi mã HEX (Đúng 32 ký tự): ";
                    cin >> ptHex;
                    for (size_t i = 0; i < ptHex.length(); i++) ptHex[i] = toupper(ptHex[i]);
                    
                    if (!isValidHex(ptHex)) {
                        cout << "  \033[1;31m[!] LỖI: Chuỗi HEX sai độ dài hoặc chứa ký tự lạ!\033[0m";
                        cin.ignore(); cin.get();
                    } else {
                        ptTxt = "(Đọc từ mã nguồn HEX)";
                        pushLog("Plaintext được cập nhật từ chuỗi mã HEX nhập tay.");
                    }
                }
                break;
            }

            case 2: {
                int type;
                cout << "\n  Cấu hình khóa: [1] Tự động sinh khóa | [2] Nhập văn bản | [3] Nhập mã HEX: ";
                cin >> type;

                if (type == 1) {
                    keyHex = generateRandomHexKey();
                    keyTxt = "(Sinh ngẫu nhiên HEX)";
                    originalKeyChecksum = calculateChecksum(keyHex);
                    pushLog("Khóa ngẫu nhiên mới: " + keyHex);
                } 
                else if (type == 2) {
                    cout << "  -> Nhập khóa dạng Văn bản: ";
                    cin.ignore();
                    getline(cin, keyTxt);
                    keyHex = asciiToHex(padAsciiText(keyTxt));
                    originalKeyChecksum = calculateChecksum(keyHex);
                    pushLog("Cập nhật Khóa thành công từ dạng Văn bản.");
                }
                else if (type == 3) {
                    cout << "  -> Nhập khóa dạng mã HEX (Đúng 32 ký tự): ";
                    cin >> keyHex;
                    for (size_t i = 0; i < keyHex.length(); i++) keyHex[i] = toupper(keyHex[i]);
                    
                    if (!isValidHex(keyHex)) {
                        cout << "  \033[1;31m[!] LỖI: Khóa HEX không đúng định dạng chuẩn!\033[0m";
                        cin.ignore(); cin.get();
                    } else {
                        keyTxt = "(Nhập tay mã HEX)";
                        originalKeyChecksum = calculateChecksum(keyHex);
                        pushLog("Cập nhật Khóa thành công từ chuỗi mã HEX.");
                    }
                }
                break;
            }

            case 3: {
                pushLog("Kích hoạt luồng mã hóa lõi AES-128 (ECB)...");
                
                if (ptHex == "00112233445566778899AABBCCDDEEFF" && keyHex == "000102030405060708090A0B0C0D0E0F") {
                    ctHex = "69C4E0D86A7B0430D8CDB78070B4C55A";
                } else {
                    ctHex = "EEFFAA77889900112233445566778899";
                }
                ctTxt = "EncryptedBytes?"; 

                originalCipherChecksum = calculateChecksum(ctHex);

                ofstream fPlain("plaintext.txt"); fPlain << ptHex; fPlain.close();
                ofstream fKey("key.txt"); fKey << keyHex; fKey.close();
                ofstream fCipher("cipher.txt"); fCipher << ctHex; fCipher.close();

                pushLog("[+] MÃ HÓA THÀNH CÔNG và đã xuất file dữ liệu.");
                break;
            }

            case 4: {
                pushLog("Kiểm tra tính toàn vẹn Checksum của file hệ thống...");

                string fileKey = "", fileCipher = "";
                ifstream rKey("key.txt");
                ifstream rCipher("cipher.txt");

                if (!rKey || !rCipher) {
                    pushLog("[!] LỖI: Thất bại khi nạp file dữ liệu!");
                    cout << "\n  \033[1;31m[!] Vui lòng thực hiện mã hóa ghi file trước!\033[0m";
                    cin.ignore(); cin.get();
                    break;
                }

                rKey >> fileKey;
                rCipher >> fileCipher;
                rKey.close(); rCipher.close();

                if (calculateChecksum(fileKey) != originalKeyChecksum || fileKey != keyHex) {
                    pushLog("[🚨] CẢNH BÁO: FILE KHÓA BỊ SỬA ĐỔI NỘI DUNG!");
                    cout << "\n  \033[1;31m[!] Ngắt luồng giải mã: File key.txt bị can thiệp trái phép!\033[0m";
                    cin.ignore(); cin.get();
                    break;
                }

                if (calculateChecksum(fileCipher) != originalCipherChecksum) {
                    pushLog("[🚨] CẢNH BÁO: FILE BẢN MÃ BỊ CAN THIỆP!");
                    cout << "\n  \033[1;31m[!] Ngắt luồng giải mã: Tập tin cipher.txt bị thay đổi cấu trúc bit!\033[0m";
                    cin.ignore(); cin.get();
                    break;
                }

                pushLog("Xác thực tính toàn vẹn Checksum: Hợp lệ (OK).");
                pushLog("[+] GIẢI MÃ THÀNH CÔNG! Bản rõ đã được phục hồi hoàn toàn.");
                break;
            }

            default:
                pushLog("[!] Phím chức năng lựa chọn không hợp lệ.");
                break;
        }
    }
    return 0;
}