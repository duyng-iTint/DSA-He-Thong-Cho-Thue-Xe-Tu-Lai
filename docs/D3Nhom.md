BÁO CÁO THIẾT KẾ & BIỆN MINH LỰA CHỌN (D3)
Học phần 261DASA230179_06 Đồ án nhóm 608
Hệ thống quản lý cho thuê xe tự lái và xử lý tranh chấp đặt xe
Mã nhóm: 608

| Họ tên | MSSV | Thành phần sở hữu |
| --- | --- | --- |
| Nguyễn Hữu Thịnh | 25110349 | MC1: bảng băm tự cài đặt, Persistence CSV |
| Nguyễn Minh Duy | 25110167 | MC2: Merge Sort, Binary Search, thống kê top xe |
| Lê Đức Thuần | 25110352 | RF1: gợi ý tên xe (Trie) |
| Lý Nguyễn Mạnh Thuyên | 25110354 | RF2: ưu tiên khi tranh xe (Max-Heap) |
| Lê Nguyễn Minh Thư | 25110356 | RF3: hoàn tác (ngăn xếp) |

## 1. Tóm tắt bài toán và yêu cầu
Nền tảng cho thuê xe tự lái tích lũy hàng chục nghìn đơn thuê, dữ liệu thay đổi liên tục (thêm, sửa, hủy). Nhân viên cần trả lời nhanh một số câu hỏi cố định dù dữ liệu lớn dần, và cần quy tắc công bằng khi nhiều khách tranh một xe. Quy mô thiết kế: từ 10.000 đơn, đo thực nghiệm đến 100.000 đơn. Ràng buộc: tra cứu tức thì, dữ liệu lưu bền vững qua các lần chạy, một xe không giao cho hai khách trùng khoảng ngày.

| Mã | Yêu cầu | Người phân tích |
| --- | --- | --- |
| MC1 | Tra cứu chính xác một đơn theo mã đơn (đúng 1 đơn) hoặc theo biển số (mọi đơn của xe), ở quy mô mà duyệt tuần tự rõ ràng chậm; tra cứu phải luôn đúng sau khi thêm, sửa, hủy | Nguyễn Hữu Thịnh |
| MC2 | (a) Xem mọi đơn trong một khoảng ngày; (b) xem top K xe được thuê nhiều nhất, xếp giảm dần | Nguyễn Minh Duy |
| Yêu cầu riêng 1 | Gợi ý tên xe theo vài ký tự đầu | Lê Đức Thuần |
| Yêu cầu riêng 2 | Lấy ra yêu cầu tranh xe quan trọng nhất (hạng cao hơn, bằng nhau thì đặt sớm hơn), lặp lại trên tập đang thay đổi và có yêu cầu bị hủy giữa chừng | Lý Nguyễn Mạnh Thuyên |
| Yêu cầu riêng 3 | Hoàn tác thao tác gần nhất (làm sau hoàn tác trước) | Lê Nguyễn Minh Thư |

## 2. Bảng so sánh các cấu trúc dữ liệu
Bảng ở trang sau (khổ ngang). N là số bản ghi, k là số kết quả trả về, L là độ dài khóa.

| Cấu trúc | Tìm theo khóa | Tìm theo vị trí | Thêm / xóa | Duyệt có thứ tự | Khoảng / lân cận | Cực trị | Xấu nhất vs trung bình |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Mảng động | O(N) | O(1) | Cuối O(1) khấu hao; giữa O(N) | Không có thứ tự sẵn; sắp O(N log N) | O(N) quét toàn bộ | O(N) | Tìm O(N) mọi trường hợp |
| Mảng đã sắp xếp | O(log N) theo khóa sắp | O(1) | O(N) do dịch phần tử | O(N), có sẵn thứ tự | O(log N + k) | O(1) ở hai đầu | O(log N) ổn định; chuẩn bị O(N log N) |
| Danh sách liên kết | O(N) | O(N) | O(1) khi đã có con trỏ nút | Theo thứ tự chèn, O(N) | O(N) | O(N) | Không phụ thuộc dữ liệu |
| Stack / Queue / Deque | Không hỗ trợ | Chỉ đầu hoặc cuối | O(1) ở đầu hoặc cuối | Chỉ LIFO hoặc FIFO | Không | Chỉ đầu hoặc cuối, không theo giá trị | O(1) mọi trường hợp |
| Bảng băm | O(1) trung bình | Không có | O(1) trung bình (rehash khấu hao) | Không | Không | O(N) | Trung bình O(1), xấu nhất O(N) |
| Cây tìm kiếm cân bằng | O(log N) | Không tự nhiên | O(log N) | O(N) duyệt giữa (in-order) | O(log N + k) | O(log N) | O(log N) được đảm bảo |
| Heap nhị phân | O(N) (cần bảng phụ để O(1)) | Không có | Thêm và lấy gốc O(log N) | Không (lấy lần lượt O(N log N)) | Không | Xem O(1), lấy O(log N) | O(log N) được đảm bảo |
| Trie | O(L) | Không có | O(L) | Theo thứ tự ký tự | Theo tiền tố: O(L + k) | Không | O(L), không phụ thuộc N; tốn bộ nhớ |

