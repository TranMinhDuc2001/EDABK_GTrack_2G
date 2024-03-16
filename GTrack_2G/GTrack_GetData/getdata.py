import csv

# Hàm loại bỏ dấu ngoặc kép từ dòng dữ liệu
def remove_quotes_from_line(line):
    return line.replace('"', '')

# Mở file CSV đầu vào và tạo một file CSV mới để ghi dữ liệu đã xử lý
with open('device3-20240310-0858.csv', 'r', newline='') as infile, open('data1.csv', 'w', newline='') as outfile:
    # Đọc dữ liệu từ file đầu vào và ghi vào file mới
    outfile.write(remove_quotes_from_line(infile.read()))

print("Dữ liệu đã được ghi vào file CSV mới.")
