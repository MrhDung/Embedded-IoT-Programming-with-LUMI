# IOT303x: Assignment 2

### Tên dự án: Giao tiếp với cảm biến nhiệt độ - độ ẩm và hiển thị các thông số lên màn hình LCD

### Tổng quan dự án:
Viết chương trình MCU giao tiếp với cảm biến nhiệt độ - độ ẩm và hiển thị các thông số nhiệt độ, độ ẩm lên màn hình hiển thị LCD đáp ứng các yêu cầu sau:
1. Thời gian lấy mẫu dữ liệu thô của cảm biến là 1 giây.
2. Xử lý dữ liệu thô của cảm biến bằng bộ lọc nhiễu (như bộ lọc Kalman...)
3. Khoảng thời gian cập nhật giá trị đo từ cảm biến (Interval period) có thể tùy chỉnh với giá trị mặc định là 5 giây.
4. Giá trị chênh lệch giữa hai lần đo gần nhất (Delta threshold) để cập nhật giá trị đo từ cảm biến có thể tùy chỉnh với giá trị mặc định đối với nhiệt độ là 2ºC và độ ẩm là 2%.

### Tại sao cần làm dự án này?
Mục tiêu giúp làm quen và hiểu được các vấn đề sau:
1. Biết cách giao tiếp với cảm biến thông qua giao tiếp I2C.
2. Biết cách giao tiếp với màn hình LCD thông qua giao tiếp SPI.
3. Biết cách chia và vận dụng các hàm con (Functions).
4. Thông qua dự án, sẽ học được các thao tác cơ bản của một hệ thống điều khiển bao gồm đo lường, xử lý tín hiệu từ cảm biến.

### Tài nguyên tham khảo:
Dựa án này, sẽ xử lý giao tiếp với màn hình LCD và đo đạc các thông số của cảm biến nhiệt độ, độ ẩm. Vì thế, để xử lý dự án này, cần phải tìm hiểu tài liệu tham khảo về cảm biến Si7020, thư viện để sử dụng giao tiếp với màn hình LCD. Có thể tham khảo các tài liệu dưới đây để thực hiện dự án:
1. [Tài liệu datasheet về cảm biến Si7020 - A20](https://www.silabs.com/documents/public/data-sheets/Si7020-A20.pdf)
2. [Tài liệu datasheet về LCD sử dụng chip ST7735](https://www.displayfuture.com/Display/datasheet/controller/ST7735.pdf)
3. [Tham khảo thư viện ucglib cho màn hình LCD](https://github.com/olikraus/ucglib/wiki/reference)
