# IOT303x: Assignment 1

### Tên dự án: Mô phỏng thiết bị công tắc cảm ứng điều khiển đóng/mở rèm cửa của Lumi Smart Home.

### Tổng quan dự án:
Viết chương trình MCU mô phỏng các hoạt động của công tắc cảm ứng điều khiển rèm theo các yêu cầu sau:
1. Thiết bị sẽ nháy led GREEN trên Board STM32 bốn lần để thông báo trạng thái của thiết bị đã được cấp nguồn.
2. Nhấn nút B3 năm lần sẽ điều khiển nháy tất cả các led RGB màu GREEN năm lần và đồng thời còi sẽ kêu bíp hai lần để thông báo thiết bị đang gia nhập mạng.
3. Nhấn giữ nút B2/B4 để điều khiển đóng/mở rèm thông qua mô phỏng trạng thái bật/tắt các led RGB:
   - Nhấn giữ nút B2 với thời gian T lớn hơn 500 ms để điều khiển bật LED_1 màu BLUE.
   - Nhấn giữ nút B4 với thời gian T lớn hơn 500 ms để điều khiển bật LED_2 màu RED.
   - Khi nhả nút B2/B4 sẽ điều khiển tắt màu LED tương ứng.
4. Nhấn nút B2/B4 hai lần sẽ điều khiển đóng/mở rèm thông qua trạng thái bật/tắt led RGB:
   - Nhấn nút B2 hai lần với timeout giữa hai lần nhấn là 400 ms sẽ điều khiển bật LED_1 màu BLUE.
   - Nhấn nút B4 hai lần với timeout giữa hai lần nhấn là 400 ms sẽ điều khiển bật LED_2 màu RED.
   - Nhấn nút B2/B4 một lần nữa sẽ điều khiển tắt led màu RGB tương ứng.

### Tại sao cần làm dự án này?
Mục tiêu giúp làm quen và hiểu được các vấn đề sau:
1. Biết cách cấu hình sử dụng ngắt và sử dụng _timer system tick_ của hệ thống.
2. Biết cách chia và vận dụng các hàm con (Functions).
3. Thông qua dự án, sẽ học được các thao tác cơ bản điều khiển các thiết bị thông qua ngoại vi GPIO của vi điều khiển STM32F4 sử dụng ngắt để thực hiện.

### Tài nguyên tham khảo:
Để xử lý dự án này, cần phải tìm hiểu tài liệu tham khảo về kiểm tra số lần nhấn nút, thời gian nhấn nút, sau đó mới thực hiện các yêu cầu điều khiển. Có thể tham khảo các tài liệu dưới đây để thực hiện dự án:
1. [Giới thiệu về System Tick Timer (SYSTICK)](https://drive.google.com/file/d/11LmuBh-ZCUstOoRlXmbT06krK1wsyQl_/view)
2. [Xử lý tín hiệu nhiễu ở đầu vào nút nhấn trên STM32](http://www.emcu.it/STM32/STM32Discovery-Debounce/STM32Discovery-InputWithDebounce_Output_UART_SPI_SysTick.html)
