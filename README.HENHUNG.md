# 🎮PROJECT NHÚNG IT4210

Xây dựng game Tetris trên kit STM32F429I-DISCOVERY sử dụng TouchGFX, FreeRTOS và các ngoại vi: joystick, đèn led.

## 📌GIỚI THIỆU

__Đề bài__: Xây dựng Game “Tetris”

__Sản phẩm:__
1. Xây dựng tựa game Tetris có đầy đủ logic: xoay khối, di chuyển, rơi, xóa hàng, tính điểm, thua game.
2. Hiển thị giao diện đồ họa TouchGFX.
3. Điều khiển game bằng joystick và thêm LED để tăng tính trực quan.
- 📷Ảnh chụp minh họa:\
  ![image](https://github.com/user-attachments/assets/e09c6cbd-a362-4dee-bf04-800bf2b9ea45)

## 👥TÁC GIẢ

- Tên nhóm: Nhúng lẩu
- Thành viên trong nhóm
  |STT|Họ tên|MSSV|Công việc|
  |--:|--|--|--|
  |1|Nguyễn Ngọc Thuận|20225413|Hiện thị oled, và xử lý ngắt|
  |2|Vũ Tuấn Thành|20225406|Nối mạch, viết báo cáo|
  |3|Đỗ Thế Quân|20225382|Xử lí logic sự kiện game|

## ⚙️MÔI TRƯỜNG HOẠT ĐỘNG

  &nbsp;&nbsp;&nbsp;&nbsp;•	Kit chính: STM32F429I-DISCOVERY (STM32F429ZIT6 – ARM Cortex-M4)

  &nbsp;&nbsp;&nbsp;&nbsp;•	IDE: STM32CubeIDE

  &nbsp;&nbsp;&nbsp;&nbsp;•	Giao diện: TouchGFX Designer

  &nbsp;&nbsp;&nbsp;&nbsp;•	RTOS: FreeRTOS (xử lý thời gian thực)

  &nbsp;&nbsp;&nbsp;&nbsp;•	Ngoại vi sử dụng: Joystick 2 trục, đèn LED GPIO, LCD-TFT 2.4”

## 🧩SƠ ĐỒ SCHEMATIC

  > |STM32F429|Module ngoại vi|
  > |--|--|
  > |PC3|JoyStick - Ngõ ra trục X(VRx)|
  |PA5|JoyStick - Ngõ ra trục Y(VRy)|
  |3V3 (Từ kit)|JoyStick - Chân cấp nguồn(VCC)|
  |GND (Từ kit)|JoyStick - Chân nối đất(GND)|
  |PG2|Đèn LED|
  |LCD Pins|Màn hình LCD-TFT|

### 🧠TÍCH HỢP HỆ THỐNG

- Phần cứng:
  |Linh kiện|Vai trò|
  |--|--|
  |JoyStick - Ngõ ra trục X(VRx)|Cho phép người chơi điều khiển di chuyển khối Tetris sang trái hoặc phải bằng cách nghiêng joystick theo trục ngang. Tín hiệu được đọc qua ADC1_IN13.|
  |JoyStick - Ngõ ra trục Y(VRy)|Dùng để tăng tốc rơi khối (kéo xuống nhanh) hoặc xoay khối khi đẩy joystick lên/xuống. Tín hiệu đọc từ ADC1_IN5.|
  |JoyStick - Chân cấp nguồn(VCC)|Cung cấp điện áp 3.3V để module Joystick hoạt động ổn định trong suốt quá trình chơi game.|
  |JoyStick - Chân nối đất(GND)|Nối với GND chung của hệ thống để đảm bảo tín hiệu joystick chính xác, không nhiễu loạn.|
  |Đèn LED|Nhận tín hiệu điều khiển để phát sáng khi người chơi ăn điểm.|
  |Màn hình LCD-TFT|Hiển thị giao diện trò chơi Tetris gồm khối gạch, điểm số, tốc độ rơi, và trạng thái chơi.|
- Phần mềm:
  |Thành phần|Vai trò|
  |--|--|
  |Front-end|Hiển thị giao diện TouchGFX, cập nhật khối, điểm số, khối kế tiếp.|
  |Back-end|Xử lý logic Tetris: di chuyển, xoay, rơi, xóa hàng, tính điểm.|
  |ADC Worker|Đọc giá trị analog từ joystick (ADC1, ADC2).|
  |FreeRTOS|Quản lý thời gian rơi khối định kỳ (handleTickEvent())|

### ĐẶC TẢ HÀM

- Ý nghĩa một số hàm quan trọng:

  ```C
     /**
      * Hàm xoay khối trong khi đang rơi
      */
     void Screen2View::rotateTetromino()
  ```
  ```C
     /**
      * Hàm kiểm tra vị trí va chạm của khối đang rơi
      * @param1 row: hàng cần kiểm tra
      * @param2 col: cột cần kiểm tra
      */
     bool Screen2View::canPlaceTetromino(int row, int col)
  ```
  ```C
     /**
      * Hàm xóa hàng khi có đầy đủ khối theo hướng nằm ngang
      */
     int Screen2View::checkAndClearLines()
  ```
  ```C
     /**
      * Hàm tính điểm cho người chơi
      * @param1 clearedLines: số hàng đầy đủ mà đã được xóa
      */
     void Screen2View::updateScore(int clearedLines)
  ```
  ```C
     /**
      * Hàm gia tăng tốc độ rơi của khối dựa trên level
      */
     uint32_t Screen2View::getFallDelay()
  ```
  
### ✅KẾT QUẢ
  &nbsp;&nbsp;&nbsp;&nbsp;•	🧩 Game chạy mượt trên STM32F429 với giao diện TouchGFX

  &nbsp;&nbsp;&nbsp;&nbsp;•	🎮 Joystick điều khiển chính xác, responsive

  &nbsp;&nbsp;&nbsp;&nbsp;•	💡 LED nhấp nháy khi xóa hàng, tăng điểm

  &nbsp;&nbsp;&nbsp;&nbsp;•	🧠 Logic đầy đủ theo chuẩn Tetris cổ điển

  &nbsp;&nbsp;&nbsp;&nbsp;👉 (Video của nhóm): https://drive.google.com/file/d/1-VqlwvGcNcQszQN_dZGHPZ5y4LJb_vh_/view?usp=sharing

### ❤️ Lời nói cuối

  &nbsp;&nbsp;&nbsp;&nbsp;Dù thời gian có hạn, nhưng nhờ có sự chỉ dẫn tận tình của thầy và tinh thần teamwork cao, chúng em đã hoàn thiện được sản phẩm chỉnh chu và đúng yêu cầu.

  &nbsp;&nbsp;&nbsp;&nbsp;Nhóm em xin chân thành cảm ơn thầy đã tận tình hỗ trợ trong suốt thời gian dạy học qua.