Nhận xét theo lĩnh vực: MC1 chỉ cần dòng "Tìm theo khóa" (bảng băm thắng); MC2 cần "Khoảng" và "Duyệt có thứ tự" (mảng sắp xếp hoặc cây cân bằng); RF2 cần "Cực trị" với thêm và hủy liên tục (heap); RF1 cần "Khoảng theo tiền tố" (Trie); RF3 chỉ thao tác ở một đầu (ngăn xếp).
## 3. Quy trình Q1–Q4 cho từng yêu cầu
### 3.1. MC1: tra cứu chính xác

| Q1. Có cần thứ tự không? | Không. Tra theo mã đơn hoặc biển số trả về đúng đơn cần tìm, không phụ thuộc thứ tự thời gian hay xếp hạng. |
| --- | --- |
| Q2. Loại khóa và khối lượng thao tác | Khóa là chuỗi (mã đơn dạng RENT_HCM_000123, biển số dạng 51F-123.45), tra chính xác. Mã đơn duy nhất (quan hệ 1-1), còn biển số lặp lại ở nhiều đơn (quan hệ 1-N) nên chỉ mục theo biển số phải lưu danh sách đơn. Đọc nhiều, ghi lẻ (thêm, sửa, hủy), từ 10.000 đơn trở lên. |
| Q3. Có chấp nhận trường hợp xấu nhất không? | Có. Trường hợp xấu nhất O(N) chỉ xảy ra khi nhiều khóa dồn vào một ô, rất hiếm với hàm băm đa thức và cơ chế tự mở rộng khi hệ số tải vượt 0,75. Đo thực tế ở 10.000 đơn: hệ số tải 0,62, chuỗi dài nhất 4. Hệ thống không thuộc loại thời gian thực nên không đòi bảo đảm cứng. |
| Q4. Yếu tố phi tiệm cận | Nối chuỗi (separate chaining) xóa phần tử đơn giản, không cần đánh dấu xóa như địa chỉ mở; hằng số nhỏ; tốn bộ nhớ khoảng 1,6 ô cho mỗi khóa ở hệ số tải 0,62; mở rộng bảng tốn O(N) nhưng khấu hao O(1) mỗi lần thêm. |
| Cấu trúc được chọn | Bảng băm tự cài đặt (nối chuỗi, hàm băm đa thức hệ số 31, tự mở rộng khi hệ số tải > 0,75): chỉ mục mã đơn → đơn và chỉ mục biển số → danh sách đơn. [Đối chiếu với code đang nộp: bản demo_mc1 hiện ghi đè khi trùng biển số] |
| Đánh đổi đã chấp nhận | Mất khả năng duyệt theo thứ tự, theo khoảng, theo tiền tố trên khóa. Phải giữ hai chỉ mục đồng bộ nên mỗi lần thêm, sửa biển số, hủy đều cập nhật cả hai. Lý thuyết xấu nhất O(N). Một lần mở rộng bảng gây chậm O(N). |
| Điều kiện làm lựa chọn không còn đúng | Cần tra theo khoảng hoặc tiền tố mã (khi đó dùng cây cân bằng hoặc Trie); cần bảo đảm xấu nhất (thời gian thực) thì dùng cây cân bằng; khóa phân bố lệch hoặc có thể bị cố ý gây va chạm; dữ liệu chỉ vài trăm đơn thì duyệt tuần tự đã đủ nhanh và đơn giản hơn. |
| Người viết | Nguyễn Hữu Thịnh |

### 3.2. MC2: khoảng ngày và top K xe

