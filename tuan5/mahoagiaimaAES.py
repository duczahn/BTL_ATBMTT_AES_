import tkinter as tk
from tkinter import messagebox, filedialog
import customtkinter as ctk
from Crypto.Cipher import AES
import os
import binascii

BLOCK = 16

ctk.set_appearance_mode("Dark")
ctk.set_default_color_theme("blue")


def pad_ascii(text):
    if len(text) >= BLOCK:
        return text[:BLOCK].encode('utf-8')
    return text.ljust(BLOCK, ' ').encode('utf-8')

def pad_bytes(data):
    if len(data) % BLOCK == 0 and len(data) > 0:
        return data
    pad_len = BLOCK - len(data) % BLOCK
    return data + bytes([pad_len] * pad_len)


def unpad_bytes(data):
    if len(data) == 0:
        return data
    pad_len = data[-1]
    if 1 <= pad_len <= 16:
        if data[-pad_len:] == bytes([pad_len] * pad_len):
            return data[:-pad_len]
    return data


def to_bytes(hex_str):
    try:
        return binascii.unhexlify(hex_str.strip().replace(" ", ""))
    except:
        raise ValueError("Chuỗi HEX không hợp lệ (Chỉ gồm 0-9, A-F)")


def to_hex(b):
    return binascii.hexlify(b).decode().upper()


def safe_decode_ascii(b_data):
    res = ""
    for b in b_data:
        if 32 <= b <= 126:
            res += chr(b)
        else:
            res += ""
    return res

