# 🌟 Dự án: Đồng Hồ Đa Năng ESP32-S3 + WS2812

## 📌 Mục tiêu đề tài

Thiết kế và phát triển một hệ thống **đồng hồ đa năng** sử dụng **vi điều khiển ESP32-S3** kết hợp với **màn hình LED WS2812 16x16**, **loa mini**, và **nút bấm vật lý**. Hệ thống cung cấp các chức năng:

- ⏰ Hiển thị thời gian thực
- 🔔 Báo thức linh hoạt
- 🎮 Chơi trò chơi đơn giản (Snake, Tetris,...)
- 🖼️ Hiển thị hình ảnh hoặc hoạt ảnh tùy chỉnh
- 📱 Điều khiển từ xa thông qua **ứng dụng Android/Web** thông qua **giao tiếp BLE (Bluetooth Low Energy)**

> Ứng dụng giúp nâng cao trải nghiệm hiển thị thời gian và giải trí, đồng thời là bài học thực hành chuyên sâu về hệ thống nhúng, BLE, FreeRTOS và điều khiển đa luồng.

---

## 📦 Phạm vi và chức năng chính

### 1. **Hiển thị thời gian & giao diện**
- Màn hình LED 16x16 hiển thị giờ, phút, hiệu ứng trượt và các giao diện tùy chỉnh.
- Cập nhật thời gian thực từ NTP nếu kết nối WiFi.

### 2. **Chức năng báo thức**
- Hệ thống hỗ trợ nhiều báo thức.
- Phát âm thanh thông qua loa mini.
- Có thể cài đặt báo thức từ ứng dụng qua BLE.

### 3. **Trò chơi mini**
- Các trò chơi đơn giản (như Snake, Tetris) được hiển thị và điều khiển qua nút bấm.
- Giao diện chơi game cập nhật thời gian thực trên ma trận LED.

### 4. **Điều khiển qua BLE**
- Giao tiếp với ứng dụng Android hoặc Web thông qua BLE.
- Nhận dữ liệu để cập nhật thời gian, hình ảnh, thay đổi độ sáng, báo thức,...

### 5. **Nút bấm vật lý**
- Chuyển trạng thái giao diện (Clock, Menu, Game,...)
- Cấu hình, duyệt menu, điều khiển game.

---

## ⚙️ Phần cứng sử dụng

| Tên linh kiện         | Mô tả                             |
|-----------------------|-----------------------------------|
| ESP32-S3 (N8R16)      | Vi điều khiển chính, tích hợp BLE/WiFi |
| LED Matrix WS2812 16x16 | Ma trận LED hiển thị chính      |
| Loa mini              | Phát âm thanh báo thức            |
| Nút bấm (x6)          | Điều khiển giao diện và chức năng |
| Các linh kiện phụ trợ | Điện trở, tụ lọc, dây nối, nguồn 5V |

### 🛠️ Thư viện cần cài
Qua Library Manager trong Arduino IDE:
- `FastLED`
- `Preferences`
- `BLEDevice`
- `ArduinoJson`

## ✍️ Tác giả / Nhóm thực hiện

**Nhóm 03**

| Thành viên     | Vai trò                                      |
|----------------|-----------------------------------------------|
| Phan Văn Tú  | Thiết kế hệ thống, Lập trình ESP32 |
| Dương Thanh Tùng | Lập trình ESP32  |
| Vũ Xuân Tùng | Phát triển ứng dụng Web sử dụng BLE, Lập trình ESP32  |
| Lê Quốc Trung | Phát triển ứng dụng Android    |

