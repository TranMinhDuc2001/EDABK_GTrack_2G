import csv

# Đường dẫn đến file CSV đầu vào và file CSV đầu ra
input_file = 'output10-3.csv'
output_file = 'LatLongData10-3.csv'

# List để lưu trữ các giá trị lat và long
latitudes = []
longitudes = []

# Mở file CSV đầu vào và đọc từng dòng
with open(input_file, 'r', newline='') as csvfile:
    reader = csv.reader(csvfile)
    
    # Duyệt qua từng hàng trong file CSV
    for row in reader:
        # Lấy giá trị lat và long từ cột thứ 3 và 4 (index 2 và 3)
        latitudes.append(row[2])
        longitudes.append(row[3])

# Ghi các giá trị lat và long vào file CSV mới
with open(output_file, 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    
    # Duyệt qua từng cặp lat và long và ghi vào file CSV mới
    for lat, long in zip(latitudes, longitudes):
        writer.writerow([lat, long])

print("Dữ liệu đã được ghi vào file CSV mới:", output_file)
