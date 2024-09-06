# IOT303x: Assignment 4

### Tên dự án: Mô phỏng tính năng điều chỉnh độ sáng màn hình tự động trên điện thoại

### Tổng quan dự án:
Viết chương trình MCU điều khiển độ sáng của Led dựa vào giá trị cường độ ánh sáng đo được của cảm biến quang trở (hay còn gọi là Auto Brightness Led - ABL).

![image](https://github.com/user-attachments/assets/07398291-51c6-4dbc-b5ad-07a2f27b868d)

### Tại sao cần làm dự án này?
Mục tiêu giúp làm quen và hiểu được các vấn đề sau:
1. Biết cách giao tiếp với cảm biến với ngoại vi ADC của STM32F401.
2. Biết cách chia và vận dụng các hàm con (Functions).
3. Thông qua dự án, sẽ học được các thao tác cơ bản của một hệ thống điều khiển bao gồm đo lường, xử lý tín hiệu từ cảm biến.

### Tài nguyên tham khảo:
Dự án này sẽ xử lý giao tiếp với cảm biến quang trở và dựa vào giá trị cường độ ánh sáng đo được để điều khiển cường độ sáng của Led. Vì thế, để xử lý dự án này, cần tìm hiểu các kiến thức về ngoại vi ADC và TIMER, tham khảo các tài liệu về thuật toán lọc nhiễu (ví dụ: Kalman filter...) hay thuật toán điều chỉnh cường độ sáng tự động. Có thể tham khảo các tài liệu dưới đây để thực hiện dự án:
1. [Auto Brightness Led (ABL) - Maxim Integrated](https://www.analog.com/en/resources/design-notes/a-simple-implementation-of-lcd-brightness-control-using-the-max44009-ambientlight-sensor.html)
2. [Giới thiệu về bộ lọc Kalman](https://www.kalmanfilter.net/default.aspx)
3. [Tutorial về bộ lọc Kalman của đại học MIT](https://web.mit.edu/kirtley/kirtley/binlustuff/literature/control/Kalman%20filter.pdf)
4. [Source code về bộ lọc Kalman](https://github.com/HoangNH95/IOT-Programming-with-Master-Embedded/blob/master/Courseware%202/Kalman_filter.7z)