class AESApp(ctk.CTk):
    def __init__(self):
        super().__init__()

        self.title("AES-128 Dual-Format Toolkit")
        self.geometry("1100x680")
        self.resizable(False, False)

        self.grid_columnconfigure(0, weight=6)
        self.grid_columnconfigure(1, weight=4)
        self.grid_rowconfigure(0, weight=1)

        self.left_frame = ctk.CTkFrame(self, corner_radius=15, fg_color="#1e1e24")
        self.left_frame.grid(row=0, column=0, padx=20, pady=20, sticky="nsew")

        self.title_label = ctk.CTkLabel(self.left_frame, text="MÃ HÓA & GIẢI MÃ AES-128",
                                        font=ctk.CTkFont(size=20, weight="bold"))
        self.title_label.pack(pady=(20, 15))

        self.create_input_fields()
        self.create_action_buttons()
        self.create_file_buttons()

        self.right_frame = ctk.CTkFrame(self, corner_radius=15, fg_color="#111115")
        self.right_frame.grid(row=0, column=1, padx=(0, 20), pady=20, sticky="nsew")

        self.log_label = ctk.CTkLabel(self.right_frame, text="LOG TIẾN TRÌNH THỜI GIAN THỰC",
                                      font=ctk.CTkFont(size=13, weight="bold"), text_color="#58a6ff")
        self.log_label.pack(pady=(15, 5))

        self.log = ctk.CTkTextbox(self.right_frame, font=ctk.CTkFont(family="Consolas", size=12),
                                  fg_color="#070709", text_color="#39ff14")
        self.log.pack(fill=tk.BOTH, expand=True, padx=15, pady=15)

        self.log_msg("[*] Hệ thống AES-128 Pro khởi tạo thành công.")
        self.log_msg("[*] Sẵn sàng xử lý cả dạng Văn bản gốc và định dạng HEX.")

    def create_input_fields(self):
        pt_header = ctk.CTkFrame(self.left_frame, fg_color="transparent")
        pt_header.pack(fill="x", padx=35, pady=(5, 2))
        ctk.CTkLabel(pt_header, text="PLAINTEXT (DỮ LIỆU ĐẦU VÀO)", font=ctk.CTkFont(size=12, weight="bold")).pack(
            side=tk.LEFT)

        self.pt_type = ctk.CTkSegmentedButton(pt_header, values=["Văn bản", "Mã HEX"], width=150, height=26)
        self.pt_type.pack(side=tk.RIGHT)
        self.pt_type.set("Văn bản")

        self.pt = ctk.CTkEntry(self.left_frame,
                               placeholder_text="Nhập chuỗi văn bản (<=16 ký tự) hoặc chuỗi HEX (32 ký tự)...",
                               width=550, height=38, corner_radius=8)
        self.pt.pack(padx=35, pady=(0, 15))

        key_header = ctk.CTkFrame(self.left_frame, fg_color="transparent")
        key_header.pack(fill="x", padx=35, pady=(5, 2))
        ctk.CTkLabel(key_header, text="SECRET KEY (KHÓA BÍ MẬT)", font=ctk.CTkFont(size=12, weight="bold")).pack(
            side=tk.LEFT)

        self.key_type = ctk.CTkSegmentedButton(key_header, values=["Văn bản", "Mã HEX"], width=150, height=26)
        self.key_type.pack(side=tk.RIGHT)
        self.key_type.set("Văn bản")

        self.key = ctk.CTkEntry(self.left_frame,
                                placeholder_text="Khóa văn bản (<=16 ký tự) hoặc Khóa HEX (32 ký tự)...", width=550,
                                height=38, corner_radius=8)
        self.key.pack(padx=35, pady=(0, 15))

        ctk.CTkLabel(self.left_frame, text="CIPHERTEXT ĐẦU RA (DẠNG VĂN BẢN)",
                     font=ctk.CTkFont(size=12, weight="bold")).pack(anchor="w", padx=35, pady=(5, 2))
        self.ct_txt = ctk.CTkEntry(self.left_frame, placeholder_text="Kết quả hiển thị dạng văn bản ký tự...",
                                   width=550, height=38, corner_radius=8)
        self.ct_txt.pack(padx=35, pady=(0, 10))

        ctk.CTkLabel(self.left_frame, text="CIPHERTEXT ĐẦU RA (DẠNG MÃ HEX)",
                     font=ctk.CTkFont(size=12, weight="bold")).pack(anchor="w", padx=35, pady=(5, 2))
        self.ct_hex = ctk.CTkEntry(self.left_frame, placeholder_text="Kết quả hiển thị dạng chuỗi mã HEX...", width=550,
                                   height=38, corner_radius=8)
        self.ct_hex.pack(padx=35, pady=(0, 20))

    def create_action_buttons(self):
        btn_frame = ctk.CTkFrame(self.left_frame, fg_color="transparent")
        btn_frame.pack(fill="x", padx=35, pady=5)

        self.btn_encrypt = ctk.CTkButton(btn_frame, text="🔐 MÃ HÓA", fg_color="#1f77b4", hover_color="#115587",
                                         command=self.execute_encrypt, height=42,
                                         font=ctk.CTkFont(size=13, weight="bold"))
        self.btn_encrypt.pack(side=tk.LEFT, expand=True, fill="x", padx=(0, 5))

        self.btn_decrypt = ctk.CTkButton(btn_frame, text="🔓 GIẢI MÃ", fg_color="#d62728", hover_color="#9c1a1a",
                                         command=self.execute_decrypt, height=42,
                                         font=ctk.CTkFont(size=13, weight="bold"))
        self.btn_decrypt.pack(side=tk.LEFT, expand=True, fill="x", padx=5)

        self.btn_genkey = ctk.CTkButton(btn_frame, text="🗝 TẠO KHÓA", fg_color="#2ca02c", hover_color="#1e6f1e",
                                        command=self.make_key, height=42, font=ctk.CTkFont(size=13, weight="bold"))
        self.btn_genkey.pack(side=tk.LEFT, expand=True, fill="x", padx=(5, 0))

    def create_file_buttons(self):
        file_frame = ctk.CTkFrame(self.left_frame, fg_color="transparent")
        file_frame.pack(fill="x", padx=35, pady=(20, 10))

        ctk.CTkButton(file_frame, text="💾 Lưu Plaintext", fg_color="#4f5b66", hover_color="#343d46",
                      command=self.save_pt, height=35).pack(side=tk.LEFT, expand=True, fill="x", padx=(0, 5))
        ctk.CTkButton(file_frame, text="💾 Lưu Key", fg_color="#4f5b66", hover_color="#343d46", command=self.save_key,
                      height=35).pack(side=tk.LEFT, expand=True, fill="x", padx=5)
        ctk.CTkButton(file_frame, text="💾 Lưu Cipher (HEX)", fg_color="#4f5b66", hover_color="#343d46",
                      command=self.save_ct, height=35).pack(side=tk.LEFT, expand=True, fill="x", padx=(5, 0))

    def log_msg(self, msg):
        self.log.insert(tk.END, msg + "\n")
        self.log.see(tk.END)

    def get_key_bytes(self):
        raw_key = self.key.get().strip()
        if not raw_key:
            raise ValueError("Vui lòng điền thông tin vào ô Secret Key.")

        if self.key_type.get() == "Văn bản":
            self.log_msg(f"[>] Đang xử lý Khóa dạng văn bản: '{raw_key}'")
            return pad_ascii(raw_key)
        else:
            self.log_msg(f"[>] Đang xử lý Khóa dạng mã HEX: {raw_key}")
            k_bytes = to_bytes(raw_key)
            if len(k_bytes) != 16:
                raise ValueError("Khóa dạng HEX bắt buộc phải đủ 32 ký tự (16 bytes).")
            return k_bytes

    def make_key(self):
        k = os.urandom(16)
        self.key.delete(0, tk.END)
        if self.key_type.get() == "Văn bản":
            rand_txt = "".join(chr(65 + (b % 26)) for b in k)
            self.key.insert(0, rand_txt)
            self.log_msg(f"[+] Đã tự sinh ngẫu nhiên khóa văn bản: {rand_txt}")
        else:
            self.key.insert(0, to_hex(k))
            self.log_msg(f"[+] Đã tự sinh ngẫu nhiên khóa HEX: {to_hex(k)}")

    def execute_encrypt(self):
        try:
            raw_pt = self.pt.get().strip()
            if not raw_pt:
                raise ValueError("Vui lòng điền thông tin Plaintext đầu vào.")

            key_bytes = self.get_key_bytes()

            if self.pt_type.get() == "Văn bản":
                self.log_msg(f"[>] Định dạng Plaintext: Văn bản thuần. Nội dung: '{raw_pt}'")
                pt_bytes = pad_ascii(raw_pt)
            else:
                self.log_msg(f"[>] Định dạng Plaintext: Mã HEX. Nội dung: {raw_pt}")
                pt_bytes = to_bytes(raw_pt)
                if len(pt_bytes) != 16:
                    raise ValueError("Plaintext định dạng HEX bắt buộc phải đủ 32 ký tự (16 bytes).")

            self.log_msg("[*] Khởi chạy lõi AES-128: SubBytes -> ShiftRows -> MixColumns -> AddRoundKey")
            cipher = AES.new(key_bytes, AES.MODE_ECB)
            ct_bytes = cipher.encrypt(pad_bytes(pt_bytes))

            self.ct_txt.delete(0, tk.END)
            self.ct_txt.insert(0, safe_decode_ascii(ct_bytes))

            self.ct_hex.delete(0, tk.END)
            self.ct_hex.insert(0, to_hex(ct_bytes))

            self.log_msg("[+] MÃ HÓA THÀNH CÔNG!")
            self.log_msg(f"    -> Bản mã HEX: {to_hex(ct_bytes)}")
            messagebox.showinfo("Thành công", "Dữ liệu đã được mã hóa thành công!")
        except Exception as e:
            messagebox.showerror("Lỗi Mã Hóa", str(e))
            self.log_msg("[-] Mã hóa thất bại: " + str(e))

    def execute_decrypt(self):
        try:
            raw_ct_hex = self.ct_hex.get().strip()
            raw_ct_txt = self.ct_txt.get().strip()
            key_bytes = self.get_key_bytes()

            if raw_ct_hex:
                self.log_msg(f"[>] Tiến hành giải mã dựa trên Ciphertext (HEX): {raw_ct_hex}")
                ct_bytes = to_bytes(raw_ct_hex)
            elif raw_ct_txt:
                self.log_msg(f"[>] Tiến hành giải mã dựa trên Ciphertext (Văn bản): '{raw_ct_txt}'")
                ct_bytes = raw_ct_txt.encode('utf-8')
            else:
                raise ValueError(
                    "Vui lòng nhập dữ liệu bản mã vào ô Ciphertext (HEX hoặc Văn bản) để tiến hành giải mã.")

            cipher = AES.new(key_bytes, AES.MODE_ECB)
            decrypted_bytes = unpad_bytes(cipher.decrypt(ct_bytes))
            self.pt.delete(0, tk.END)
            if self.pt_type.get() == "Văn bản":
                clean_text = safe_decode_ascii(decrypted_bytes).rstrip()
                self.pt.insert(0, clean_text)
            else:
                self.pt.insert(0, to_hex(decrypted_bytes))

            self.log_msg("[+] GIẢI MÃ THÀNH CÔNG!")
            self.log_msg(f"    -> Khôi phục bản rõ (HEX): {to_hex(decrypted_bytes)}")
            messagebox.showinfo("Thành công", "Dữ liệu đã được giải mã phục hồi hoàn toàn!")
        except Exception as e:
            messagebox.showerror("Lỗi Giải Mã", str(e))
            self.log_msg("[-] Giải mã thất bại: " + str(e))


    def save_pt(self):
        self.save_file(self.pt.get().strip(), "plaintext.txt")

    def save_key(self):
        self.save_file(self.key.get().strip(), "key.txt")

    def save_ct(self):
        self.save_file(self.ct_hex.get().strip(), "ciphertext.txt")

    def save_file(self, data, name):
        if not data:
            messagebox.showwarning("Cảnh báo", "Không tìm thấy dữ liệu hợp lệ để tiến hành lưu!")
            return
        path = filedialog.asksaveasfilename(defaultextension=".txt", initialfile=name)
        if path:
            with open(path, "w", encoding="utf-8") as f:
                f.write(data)
            self.log_msg(f"[+] Đã lưu dữ liệu xuống file tệp tin: {os.path.basename(path)}")
            messagebox.showinfo("Thông báo", "Tệp tin đã được lưu trữ thành công!")


if __name__ == "__main__":
    app = AESApp()
    app.mainloop()