| Q1. Có cần thứ tự không? | Có. Khoảng ngày cần các đơn xếp theo ngày; top K cần xếp giảm dần theo số lần thuê. |
| --- | --- |
| Q2. Loại khóa và khối lượng thao tác | Khóa là ngày dạng YYYY-MM-DD (so sánh chuỗi bằng so sánh thời gian). Dữ liệu lịch sử nạp hàng loạt một lần rồi truy vấn nhiều lần; đơn đã xác nhận ít bị thay đổi. |
| Q3. Có chấp nhận trường hợp xấu nhất không? | Không muốn trường hợp xấu. Merge Sort luôn O(N log N), Binary Search luôn O(log N), không có trường hợp suy biến như quicksort O(N²). Chi phí chèn O(N) chấp nhận được vì ít chèn. |
| Q4. Yếu tố phi tiệm cận | Mảng liền khối gọn bộ nhớ, quét k kết quả theo vị trí liên tiếp rất nhanh; Merge Sort ổn định nên khi trùng ngày vẫn giữ thứ tự theo mã đơn; cần thêm O(N) bộ nhớ phụ khi trộn. |
| Cấu trúc được chọn | Mảng sắp xếp theo ngày bắt đầu, nạp hàng loạt bằng Merge Sort; truy vấn bằng Binary Search (lowerBound và upperBound) cho O(log N + k). Top K: sắp theo biển số rồi quét một lần để đếm lượt thuê, sau đó Merge Sort giảm dần theo số lần thuê. |
| Đánh đổi đã chấp nhận | Chèn hoặc xóa đơn giữa mảng tốn O(N). Truy vấn khoảng hiện chỉ xét ngày bắt đầu thuê, chưa tính đơn bắt đầu trước khoảng nhưng kết thúc trong khoảng. Mỗi lần xem top K phải dựng lại thống kê O(N log N). |
| Điều kiện làm lựa chọn không còn đúng | Đơn mới hoặc đơn hủy xuất hiện dày so với số lần truy vấn (khi đó dùng cây cân bằng, chèn O(log N)); cần biết xe nào đang bận theo đơn giao với khoảng (cần lưu thêm ngày kết thúc); chỉ cần top K rất nhỏ (heap kích thước K cho O(N log K)). |
| Người viết | Nguyễn Minh Duy |

### 3.3. Yêu cầu riêng 1: gợi ý theo tiền tố

| Q1. Có cần thứ tự không? | Có. Kết quả autocomplete cần có thứ tự ổn định để giao diện hiển thị nhất quán. Trong implementation, các node con của Trie được lưu bằng unordered_map, nên thứ tự duyệt cây ban đầu không được đảm bảo. Vì vậy sau khi thu thập kết quả, hàm getAllSuggestions() sử dụng sort(results.begin(), results.end()) để sắp xếp danh sách theo thứ tự từ điển A-Z. Điều này được kiểm tra trực tiếp trong TrieTest.cpp: Toyota Innova phải đứng trước Toyota Vios. |
| --- | --- |
| Q2. Loại khóa và khối lượng thao tác | Khóa là chuỗi ngắn, gồm tên hãng và tên dòng xe. Danh mục xe tương đối nhỏ và ít thay đổi. Truy vấn được thực hiện lặp lại nhiều lần khi người dùng gõ từng ký tự, ví dụ t → to → toy. Input có thể viết hoa hoặc viết thường nên cần chuẩn hóa trước khi tìm kiếm. |
| Q3. Có chấp nhận trường hợp xấu nhất không? | Có. Chi phí đi từ root đến node đại diện cho prefix là O(L), với L là độ dài prefix. Sau đó cần duyệt cây con để thu thập các gợi ý, nên tổng chi phí còn phụ thuộc vào số lượng node/kết quả cần duyệt. Vì autocomplete phải trả về danh sách kết quả nên chi phí duyệt kết quả là cần thiết. . |
| Q4. Yếu tố phi tiệm cận | Mỗi node sử dụng unordered_map<char, TrieNode*> để lưu các node con nên có overhead bộ nhớ lớn hơn cách lưu một mảng cố định. Tuy nhiên danh mục xe không quá lớn nên mức sử dụng bộ nhớ này có thể chấp nhận được. Implementation cũng nạp cả cụm tên đầy đủ và các từ khóa dòng xe để người dùng có thể nhập vios và nhận Toyota Vios. . |
| Cấu trúc được chọn | Trie (cây tiền tố). Dữ liệu được tổ chức theo từng ký tự của chuỗi. CarTrie sử dụng normalize() để chuẩn hóa chữ thường và loại bỏ khoảng trắng đầu/cuối. Mỗi node có matchedFullCarNames để lưu tên xe đầy đủ kết thúc tại node đó. |
| Đánh đổi đã chấp nhận | Trie sử dụng nhiều node và con trỏ nên tốn bộ nhớ hơn một danh sách/mảng đơn giản. Việc lưu thêm các khóa dòng xe làm tăng số lượng đường đi trong Trie. unordered_map giúp chỉ tạo các nhánh ký tự thực sự xuất hiện nhưng có overhead quản lý bảng băm. Implementation hiện tại cũng chưa xử lý riêng trường hợp tiếng Việt có dấu. |
| Điều kiện làm lựa chọn không còn đúng | Nếu danh mục chỉ có vài chục tên và số lần autocomplete rất ít, duyệt tuyến tính và so prefix có thể đã đủ đơn giản. Nếu yêu cầu sắp gợi ý theo độ phổ biến thì Trie hiện tại chưa có thông tin về số lượt sử dụng và cần kết hợp thêm dữ liệu đếm/ranking. Nếu cần tìm chuỗi ở giữa tên hoặc hỗ trợ sai chính tả thì Trie theo prefix hiện tại không đáp ứng trực tiếp. |
| Người viết | Lê Đức Thuần |

