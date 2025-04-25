# Đồ án cuối khóa - Lập trình nhúng IOT

## Đề tài: Xây dựng hệ thống IoT – Hệ thống cảm biến hiện diện cho nhà thông minh

## Giới thiệu đề tài
- Lý do chọn đề tài:
  + Hiện nay, nhà thông minh đang trở thành xu hướng phổ biến. Trong đó, cảm biến hiện diện đóng vai trò rất quan trọng giúp điều khiển thiết bị trong nhà thông minh, giúp tối ưu hóa hiệu quả tiêu thụ năng lượng và mang lại sự tiện nghi trong không gian sống.
  + Khắc phục một số hạn chế của cảm biến PIR (Không phát hiện được người đứng yên, không hoạt động tốt khi có vật cản, dễ bị nhiễu bởi các yếu tố môi trường…
- Giải pháp: Sử dụng cảm biến LD2410B (sử dụng công nghệ radar) kết hợp với mạng không dây Zigbee.

## Mục tiêu đề tài
- Xây dựng hệ thống cảm biến hiện diện IoT cho nhà thông minh.
- Tự động hóa điều khiển thiết bị điện dựa trên sự hiện diện.
- Ứng dụng công nghệ Zigbee để kết nối các thiết bị không dây.
- Tích hợp vi điều khiển STM32 (có các ngoại vi như: LCD, nút nhấn,..) để điều khiển và hiển thị thông tin.

## Các thiết bị sử dụng trong mô hình
- Thiết bị STM32 với module LCD đóng vài trò làm giao diện điều khiển và hiển thị.
  <p align="left">
    <img src="https://github.com/user-attachments/assets/e781d0e9-b479-4dc4-b8f0-d522f2aefd3f" alt="image" width="250">
  </p>


- 03 thiết bị EFR32MG21, trong đó:
  + 01 thiết bị làm Coordinator đóng vai trò thiết lập mạng và quản lý các thiết bị trong mạng.
  + 01 thiết bị mô phỏng hệ thống đèn.
  + 01 thiết bị mô phỏng hệ thống cảm biến.
    <p align="left">
      <img src="https://github.com/user-attachments/assets/7b98c41e-f0ec-4d57-b7ba-33e1142e6ce4" alt="image" width="180">
    </p>

- Cảm biến hiện diện LD2410B
  <p align="left">
    <img src="https://github.com/user-attachments/assets/f21dc068-c0b2-4c5d-8435-98735b42ee28" alt="image" width="250">
  </p>

## Mô hình hệ thống
  <p align="left">
    <img src="https://github.com/user-attachments/assets/c3489e9c-b1be-4367-894a-c754ffbb4d9d" alt="image" width="980">
  </p>

## Chức năng chính
- Phát hiện sự hiện diện của con người trong không gian.
- Tự động bật/tắt đèn theo trạng thái hiện diện.
- Cấu hình thông số cảm biến qua giao diện trên LCD (STM32).

## Nguyên lý hoạt động
- Hệ thống cảm biến thu thập các thông số môi trường (nhiệt độ, độ ẩm, cường độ ánh sáng) và phát hiện sự hiện diện của con người, sau đó truyền dữ liệu đến Coordinator và điều khiển On/Off thiết bị LED (ở chế độ Binding) qua Zigbee.
- Coordinator tiếp nhận dữ liệu cảm biến, quản lý trạng thái mạng, trạng thái kết nối và hoạt động của các thiết bị, rồi gửi thông tin này đến STM32 để hiển thị trên LCD.
- STM32 hiển thị dữ liệu, gửi lệnh điều khiển và thiết lập thông số cảm biến đến Coordinator qua UART. Coordinator tiếp tục chuyển tiếp các lệnh này đến các thiết bị đầu cuối thông qua Zigbee.

## Kết quả đạt được
- Hệ thống hoạt động ổn định theo mô hình đã đề ra.
- Cảm biến LD2410B phát hiện chính xác sự hiện diện, chuyển động và hoạt động chính xác theo các thông số được cài đặt.
- Đèn bật/tắt tự động theo trạng thái hiện diện.
- STM32 hiển thị chính xác các thông số theo thiết kế.

## Video giới thiệu và demo hệ thống
https://drive.google.com/file/d/1qT96-juyn1l97V1HrfTaL7KQz0rTjUvF/view

## Hướng phát triển
- Kết hợp với vi điều khiển ESP32 nhằm bổ sung khả năng hiển thị thông tin, trạng thái và điều khiển thiết bị thông qua Web Server.
- Điều khiển và giám sát hệ thống thông qua ứng dụng di động.
