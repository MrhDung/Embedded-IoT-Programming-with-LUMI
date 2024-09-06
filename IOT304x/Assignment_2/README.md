# IOT304x: Assignment 2

### Tên dự án: Kết hợp và sử dụng Command Line Interface để xử lý bài toán Binding, Groups trong Zigbee

### Tổng quan dự án:
Xây dựng một hệ thống Zigbee bao gồm: 01 Zigbee Coordinator, 02 thiết bị thuộc type HA On/Off Light - 1 endpoint: xử lý on/off.

Sử dụng CLI để đáp ứng các yêu cầu sau:
1. Join a network
   - Tạo mạng, gia nhập mạng giữa 2 thiết bị.
   - Điều khiển On/Off thiết bị từ ZC với: ON: Led 1 sáng Green, OFF: Led 1 tắt.
2. Binding
   - Cấu hình binding thiết bị 1 & 2 ( lựa chọn 1 trong 2 thiết bị làm Initiator).
   - ZC sẽ điều khiển thiết bị Initiator. Initiator sẽ xử lý bản tin để gửi cho Target.
3. Group
   - Cấu hình group cho thiết bị 1 & 2.
   - ZC sẽ sử dụng Multicast để gửi bản tin điều khiển, các thiết bị trong Group sẽ phải nhận & xử lý các bản tin giống nhau.
  
### Tại sao cần làm dự án này?
Mục tiêu giúp làm quen và hiểu được các vấn đề sau:
1. Hiểu và vận dụng Command Line Interface (CLI) vào các bài toán thực tế.
2. Hiểu được các chức năng như: tạo mạng, gia nhập mạng, điều khiển các thiết bị trong mạng...
3. Hiểu cách thức sử dụng của Binding, Groups...

### Tài nguyên tham khảo:
[Application Framework Command Line Interface (CLI)](https://docs.silabs.com/zigbee/6.4/af_v2/group-cli)