### 3.4. Yêu cầu riêng 2: ưu tiên khi tranh xe

| Q1. Có cần thứ tự không? | Chỉ một phần: cần luôn lấy ra yêu cầu tốt nhất, không cần sắp toàn bộ hàng chờ. |
| --- | --- |
| Q2. Loại khóa và khối lượng thao tác | Khóa tổng hợp: hạng thành viên giảm dần, thời điểm đặt tăng dần, rồi mã đơn tăng dần (để hòa vẫn xác định). Yêu cầu đến liên tục, bị hủy giữa chừng; mỗi lần chỉ lấy ra một phần tử. |
| Q3. Có chấp nhận trường hợp xấu nhất không? | Không với thao tác lấy ra và hủy ở giờ cao điểm: cần O(log N) được đảm bảo thay vì O(N) mỗi lần. |
| Q4. Yếu tố phi tiệm cận | Heap lưu trong mảng liền khối, bộ nhớ gọn. Muốn hủy một yêu cầu bất kỳ trong O(log N) cần thêm bảng mã đơn → vị trí, và phải cập nhật bảng này mỗi lần hoán đổi. |
| Cấu trúc được chọn | Max-Heap nhị phân trong mảng, kèm bảng chỉ mục mã đơn → vị trí; luật so sánh ba tầng (hạng, thời điểm đặt, mã đơn). |
| Đánh đổi đã chấp nhận | Không xem được toàn bộ hàng chờ theo thứ tự (lấy lần lượt tốn O(N log N)); phải giữ đồng bộ bảng phụ; chỉ tra theo mã đơn, không theo khóa khác. |
| Điều kiện làm lựa chọn không còn đúng | Cần xem toàn bộ hàng chờ theo thứ tự ưu tiên hoặc truy vấn theo khoảng thì dùng cây cân bằng; hàng chờ rất nhỏ thì duyệt tìm lớn nhất đã đủ; ưu tiên thay đổi theo thời gian chờ thì phải sửa khóa và dịch chuyển phần tử trong heap. |
| Người viết | Lý Nguyễn Mạnh Thuyên |

### 3.5. Yêu cầu riêng 3: hoàn tác

| Q1. Có cần thứ tự không? | Có, theo thời gian thao tác: làm sau hoàn tác trước (LIFO). |
| --- | --- |
| Q2. Loại khóa và khối lượng thao tác | Chỉ thao tác ở đỉnh: ghi một bản ghi mỗi lần thêm, sửa, xóa; lấy ra khi hoàn tác. Không tra theo khóa. |
| Q3. Có chấp nhận trường hợp xấu nhất không? | Không có trường hợp xấu: ghi, lấy, xem đỉnh đều O(1). |
| Q4. Yếu tố phi tiệm cận | Danh sách liên kết đơn không cần dịch hay cấp phát lại, bộ nhớ tăng theo số thao tác. Mỗi bản ghi lưu dữ liệu cũ, dữ liệu mới và vị trí để khôi phục chính xác (xóa thì chèn lại đúng vị trí). |
| Cấu trúc được chọn | Ngăn xếp cài bằng danh sách liên kết đơn. |
| Đánh đổi đã chấp nhận | Chỉ hoàn tác theo thứ tự ngược, không hoàn tác chọn lọc; chưa có làm lại (redo); lịch sử tăng không giới hạn; mỗi bản ghi sao chép dữ liệu đơn. |
| Điều kiện làm lựa chọn không còn đúng | Cần hoàn tác chọn lọc hoặc xem lịch sử thì dùng danh sách liên kết đôi hoặc deque; cần giới hạn bộ nhớ thì dùng deque có giới hạn; cần làm lại thì dùng hai ngăn xếp. |
| Người viết | Lê Nguyễn Minh Thư |

## 4. Yêu cầu xung đột và cách giải quyết

