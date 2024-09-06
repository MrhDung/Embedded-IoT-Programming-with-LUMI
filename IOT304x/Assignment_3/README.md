# IOT304x: Assignment 3

### Tên dự án: Xây dựng thiết bị công tắc 2 nút hoạt động với tính năng hoàn chỉnh, tích hợp vào hệ thống Lumi Life

### Tổng quan dự án:
Xây dựng chương trình gồm 02 thiết bị với các endpoint sau đây:
- Endpoint 1: HA On/Off Light
- Endpoint 2: HA On/Off Light
- Endpoint 3: HA Light Sensor

Đáp ứng các yêu cầu sau:
1. Device Type: Zigbee Router
2. Model ID: SW2_LM1
3. Khi khởi động kiểm tra mạng:
   - Nếu thiết bị đã gia nhập mạng sẽ nháy hồng (màu Red + màu Blue) 03 lần với chu kỳ 300ms và gửi Model ID cho thiết bị Zigbee Coordinator.
   - Nếu thiết bị chưa gia nhập mạng sẽ nháy màu đỏ 03 lần với chu kỳ 300ms.
4. Khi gia nhập mạng thành công:
   - LED 1 sẽ nháy màu hồng (màu Red + màu Blue) 03 lần với chu kỳ 300ms.
   - Gửi Model ID cho thiết bị Zigbee Coordinator.
5. Điều khiển trạng thái của các endpoint (Trường hợp không có Binding):
   - Nhấn nút SW1 01 lần sẽ điều khiển Led 1 sáng màu Blue và gửi phản hồi trạng thái bật "ON" của endpoint 1 cho thiết bị Zigbee Coordinator.
   - Nhấn nút SW1 02 lần sẽ điều khiển Led 1 tắt và gửi phản hồi trạng thái tắt "OFF" của endpoint 1 cho thiết bị Zigbee Coordinator.
   - Nhấn nút SW2 01 lần sẽ điều khiển Led 2 sáng màu Blue và gửi phản hồi trạng thái bật "ON" của endpoint 2 cho thiết bị Zigbee Coordinator.
   - Nhấn nút SW2 02 lần sẽ điều khiển Led 2 tắt và gửi phản hồi trạng thái tắt "OFF" của endpoint 2 cho thiết bị Zigbee Coordinator.
6. Khi nhận lệnh điều khiển từ HC đến 1 trong 2 thiết bị:
   - Nếu là endpoint 1 thì thực hiện điều khiển bật/tắt trạng thái LED 1 màu Blue ở thiết bị nhận được.
   - Nếu là endpoint 2 thì thực hiện điều khiển bật/tắt trạng thái LED 2 màu Blue ở thiết bị nhận được.
   - Phản hồi trạng thái bật/tắt về thiết bị Zigbee Coordinator.
7. Thực hiện xử lý cấu hình Binding cho thiết bị
   - Bấm 3 lần nút SW1: khởi chạy Target qua API - `emberAfPluginFindAndBindTargetStart(uint8_t endpoint)`
   - Bấm 4 lần nút SW1: khởi chạy Initiator qua API - `emberAfPluginFindAndBindInitiatorStart(uint8_t endpoint)`
8. Thực hiện xử lý các lệnh điều khiển (Trường hợp sử dụng Binding)
   - Lưu ý rằng, lệnh điều khiển đến 1 trong 2 thiết bị, thì thiết bị còn lại cũng nhận được lệnh xử lý tương tự. Vì vậy, cần phải cấu hình Binding 2 chiều cho cả 2 thiết bị (vừa làm Inititator, vừa làm Target).
   - Việc xử lý bản tin trong trường hợp này cũng phải xử lý cẩn thận, nếu không 1 bản tin sẽ được gửi qua lại giữa 2 thiết bị liên tục
9. Mỗi 1 phút, nếu cường độ ánh sáng:
   - Thay đổi so với lần cập nhật dữ liệu trước > 30 đơn vị lux thì gửi dữ liệu ánh sáng lên thiết bị Zigbee Coordinator.
   - Nếu cường độ ánh sáng ở thiết bị > 500 lux: LED 2 sáng xanh green (xử lý độc lập ở mỗi thiết bị).
   - Nếu cường độ ánh sáng ở thiết bị < 500 lux: LED 2 tắt (xử lý độc lập ở mỗi thiết bị).
10. Thực hiện reset khi:
    - Nhấn nút SW1 05 lần
    - Nhận ZDO Leave từ ZC (Xóa ra khỏi app)

_Lưu ý: Khi thực hiện reset, cần gửi bản tin phản hồi ZDO Leave Response cho thiết bị Zigbee Coordinator._

### Tại sao cần làm dự án này?
Mục tiêu giúp làm quen và hiểu được các vấn đề sau:
1. Biết cách xây dựng một thiết bị hoàn chỉnh và tích hợp vào hệ thống Lumi Life.
2. Biết cách xử lý các bài toán điều khiển trong thực tế.

### Tài nguyên tham khảo:
1. [Tải phần mềm Lumi Life PC](https://files.lumi.com.vn/s/P384GLPiMW5cAdT)
2. [Application Framework Command Line Interface (CLI)](https://docs.silabs.com/zigbee/6.4/af_v2/group-cli)













