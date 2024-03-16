import csv
from datetime import datetime, timedelta



# Hàm chuyển đổi thời gian từ UTC+0 sang UTC+7
def convert_to_utc7(date_str, time_str):
    # Tạo đối tượng datetime từ ngày và giờ
    datetime_obj = datetime.strptime(date_str + time_str, '%d%m%y%H%M%S')
    
    # Thêm múi giờ UTC+7
    datetime_obj += timedelta(hours=7)
    
    # Chuyển đổi datetime thành string với định dạng mong muốn
    return datetime_obj.strftime('%d/%m/%Y,%H:%M:%S')

# Mở file CSV đầu vào và tạo một file CSV mới để ghi dữ liệu đã xử lý
with open('data1.csv', 'r', newline='') as infile, open('output10-3.csv', 'w', newline='') as outfile:



    reader = csv.reader(infile)
    
    # Duyệt qua từng hàng trong file CSV
    for row in reader:
        # Chuyển đổi thời gian từ UTC+0 sang UTC+7 cho cột đầu tiên
        row[0] = convert_to_utc7(row[0], row[1])  # Thực hiện chuyển đổi cho cột ngày và giờ
        del row[1]  # Xóa cột giờ vì chúng ta đã chuyển đổi xong
        
        # Ghi hàng đã xử lý vào file CSV mới
        outfile.write(','.join(row) + '\n')

