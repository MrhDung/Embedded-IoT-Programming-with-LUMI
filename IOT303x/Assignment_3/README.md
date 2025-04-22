# IOT303x: Assignment 3

### Tên dự án: Giao tiếp vi điều khiển STM32F4 với phần mềm mô phỏng PC_Simulator_KIT

### Tổng quan dự án:
Viết chương trình MCU giao tiếp với phần mềm mô phỏng PC_Simulator_KIT, để điều khiển ngoại vi (như còi, led...) và truyền dữ liệu lên phần mềm để cập nhật giá trị của cảm biến ánh sáng, nhiệt độ - độ ẩm theo theo khung truyền sau:

![image](https://github.com/user-attachments/assets/335e3143-ef23-43ad-870a-9879d97cf329)

Các thành phần khung truyền:
- Trường Start: Byte bắt đầu của khung truyền, quy định là 0xB1.
- Trường Length: Độ dài của khung truyền, là tổng số byte có trong khung truyền (không tính byte Start). Dùng để xác định các thành phần có trong bản tin sau trường Length.
- Trường Option: Dự trữ cho tương lai nếu muốn sử dụng.
- Trường Command ID: Định danh bản tin tương ứng với từng module như led, còi, button...
- Trường Command Type: Loại bản tin giữa thiết bị và máy tính (HOST)
  + Set: Bản tin điều khiển thiết bị từ HOST.
  + Get: Bản tin lấy thông tin, trạng thái của thiết bị từ HOST.
  + Response: Bản tin phản hồi thông tin, trạng thái của thiết bị lên HOST.
- Trường Data: Dữ liệu khung truyền tương ứng với từng module.
- Trường Sequence: Mỗi bản tin khác nhau thì sẽ có 1 sequence khác nhau, để tránh trường hợp lặp bản tin (cùng 1 Sequence, thì sẽ hiểu đó là 1 bản tin)
- Trường CXOR: Mã check xor 8-bit đảm bảo tính toàn vẹn của khung truyền.
  + CXOR = 0xFF ^ OPT ^ CMDID ^ CMDTYPE ^ DATA(n bytes) ^ SEQUENCE

_Lưu ý: Khung truyền sẽ truyền theo thứ tự lần lượt từ trái qua phải, bắt đầu là byte Start và kết thúc là byte CXOR._

### Tại sao cần làm dự án này?
Mục tiêu giúp làm quen và hiểu được các vấn đề sau:
1. Biết cách sử dụng mảng.
2. Biết cách sử dụng con trỏ.
3. Biết cách chia và vận dụng các hàm con (Functions).
4. Thông qua dự án, sẽ được làm quen với những kĩ năng quan trọng trong lĩnh vực IOT như kĩ năng giao tiếp, truyền thông, kết nối giữa thiết bị sử dụng vi điều khiển với một hệ thống trung tâm.

### Tài nguyên tham khảo:
Để thực hiện dự án này, cần xem về giao thức truyền thông UART và cách trao đổi thông tin giữa thiết bị MCU với bộ điều khiển HOST, lưu trữ dữ liệu sử dụng bộ đệm FIFO. Có thể tham khảo tài liệu dưới đây (hoặc tự tìm thêm các tài liệu khác):
1. [Giao thức truyền thông UART nói chung](https://www.codrey.com/embedded-systems/uart-serial-communication-rs232/)
2. [Giao thức UART trên STM32F4](https://www.st.com/resource/en/application_note/dm00110292-implementing-an-emulated-uart-on-stm32f4-microcontrollers-stmicroelectronics.pdf)
3. [Bộ đệm FIFO - Embedded](https://www.embedded.com/ring-buffer-basics/)
4. [Tài liệu hướng dẫn sử dụng phần mềm PC Simulator KIT IOT STM](https://github.com/HoangNH95/IOT-Programming-with-Master-Embedded/blob/master/Courseware%202/PC_Simulator_BoardSTM32.7z)