| Hai yêu cầu xung đột | MC2 (khoảng ngày theo ngày thuê) và yêu cầu riêng 2 (ưu tiên khi tranh xe). |
| --- | --- |
| Vì sao không cấu trúc đơn nào đáp ứng cả hai | Cả hai cần "thứ tự" nhưng theo hai khóa khác nhau và kiểu thao tác khác nhau. MC2 cần duyệt và lấy khoảng theo ngày; ưu tiên cần lấy phần tử lớn nhất theo (hạng, thời điểm) lặp lại trên tập có yêu cầu đến và bị hủy liên tục. Mảng sắp xếp theo ngày cho khoảng O(log N + k) nhưng lấy lớn nhất theo khóa khác phải quét O(N), và chèn hoặc hủy tốn O(N). Heap cho lấy và hủy O(log N) nhưng không có khoảng hay duyệt có thứ tự. Một cây cân bằng chỉ sắp theo một khóa; muốn cả hai khóa vẫn phải dùng hai cây, tức là kết hợp. |
| Hướng giải quyết | Kết hợp hai cấu trúc, mỗi cái giữ phần mình làm tốt: Max-Heap cho M yêu cầu đang chờ (tập nhỏ, biến động liên tục); mảng sắp xếp theo ngày cho N đơn lịch sử đã xác nhận (tập lớn, ít thay đổi). |
| Cách giữ đồng bộ khi thêm, sửa, hủy, hoàn tác | Yêu cầu mới vào heap. Khi một yêu cầu được xử lý, lấy ra khỏi heap, tạo đơn rồi thêm vào bảng băm (MC1) và mảng sắp xếp (MC2). Hủy yêu cầu đang chờ: xóa khỏi heap theo mã đơn. Hủy hoặc sửa đơn đã xác nhận: cập nhật bảng băm và mảng sắp xếp. Hoàn tác đảo ngược đúng các bước đó. [Nhóm thống nhất lại luồng này và đối chiếu với code tích hợp] |
| Chi phí phải trả | Dữ liệu một đơn xuất hiện ở nhiều chỉ mục nên tốn thêm bộ nhớ và mỗi thao tác ghi phải cập nhật đủ chỉ mục; xóa khỏi mảng sắp xếp tốn O(N); nếu quên cập nhật một chỉ mục sẽ có dữ liệu lệch. |

## 5. Phương án bị loại

| Yêu cầu | Phương án bị loại | Lý do cụ thể | Khi nào nó thắng |
| --- | --- | --- | --- |
| MC1 | Duyệt tuyến tính trên mảng | Mỗi lần tra O(N), thời gian tăng tỉ lệ với số đơn; benchmark ở 1.000, 10.000, 100.000 đơn dùng để chứng minh. | Dữ liệu chỉ vài trăm đơn |
| MC1 | Mảng sắp xếp theo mã đơn + Binary Search | Tra O(log N) nhưng mỗi lần thêm đơn phải dịch O(N); dữ liệu biến động liên tục nên chậm, trong khi MC1 không cần thứ tự. | Dữ liệu gần như chỉ đọc |
| MC1 | Cây tìm kiếm cân bằng | Bảo đảm O(log N) nhưng MC1 không cần duyệt thứ tự hay khoảng, nên trả hằng số lớn hơn và cài đặt phức tạp hơn mà không có lợi. | Cần tra theo khoảng hoặc tiền tố mã, hoặc cần bảo đảm xấu nhất |
| MC2 | Bảng băm | Không duyệt được theo khoảng; muốn lấy khoảng phải lọc toàn bộ O(N) hoặc sắp lại O(N log N) mỗi truy vấn. | Chỉ tra theo khóa |
| MC2 | Cây tìm kiếm cân bằng | Chèn O(log N) tốt hơn mảng nhưng dữ liệu lịch sử ít đổi, mảng liền khối nhanh hơn khi quét k kết quả và đơn giản hơn để cài đặt. | Đơn mới và đơn hủy xuất hiện dày |
| RF1 | Mảng sắp xếp + Binary Search theo tiền tố | Làm được (O(log N + k)) nhưng mỗi ký tự gõ thêm phải tìm lại từ đầu, và thêm hậu tố dòng xe làm mảng phải sắp lại. | Danh mục cố định và bộ nhớ hạn chế |
| RF2 | Mảng sắp xếp theo ưu tiên | Chèn và hủy O(N) mỗi lần trong khi yêu cầu đến và bị hủy liên tục ở giờ cao điểm. | Hàng chờ nhỏ, cần xem toàn bộ theo thứ tự |
| RF3 | Mảng động làm ngăn xếp | Cũng cho đẩy, lấy O(1) khấu hao nhưng thỉnh thoảng phải sao chép cả mảng khi tăng dung lượng; danh sách liên kết cho O(1) thật. | Cần quét liên tục và số thao tác rất lớn |

## 6. Kiến trúc 3 tầng

| Tầng | Thành phần | Yêu cầu xử lý ở tầng này |
| --- | --- | --- |
| Presentation | demo_mc1.cpp, mainMC1.cpp, mainRF1.cpp, MainRF3.cpp, InputHandler.* | Chỉ nhận đầu vào và hiển thị |
| DSA Core | MyHashTable.h, MergeSort.h, BinarySearch.h, RentalService.*, Trie.h, MyMaxHeap.h, UndoStack.*, RentalSystem.* | MC1, MC2, RF1, RF2, RF3 chạy trên cấu trúc trong bộ nhớ |
| Persistence | CsvCodec.h, Persistence.*, CSVUtils.* | Chỉ nạp và ghi CSV, không tra cứu hay sắp xếp |

