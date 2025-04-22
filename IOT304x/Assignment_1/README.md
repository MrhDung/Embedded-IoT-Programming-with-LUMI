# IOT304x: Assignment 1

### Tên dự án: Giao tiếp với các cảm biến môi trường và hiển thị các thông số lên màn hình PC

### Tổng quan dự án:
Xây dựng chương trình MCU để giao tiếp với các cảm biến môi trường, hiển thị các kết quả đo cường độ ánh sáng, nhiệt độ và độ ẩm lên màn hình PC thông qua giao thức UART, và đáp ứng được các yêu cầu như sau:
- Chu kỳ trích mẫu dữ liệu cảm biến là 1 giây (s).
- Xử lý dữ liệu thô của cảm biến bằng bộ lọc nhiễu (ví dụ: Kalman filter...)
- Chu kỳ cập nhật giá trị đo từ cảm biến (interval period) được mặc định là 5 giây (có thể tùy chỉnh giá trị này).
- Kết quả đo từ cảm biến được cập nhật nếu sự chênh lệch kết quả giữa hai phép đo gần nhất vượt ngưỡng quy định (delta-threshold). Ngưỡng này được mặc định là 2 độ C với phép đo nhiệt độ, và 2% với phép đo độ ẩm, 10 lux đối với phép đo ánh sáng (có thể tùy chỉnh ngưỡng quy định).
- Sử dụng giao thức UART, với tools hỗ trợ là Hercules (Windows), Cutecom (Ubuntu) để hiển thị các thông số.

### Tại sao cần làm dự án này?
Mục tiêu giúp làm quen và hiểu được các vấn đề sau:
1. Làm quen với module EFR32MG21 và IDE Simplicity Studio.
2. Biết đến cách thức Debug Printing, hiển thị log hoạt động thiết bị.
3. Hiểu và vận dụng các kiến thức xử lý giao tiếp với cảm biến.

### Tài nguyên tham khảo:
Dự án này, sẽ xử lý giao tiếp với hai loại cảm biến là cảm biến ánh sáng sử dụng quang điện trở LDR và cảm biến nhiệt độ - độ ẩm sử dụng chip Si7020 để hiển thị các thông số môi trường qua màn hình PC sử dụng giao thức truyền thông UART. Vì thế để xử lý dự án này, cần phải tìm hiểu tài liệu tham khảo về cảm biến LDR, Si7020, và tải phần mềm Hercules (Windows), Cutecom (Ubuntu) để thực hiện việc in các dòng log hiển thị. Có thể tham khảo các tài liệu dưới đây để thực hiện sự án:
1. [Tài liệu datasheet về cảm biến Si7020](https://www.silabs.com/documents/public/data-sheets/Si7020-A20.pdf)
2. [Tài liệu datasheet về cảm biến LDR](https://www.kth.se/social/files/54ef17dbf27654753f437c56/GL5537.pdf)
3. [Phần mềm Hercules (Windows)](https://www.hw-group.com/software/hercules-setup-utility)
4. [Phần mềm Cutecom (Ubuntu)](https://cutecom.sourceforge.net/)
