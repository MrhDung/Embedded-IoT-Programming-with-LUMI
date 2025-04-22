# IOT302x: Assignment 1

### Tên dự án: Mô phỏng thiết bị công tắc cảm ứng của Lumi Smart Home.

### Tổng quan dự án:
Viết chương trình mô phỏng thiết bị công tắc cảm ứng của Lumi SmartHome theo các yêu cầu sau:
1. Hiển thị dòng text *“IOT Programming by Lumi Smarthome”* lên màn hình LCD của Kit mở rộng khi bắt đầu cắm nguồn cho thiết bị.
2. Nhấn nút B3 năm lần khi đó tất cả các Led trên Kit mở rộng sẽ nháy năm lần màu GREEN với cường độ sáng là 50 % và hiển thị các thông tin lên màn hình LCD như sau:
   - *Device: Board STM32 Nucleo*
   - *Code: STM32F401RE_NUCLEO*
   - *Manufacturer: STMicroelectronics*
   - *Kit expansion: Lumi Smarthome*
   - *Project: Simulator touch switch*
3. Nhấn các nút B1, B2, B5, B4 một lần để điều khiển bật/tắt màu tương ứng RED, GREEN, BLUE, WHITE của tất cả các led RGB trên Kit mở rộng và còi sẽ kêu một tiếng bíp.
4. Nhấn giữ B1/B5 để điều khiển tăng/giảm cường độ sáng của led RGB:
   - Nhấn giữ nút B1 với thời gian T lớn hơn một giây để điều khiển tăng độ sáng của led màu GREEN và nhả ra thì dừng điều khiển.
   - Nhấn giữ nút B5 với thời gian T lớn hơn một giây để điều khiển giảm độ sáng của led màu GREEN và nhả ra thì dừng điều khiển.
5. Cập nhật thông số nhiệt độ, độ ẩm và cường độ ánh sáng của các cảm biến lên màn hình LCD theo chu kỳ có thể cấu hình được.

### Tại sao cần làm dự án này?
Mục tiêu giúp làm quen và hiểu được các vấn đề sau:
1. Biết cách sử dụng mảng.
2. Biết cách sử dụng con trỏ.
3. Biết cách chia và vận dụng các hàm con (Functions).
4. Thông qua dự án, sẽ học được các thao tác cơ bản của một hệ thống điều khiển bao gồm đo lường, xử lý tín hiệu từ cảm biến, điều khiển cơ cấu chấp hành và hiển thị.

### Tài nguyên tham khảo:
Dự án này sẽ xử lý các thao tác khi nhấn nút để điều khiển đầu ra của led, còi, màn hình LCD và đo đạc các thông số của cảm biến ở đầu vào. Vì thế để xử lý dự án này, cần phải tìm hiểu tài liệu tham khảo về xử lý nhiễu ở đầu vào của nút nhấn và lọc các giá trị nhiễu mà cảm biến trả về. Các bạn có thể tham khảo các tài liệu dưới đây để từ đó làm kiến thức cho mình để thực hiện bài tập:
1. [Xử lý tín hiệu nhiễu ở đầu vào nút nhấn trên STM32](http://www.emcu.it/STM32/STM32Discovery-Debounce/STM32Discovery-InputWithDebounce_Output_UART_SPI_SysTick.html)
2. [Giới thiệu về bộ lọc Kalman](https://www.kalmanfilter.net/default.aspx)
3. [Tutorial về bộ lọc Kalman của đại học MIT](https://web.mit.edu/kirtley/kirtley/binlustuff/literature/control/Kalman%20filter.pdf)
4. [Source code về bộ lọc Kalman](https://github.com/HoangNH95/IOT-Programming-with-Master-Embedded/blob/master/Courseware%202/Kalman_filter.7z)