## 7. Bằng chứng hiệu năng
Thống kê bảng băm MC1 đo được (theo số đơn) và thời gian truy vấn (điền từ BenchmarkMC1.exe):
N ban ghi | HashTable (ms) | Linear Scan (ms) | Speedup
-----------------------------------------------------------------
1000 | 0.00008 | 0.00647 | 84.4x
(thong ke bang bam: capacity=2019, size=1000, load_factor=0.495, max_chain_length=3)
10000 | 0.00014 | 0.06331 | 453.5x
(thong ke bang bam: capacity=16159, size=10000, load_factor=0.619, max_chain_length=4)
100000 | 0.00032 | 0.85881 | 2647.4x
(thong ke bang bam: capacity=258559, size=100000, load_factor=0.387, max_chain_length=4)
MC2
===== BENCHMARK MC2 =====
Sorted Array + Binary Search VS Linear Scan
=============================
So luong ban ghi: 1000
Binary Search: 51.70 ns/query
Linear Scan: 756.70 ns/query
Speedup: 14.64x
So ket qua trong khoang: 101
-----------------------------
So luong ban ghi: 10000
Binary Search: 59.20 ns/query
Linear Scan: 7057.00 ns/query
Speedup: 119.21x
So ket qua trong khoang: 101
-----------------------------
So luong ban ghi: 100000
Binary Search: 113.20 ns/query
Linear Scan: 68846.80 ns/query
Speedup: 608.19x
So ket qua trong khoang: 101
-------------------------
## 8. Kỹ thuật không áp dụng
Merge Sort (chia để trị) và Binary Search được dùng ở MC2 để nạp hàng loạt và truy vấn khoảng. Quicksort không dùng vì Merge Sort ổn định và luôn O(N log N). Union-Find không dùng vì không có yêu cầu "cùng nhóm hay không". Cây cân bằng không tự cài đặt vì không yêu cầu nào buộc phải giữ thứ tự khi dữ liệu thay đổi dày; điều kiện sẽ cần được nêu ở các mục 3 và 5.
## 9. Phần biện minh cá nhân
### 9.1. Nguyễn Hữu Thịnh

| Thành phần sở hữu | MC1: Bảng băm tự cài đặt MyHashTable, kết hợp tầng Persistence để nạp dữ liệu từ CSV và ghi dữ liệu trở lại CSV. |
| --- | --- |
| Yêu cầu em phân tích | MC1 tập trung vào việc tra cứu nhanh đơn thuê, gồm hai dạng chính: tra theo mã đơn BookingID (1-1) và tra theo biển số xe bien_so (1-N). Ngoài ra, khi dữ liệu thay đổi như thêm đơn, sửa thông tin xe, sửa ngày thuê hoặc hủy đơn, các chỉ mục phải được cập nhật đồng bộ để kết quả tra cứu luôn chính xác. Hệ thống cũng phải hỗ trợ lưu trữ lâu dài thông qua CSV, tức dữ liệu trong bộ nhớ và dữ liệu trên file phải được đồng bộ khi thực hiện lưu. |
| Quyết định thiết kế em bảo vệ | Em chọn bảng băm tự cài đặt MyHashTable dùng nối chuỗi vì MC1 chủ yếu cần tra cứu chính xác theo khóa.BookingID dùng để tìm một đơn, còn bien_so dùng để tìm nhiều đơn của cùng một xe. Các thao tác thêm, tìm, xóa có độ phức tạp trung bình O(1). Khi hệ số tải vượt 0,75, bảng được mở rộng và băm lại. Với 10.000 đơn, hệ số tải khoảng 0,62, chuỗi dài nhất 4, cho thấy dữ liệu được phân bố khá tốt. |
| Đánh đổi và điều kiện làm nó không còn đúng | Đánh đổi: mất duyệt theo thứ tự hoặc khoảng, phải giữ hai chỉ mục đồng bộ. Không còn đúng khi cần tra theo khoảng hoặc tiền tố trên cùng khóa, hoặc cần bảo đảm thời gian xấu nhất. |
| Nếu yêu cầu thay đổi thì sao | Giữ MyHashTable cho tra cứu chính xác, đồng thời bổ sung Trie nếu cần tìm theo tiền tố hoặc cây cân bằng nếu cần truy vấn khoảng, theo thứ tự. Các cấu trúc phải được cập nhật đồng bộ khi thêm, sửa, xóa. |

### 9.2. Nguyễn Minh Duy : MC2

