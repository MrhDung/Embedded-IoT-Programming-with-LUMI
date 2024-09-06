# IOT302x: Assignment 2

### Tên dự án: Giao tiếp với phần mềm mô phỏng PC Simulator KIT IOT.

### Tổng quan dự án:
- Dự án này mô phỏng quá trình điều khiển và giám sát thiết bị KIT IOT bao gồm Board STM32 và Kit mở rộng thông qua một phần mềm giao diện trên máy tính. Phần mềm này sẽ giám sát các thông số môi trường như nhiệt độ - độ ẩm, cường độ ánh sáng của cảm biến và cho phép điều khiển bật tắt các ngoại vi như led, còi và hiển thị thông tin lên màn hình LCD tương tự như một phần mềm trên điện thoại.
- Download phần mềm mô phỏng PC Simulator KIT IOT và đọc hướng dẫn sử dụng [tại đây](https://github.com/HoangNH95/IOT-Programming-with-Master-Embedded/blob/master/Courseware%202/PC_Simulator_BoardSTM32.7z) để chạy phần mềm cùng với thiết bị KIT IOT.
- Từ Assignment 1, hãy viết chương trình cho vi điều khiển STM32F401 giao tiếp với phần mềm mô phỏng PC Simulator KIT IOT theo chuẩn giao tiếp truyền thông UART:

  1\. Hiển thị thông tin thiết bị IOT KIT lên phần mềm mô phỏng.

  2\. Nhấn các nút B1, B2, B5, B4 một lần để điều khiển bật màu tương ứng RED, GREEN, BLUE, WHITE của tất cả các led và gửi bản tin cập nhật trạng thái màu led tương ứng theo định dạng; nhấn một lần nữa sẽ điều khiển tắt màu của tất cả led tương ứng và gửi bản tin theo định dạng để cập nhật trạng thái màu led tương ứng.

  3\. Khi nhấn các nút B1, B2, B5, B4 một lần thì còi sẽ kêu một tiếng bíp và đồng thời gửi bản tin cập nhật trạng thái kêu một tiếng bíp lên phần mềm mô phỏng theo định dạng.

  4\. Điều khiển bật/tắt led RGB trên thiết bị IOT KIT khi nhận được bản tin định dạng từ phần mềm mô phỏng.

  5\. Điều khiển còi khi nhận được bản tin theo định dạng từ phần mềm mô phỏng.

  6\. Điều khiển màn hình LCD hiển thị đoạn văn bản khi nhận được bản tin theo định dạng từ phần mềm mô phỏng.

  7\. Đọc giá trị nhiệt độ - độ ẩm và cường độ ánh sáng từ các cảm biến với thời gian lấy mẫu (Tsample) một giây và gửi các thông số đó theo định dạng lên phần mềm mô phỏng để hiển thị dưới dạng đồ thị theo thời gian thực.

### Tại sao cần làm dự án này?
Mục tiêu giúp làm quen và hiểu được các vấn đề sau:
1. Biết cách sử dụng cấu trúc để quản lý các thông tin của bản tin giao tiếp.
2. Biết cách sử dụng con trỏ hàm (function callback) để xử lý các bản tin điều khiển và trạng thái khi giao tiếp.
3. Thông qua dự án, sẽ được làm quen với những kỹ năng quan trọng trong lĩnh vực IOT như kỹ năng giao tiếp, truyền thông, kết nối giữa thiết bị sử dụng vi điều khiển với một hệ thống trung tâm.

### Tài nguyên tham khảo:
Để thực hiện dự án này, cần phải hiểu về giao thức truyền thông UART và cách trao đổi thông tin giữa thiết bị với bộ điều khiển HOST. Có thể tham khảo tài liệu dưới đây hoặc tự tìm thêm các tài liệu liên quan đến chuẩn giao thức UART:
1. [Giao thức truyền thông UART nói chung](https://www.codrey.com/embedded-systems/uart-serial-communication-rs232/)
2. [Giao thức UART trên STM32F4](https://www.st.com/resource/en/application_note/dm00110292-implementing-an-emulated-uart-on-stm32f4-microcontrollers-stmicroelectronics.pdf)
3. [Tài liệu hướng dẫn sử dụng phần mềm PC Simulator KIT IOT STM](https://github.com/HoangNH95/IOT-Programming-with-Master-Embedded/blob/master/Courseware%202/PC_Simulator_BoardSTM32.7z)
4. [Phần mềm Hercules trên Windows](https://www.hw-group.com/software/hercules-setup-utility)
