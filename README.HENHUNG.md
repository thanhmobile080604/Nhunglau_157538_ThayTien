# PROJECT NHÚNG IT4

Xây dựng game Tetris trên kit STM32F429I-DISCOVERY sử dụng TouchGFX, FreeRTOS và các ngoại vi: joystick, đèn led.

## GIỚI THIỆU

__Đề bài__: Xây dưng Game “Tetris”

__Sản phẩm:__
1. Xây dựng tựa game Tetris có đầy đủ logic: xoay khối, di chuyển, rơi, xóa hàng, tính điểm, thua game.
2. Hiển thị giao diện đồ họa TouchGFX.
3. Điều khiển game bằng joystick và thêm LED để tăng tính trực quan.
- Ảnh chụp minh họa:\
  ![image](https://github.com/user-attachments/assets/e09c6cbd-a362-4dee-bf04-800bf2b9ea45)

## TÁC GIẢ

- Tên nhóm: Nhúng lẩu
- Thành viên trong nhóm
  |STT|Họ tên|MSSV|Công việc|
  |--:|--|--|--|
  |1|Nguyễn Ngọc Thuận|20225413|hiện thị oled, và xử lý ngắt|
  |2|Vũ Tuấn Thành|20225406|Nối mạch, viết báo cáo|
  |3|Đỗ Thế Quân|20225382|Xử lí logic sự kiện game|

## MÔI TRƯỜNG HOẠT ĐỘNG

- Module CPU/dev kit sử dụng: STM32F429I-DISCOVERY (STM32F429ZIT6, ARM Cortex-M4), STM32CubeIDE, TouchGFX
- Kit ngoại được sử dụng: 2.4" TFT LCD with touch
                          JoyStick, Đèn LED

## SO ĐỒ SCHEMATIC
|STM32F429|Module ngoại vi|
|--|--|
|PC3|JoyStick - Ngõ ra trục X(VRx)|
|PA5|JoyStick - Ngõ ra trục Y(VRy)|
|3V3 (Từ kit)|JoyStick - Chân cấp nguồn(VCC)|
|GND (Từ kit)|JoyStick - Chân nối đất(GND)|
|PG2|Đèn LED|
|LCD Pins|Màn hình LCD-TFT|

### TÍCH HỢP HỆ THỐNG
- Phần cứng:
  |Linh kiện|Vai trò|
  |--|--|
  |JoyStick - Ngõ ra trục X(VRx)|Cho phép người chơi điều khiển di chuyển khối Tetris sang trái hoặc phải bằng cách nghiêng joystick theo trục ngang. Tín hiệu được đọc qua ADC1_IN13.|
  |JoyStick - Ngõ ra trục Y(VRy)|Dùng để tăng tốc rơi khối (kéo xuống nhanh) hoặc xoay khối khi đẩy joystick lên/xuống. Tín hiệu đọc từ ADC1_IN5.|
  |JoyStick - Chân cấp nguồn(VCC)|Cung cấp điện áp 3.3V để module Joystick hoạt động ổn định trong suốt quá trình chơi game.|
  |JoyStick - Chân nối đất(GND)|Nối với GND chung của hệ thống để đảm bảo tín hiệu joystick chính xác, không nhiễu loạn.|
  |Đèn LED|Nhận tín hiệu điều khiển để phát sáng khi người chơi ăn điểm.|
  |Màn hình LCD-TFT|Hiển thị giao diện trò chơi Tetris gồm khối gạch, điểm số, tốc độ rơi, và trạng thái chơi. Là trung tâm tương tác trực quan chính với người dùng.|
- Mô tả các thành phần phần mềm và vai trò của chúng, vị trí nằm trên phần cứng nào: Front-end, Back-end, Worker, Middleware...
  

### ĐẶC TẢ HÀM

- Giải thích một số hàm quan trọng: ý nghĩa của hàm, tham số vào, ra

  ```C
     /**
      *  Hàm tính ...
      *  @param  x  Tham số
      *  @param  y  Tham số
      */
     void abc(int x, int y = 2);
  ```
  
### KẾT QUẢ

- Các ảnh chụp với caption giải thích.
- Hoặc video sản phẩm