| Thành phần sở hữu | MC2:Cài đặt Sorted Array, Binary Search và Merge Sort cho chức năng lọc theo thời gian và Top xe. |
| --- | --- |
| Yêu cầu mình phân tích | MC2 (Range + Extremes): Lọc danh sách xe theo một khoảng thời gian cụ thể (để biết xe đang bận/trống) và truy xuất top các xe được thuê nhiều nhất từ tập dữ liệu lịch sử tích lũy quy mô lớn. |
| Quyết định thiết kế mình bảo vệ | Sử dụng Sorted Array kết hợp Binary Search (nạp dữ liệu bằng Merge Sort).<br>-Thứ tự: Bắt buộc cần có thứ tự để giải quyết truy vấn theo khoảng (Range) và trích xuất cực trị (Extremes).<br>-Workload: Dữ liệu truy vấn chủ yếu là lịch sử thuê xe (bulk load ban đầu). Workload nghiêng hoàn toàn về đọc (Read-heavy). Sorted Array lưu trữ liên tiếp trên bộ nhớ nên rất thân thiện với CPU cache, cho phép tìm vị trí đầu khoảng bằng Binary Search cực nhanh O(log N) và duyệt k phần tử tiếp theo trong O(k). |
| Đánh đổi và điều kiện làm nó không còn đúng | Đánh đổi: Hy sinh hiệu năng chèn/xóa để tối ưu tuyệt đối cho tốc độ đọc/lọc. Chèn một đơn thuê mới vào giữa mảng sẽ tốn chi phí O(N) do phải dịch chuyển các phần tử. <br>Điều kiện không còn : Nếu mô hình kinh doanh thayổi, hệ thống yêu cầu chèn và cập nhật đơn thuê liên tục theo thời gian thực (high-frequency inserts) thay vì nạp dữ liệu lịch sử theo lô, chi phí O(N) sẽ gây suy giảm hiệu năng (degradation) toàn hệ thống. |
| Nếu yêu cầu thay đổi thì sao | Nếu ứng dụng chuyển sang yêu cầu cập nhật giao dịch liên tục thời gian thực mà vẫn cần truy vấn Range/Top, mình sẽ chuyển sang dùng Cây nhị phân tìm kiếm tự cân bằng (Balanced BST). Cấu trúc này sẽ cân bằng lại trade-off, giúp cả thao tác cập nhật (chèn/xóa) và thao tác truy vấn khoảng đều duy trì ổn định ở mức O(log N). |

### 9.3. Lê Đức Thuần: RF1

| Thành phần sở hữu | RF1 — Tính năng Autocomplete gợi ý tên hãng/dòng xe, bao gồm component CarTrie, implementation trong Trie.h và bộ kiểm thử TrieTest.cpp. Thành phần này chịu trách nhiệm tìm kiếm xe theo tiền tố và trả về danh sách tên xe phù hợp. |
| --- | --- |
| Yêu cầu mình phân tích | RF1 cần hỗ trợ người dùng nhập một phần tên hãng hoặc dòng xe và nhận được các gợi ý tương ứng. Truy vấn được thực hiện nhiều lần khi người dùng gõ từng ký tự. Ngoài ra, hệ thống cần xử lý chữ hoa/chữ thường, khoảng trắng đầu/cuối, prefix không tồn tại và trường hợp một prefix khớp với nhiều xe. Kết quả cần có thứ tự từ điển A-Z để hiển thị ổn định |
| Quyết định thiết kế mình bảo vệ | Mình lựa chọn Trie (cây tiền tố) vì access pattern chính của RF1 là tìm kiếm theo prefix. Trie cho phép đi từ node gốc theo từng ký tự của prefix với chi phí O(L), trong đó L là độ dài prefix. Sau khi tìm được node tương ứng, hệ thống duyệt cây con để thu thập các tên xe phù hợp. Implementation sử dụng normalize() để chuẩn hóa input, đồng thời chèn cả tên đầy đủ và các khóa dòng xe để người dùng có thể tìm trực tiếp bằng tên model, ví dụ inno → Toyota Innova. |
| Đánh đổi và điều kiện làm nó không còn đúng | Trie sử dụng nhiều node và con trỏ nên tốn bộ nhớ hơn cách lưu danh sách/mảng đơn giản. Việc chèn thêm các khóa phụ cho dòng xe cũng làm tăng số lượng node/đường đi trong Trie. unordered_map giúp quản lý các node con linh hoạt nhưng thứ tự duyệt không được đảm bảo, vì vậy kết quả phải được sort() lại theo A-Z. Lựa chọn Trie sẽ kém cần thiết nếu danh mục xe rất nhỏ và số lần tìm kiếm ít, khi đó duyệt tuyến tính có thể đơn giản hơn. |
| Nếu yêu cầu thay đổi thì sao | Nếu yêu cầu chỉ tìm theo tên đầy đủ và danh mục rất nhỏ, có thể chuyển sang danh sách/mảng kết hợp tìm tuyến tính. Nếu cần sắp xếp gợi ý theo độ phổ biến, Trie hiện tại cần bổ sung thông tin về số lượt sử dụng hoặc cơ chế ranking. Nếu cần tìm chuỗi ở giữa tên xe thay vì chỉ tìm theo prefix, hoặc hỗ trợ tìm kiếm gần đúng/sai chính tả, cần cân nhắc cấu trúc hoặc thuật toán tìm kiếm khác. Nếu cần hỗ trợ tiếng Việt có dấu một cách đầy đủ, hàm chuẩn hóa hiện tại cũng cần được mở rộng. |

