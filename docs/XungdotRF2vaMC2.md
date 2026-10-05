## Yêu cầu xung đột giữa MC2 và RF2

### 1. Mô tả vấn đề xung đột

MC2 và RF2 có thể phát sinh xung đột khi cùng thao tác trên dữ liệu đặt xe nhưng có mục tiêu xử lý khác nhau. RF2 sử dụng cấu trúc dữ liệu **Binary Max-Heap** để ưu tiên xử lý các yêu cầu đặt thuê xe khi nhiều khách hàng cùng tranh chấp chiếc xe cuối cùng. Các yêu cầu được sắp xếp theo mức độ ưu tiên dựa trên những tiêu chí như hạng thành viên và thời điểm đặt xe.

Trong khi đó, nếu MC2 yêu cầu truy xuất, cập nhật hoặc sắp xếp danh sách đặt xe theo một tiêu chí khác, việc thay đổi dữ liệu có thể ảnh hưởng đến thứ tự ưu tiên mà RF2 đang duy trì trong Max-Heap.

### 2. Nguyên nhân xung đột

* **Khác biệt về tiêu chí sắp xếp:** MC2 và RF2 có thể sử dụng các tiêu chí khác nhau để truy xuất hoặc sắp xếp yêu cầu đặt xe. Thứ tự theo MC2 không nhất thiết trùng với thứ tự ưu tiên của Max-Heap trong RF2.
* **Xung đột khi cập nhật dữ liệu:** Khi thông tin đặt xe hoặc mức độ ưu tiên thay đổi, Max-Heap cần được điều chỉnh để bảo đảm phần tử có độ ưu tiên cao nhất luôn nằm ở gốc heap.
* **Ảnh hưởng đến tính nhất quán:** Nếu một yêu cầu đặt xe bị thêm, xóa hoặc thay đổi mà cấu trúc heap không được cập nhật tương ứng, hệ thống có thể xử lý sai thứ tự ưu tiên.

### 3. Hướng giải quyết

Hai yêu cầu nên được thiết kế để sử dụng chung nguồn dữ liệu nhưng đảm nhiệm các chức năng riêng biệt. MC2 thực hiện các thao tác theo mục tiêu của mình, còn RF2 sử dụng Max-Heap để lựa chọn yêu cầu có độ ưu tiên cao nhất. Khi dữ liệu liên quan đến RF2 thay đổi, hệ thống phải cập nhật heap tương ứng và bảo đảm các tiêu chí ưu tiên được áp dụng nhất quán.

Nếu cần sắp xếp toàn bộ danh sách để hiển thị hoặc báo cáo, hệ thống có thể tạo bản sao dữ liệu để sắp xếp thay vì trực tiếp thay đổi thứ tự nội bộ của heap.

### 4. Kết luận

Xung đột giữa MC2 và RF2 chủ yếu có thể nằm ở việc sử dụng và cập nhật dữ liệu đặt xe theo những mục tiêu khác nhau. Việc phân chia rõ trách nhiệm của từng yêu cầu, đồng bộ dữ liệu và duy trì đúng tính chất của Max-Heap giúp hạn chế xung đột, bảo đảm hệ thống xử lý yêu cầu đặt xe chính xác và nhất quán.