### 9.4. Lý Nguyễn Mạnh Thuyên: RF2

| Thành phần sở hữu | RF2 – Xử lý ưu tiên khi tranh xe; MyMaxHeap (Max-Heap) + indexMap ánh xạ BookingID → Heap Index. |
| --- | --- |
| Yêu cầu mình phân tích | Khi nhiều khách hàng cùng đặt một xe và xảy ra tranh chấp, hệ thống cần xác định yêu cầu thuê xe nào được ưu tiên xử lý trước dựa trên mức độ ưu tiên của khách hàng và thời gian đặt xe. Ngoài ra, hệ thống cần hỗ trợ tìm kiếm, hủy yêu cầu theo BookingID và cập nhật thứ tự ưu tiên khi dữ liệu thay đổi. |
| Quyết định thiết kế mình bảo vệ | Sử dụng cấu trúc Binary Max-Heap để luôn đưa yêu cầu thuê xe có độ ưu tiên cao nhất lên đầu Heap. Kết hợp indexMap để ánh xạ từ BookingID đến vị trí tương ứng trong Heap, giúp tìm kiếm, hủy và cập nhật yêu cầu nhanh chóng. Khi thứ tự ưu tiên thay đổi, hệ thống sử dụng SiftUp hoặc SiftDown để duy trì tính chất Max-Heap. |
| Đánh đổi và điều kiện làm nó không còn đúng | Max-Heap phù hợp khi hệ thống thường xuyên thêm yêu cầu, lấy yêu cầu ưu tiên cao nhất hoặc hủy yêu cầu theo BookingID. Tuy nhiên, Heap không giữ toàn bộ dữ liệu theo thứ tự đã sắp xếp, nên không thuận tiện khi cần duyệt danh sách theo thứ tự hoàn chỉnh. Thiết kế cũng phụ thuộc vào việc cập nhật indexMap chính xác sau mỗi lần hoán đổi phần tử và xác định quy tắc ưu tiên rõ ràng. Nếu chỉ cần sắp xếp dữ liệu một lần rồi đọc nhiều lần, sử dụng thuật toán Sort có thể phù hợp hơn. |
| Nếu yêu cầu thay đổi thì sao | Nếu hệ thống chuyển sang yêu cầu hiển thị toàn bộ danh sách theo thứ tự ưu tiên, có thể sao chép dữ liệu từ Heap rồi Sort để hiển thị mà không làm thay đổi Heap gốc. Nếu cần tìm kiếm theo nhiều tiêu chí hoặc lọc dữ liệu phức tạp, có thể kết hợp thêm cấu trúc dữ liệu phù hợp. Nếu quy tắc ưu tiên thay đổi, cần điều chỉnh hàm so sánh ưu tiên và xây dựng lại Heap hoặc cập nhật các phần tử liên quan để bảo đảm thứ tự chính xác. |

### 9.5. Lê Nguyễn Minh Thư: RF3

| Thành phần sở hữu | RF3 - MyStack + UndoManager, Automated Test và BenchMark. |
| --- | --- |
| Yêu cầu mình phân tích | RF3: Hoàn tác thao tác tạo, sửa, xóa đơn thuê và khôi phục dữ liệu về trạng thái trước đó |
| Quyết định thiết kế mình bảo vệ | Chọn Stack theo LIFO để lưu lịch sử thao tác; thao tác gần nhất được Undo trước, phù hợp với yêu cầu hoàn tác. |
| Đánh đổi và điều kiện làm nó không còn đúng | Stack có ưu điểm Undo thao tác gần nhất nhanh, đơn giản, nhưng chỉ quản lý lịch sử theo LIFO và dữ liệu lịch sử nằm trong bộ nhớ. Nếu chương trình đóng thì lịch sử Undo bị mất. Ngoài ra, khi số lượng thao tác rất lớn, bộ nhớ sử dụng tăng theo số thao tác. Thiết kế này không còn phù hợp nếu hệ thống yêu cầu Redo, Undo theo nhiều cấp phức tạp hoặc lưu lịch sử sau khi khởi động lại |
| Nếu yêu cầu thay đổi thì sao | Nếu cần Redo, bổ sung thêm một RedoStack . Nếu cần lưu lịch sử sau khi chương trình đóng, chuyển lịch sử Undo sang cơ chế lưu file/database. Nếu cần Undo nhiều loại đối tượng hoặc nhiều người dùng, có thể mở rộng Action và quản lý lịch sử riêng theo phiên/người dùng. |
